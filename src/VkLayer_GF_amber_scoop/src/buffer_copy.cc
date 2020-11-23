// Copyright 2020 The gf-layers Project Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "VkLayer_GF_amber_scoop/buffer_copy.h"

#include "VkLayer_GF_amber_scoop/vulkan_util.h"
#include "gf_layers_layer_util/logging.h"
#include "gf_layers_layer_util/util.h"

namespace gf_layers::amber_scoop_layer {

BufferCopy::BufferCopy(DeviceData* device_data, const VkBuffer& buffer,
                       VkDeviceSize buffer_size, VkQueue queue,
                       VkCommandPool command_pool)
    : device_data_(device_data) {
  VkDevice device = device_data_->device;

  // Create a buffer where the data will be copied to.
  {
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = buffer_size;
    create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    RequireSuccess(device_data_->vkCreateBuffer(device, &create_info, nullptr,
                                                &buffer_copy_),
                   "Failed to create buffer for the copied data.");
  }

  VkMemoryRequirements memory_requirements;
  device_data_->vkGetBufferMemoryRequirements(device, buffer_copy_,
                                              &memory_requirements);
  {
    VkMemoryAllocateInfo vkMemoryAllocateInfo = {};
    vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkMemoryAllocateInfo.allocationSize = memory_requirements.size;
    vkMemoryAllocateInfo.memoryTypeIndex =
        FindMemoryType(memory_requirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    RequireSuccess(
        device_data_->vkAllocateMemory(device, &vkMemoryAllocateInfo, nullptr,
                                       &buffer_copy_memory_),
        "Failed to allocate memory for buffer copy.");
  }
  RequireSuccess(device_data_->vkBindBufferMemory(device, buffer_copy_,
                                                  buffer_copy_memory_, 0),
                 "Failed binding memory for buffer copy.");

  // Create command buffer.
  VkCommandBuffer command_buffer = nullptr;
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
  command_buffer_allocate_info.sType =
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.commandPool = command_pool;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandBufferCount = 1;
  RequireSuccess(device_data_->vkAllocateCommandBuffers(
                     device, &command_buffer_allocate_info, &command_buffer),
                 "Failed to allocate command buffers.");

  // We have created a dispatchable object, so we must set the dispatch table
  // pointer.
  CopyDispatchTablePointer(device, command_buffer);

  // Record to the command buffer.
  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  RequireSuccess(
      device_data_->vkBeginCommandBuffer(command_buffer, &begin_info),
      "Failed to begin command buffer.");

  // Memory barrier for making sure we can safely transfer the data to a host
  // visible memory.
  // TODO(ilkkasaa): This barrier could be submitted only before the first
  //   buffer copy operation. Current implementation submits the same barrier
  //   for every buffer copy operation which is redundant.
  VkMemoryBarrier memory_barrier;
  memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
  memory_barrier.pNext = nullptr;
  memory_barrier.srcAccessMask =
      VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
      VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_WRITE_BIT;
  memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

  device_data_->vkCmdPipelineBarrier(
      command_buffer,                      // commandBuffer
      VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,  // srcStageMask
      VK_PIPELINE_STAGE_TRANSFER_BIT,      // dstStageMask
      0,                                   // dependencyFlags
      1U,                                  // memoryBarrierCount
      &memory_barrier,                     // pMemoryBarriers
      0,                                   // bufferMemoryBarrierCount
      nullptr,                             // pBufferMemoryBarriers
      0,                                   // imageMemoryBarrierCount
      nullptr);                            // pImageMemoryBarriers

  // Copy buffer.
  VkBufferCopy copy_region;
  copy_region.srcOffset = 0;
  copy_region.dstOffset = 0;
  copy_region.size = buffer_size;
  device_data_->vkCmdCopyBuffer(command_buffer, buffer, buffer_copy_, 1,
                                &copy_region);

  // Memory barrier for making sure we can safely read the copied data from
  // the host.
  VkMemoryBarrier host_copy_barrier = {};
  host_copy_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
  host_copy_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  host_copy_barrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;

  device_data_->vkCmdPipelineBarrier(
      command_buffer,                  // commandBuffer
      VK_PIPELINE_STAGE_TRANSFER_BIT,  // srcStageMask
      VK_PIPELINE_STAGE_HOST_BIT,      // dstStageMask
      0,                               // dependencyFlags
      1,                               // memoryBarrierCount
      &host_copy_barrier,              // pMemoryBarriers
      0,                               // bufferMemoryBarrierCount
      nullptr,                         // pBufferMemoryBarriers
      0,                               // imageMemoryBarrierCount
      nullptr);                        // pImageMemoryBarriers

  RequireSuccess(device_data_->vkEndCommandBuffer(command_buffer),
                 "Failed to record command buffer.");

  // Submit commands and wait for commands to be executed.
  {
    VkFence fence = nullptr;
    VkFenceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    device_data->vkCreateFence(device, &create_info, nullptr, &fence);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    device_data_->vkQueueSubmit(queue, 1, &submit_info, fence);
    device_data_->vkWaitForFences(device, 1, &fence, VK_FALSE, ~0ULL);
    device_data_->vkDestroyFence(device, fence, nullptr);
  }

  // Map and invalidate memory to make it host visible.
  {
    RequireSuccess(device_data_->vkMapMemory(device, buffer_copy_memory_, 0,
                                             buffer_size, 0, &copied_data_),
                   "Failed to map memory.");
    VkMappedMemoryRange range_to_invalidate = {};
    range_to_invalidate.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range_to_invalidate.memory = buffer_copy_memory_;
    range_to_invalidate.offset = 0;
    range_to_invalidate.size = VK_WHOLE_SIZE;
    RequireSuccess(device_data_->vkInvalidateMappedMemoryRanges(
                       device, 1, &range_to_invalidate),
                   "Failed to invalidate memory.");
  }

  // Create a span for the copied data so it can be accessed more safely.
  copied_data_span_ =
      absl::MakeConstSpan(reinterpret_cast<char*>(copied_data_), buffer_size);

  // Free resources
  device_data_->vkFreeCommandBuffers(device_data_->device, command_pool, 1,
                                     &command_buffer);
}

BufferCopy::~BufferCopy() {
  // Unmap memory, free the memory and destroy the buffer.
  device_data_->vkUnmapMemory(device_data_->device, buffer_copy_memory_);
  device_data_->vkFreeMemory(device_data_->device, buffer_copy_memory_,
                             nullptr);
  device_data_->vkDestroyBuffer(device_data_->device, buffer_copy_, nullptr);
}

uint32_t BufferCopy::FindMemoryType(uint32_t memory_type_bits,
                                    VkMemoryPropertyFlags required_properties) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  device_data_->instance_data->vkGetPhysicalDeviceMemoryProperties(
      device_data_->physical_device, &memory_properties);

  for (uint32_t idx = 0; idx < memory_properties.memoryTypeCount; idx++) {
    if ((memory_type_bits & (1U << idx)) != 0 &&
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        (memory_properties.memoryTypes[idx].propertyFlags &
         required_properties) == required_properties) {
      return idx;
    }
  }
  LOG("Failed to find suitable memory type.");
  RUNTIME_ASSERT(false);
}

}  // namespace gf_layers::amber_scoop_layer
