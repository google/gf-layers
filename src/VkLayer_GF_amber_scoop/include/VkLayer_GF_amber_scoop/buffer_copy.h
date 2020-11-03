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

#ifndef VKLAYER_GF_AMBER_SCOOP_BUFFER_COPY_H
#define VKLAYER_GF_AMBER_SCOOP_BUFFER_COPY_H

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

#include "VkLayer_GF_amber_scoop/amber_scoop_layer.h"
#include "VkLayer_GF_amber_scoop/vulkan_commands.h"

namespace gf_layers::amber_scoop_layer {

// Class used to copy contents of a Vulkan buffer to a host visible memory. The
// source buffer must be created with |VK_BUFFER_USAGE_TRANSFER_SRC_BIT|. The
// layer sets that flag in |vkCreateBuffer| for all buffers we are interested
// in. The copy operation is executed in the constructor. The data can be
// accessed via |GetCopiedData()| function. Remaining Vulkan objects are deleted
// in the destructor.
class BufferCopy {
 public:
  // Starts the copy operation and waits until finished.
  //
  // |device_data| device data reference.
  // |buffer| Vulkan buffer where the data is copied from.
  // |buffer_size| Size of the source buffer.
  // |queue| Vulkan queue where the commands will be submitted to.
  // |command_pool| command pool used to allocate the commands.
  // |pipeline_barriers| list of pipeline barriers that must be used to
  // synchronize access to the source buffer.
  BufferCopy(DeviceData* device_data, const VkBuffer& buffer,
             VkDeviceSize buffer_size, VkQueue queue,
             VkCommandPool command_pool,
             const std::vector<const CmdPipelineBarrier*>& pipeline_barriers);

  // Releases remaining Vulkan resources.
  ~BufferCopy();

  // Disabled copy/move constructors and copy/move assign operators.
  BufferCopy(const BufferCopy&) = delete;
  BufferCopy(BufferCopy&&) = delete;
  BufferCopy& operator=(const BufferCopy&) = delete;
  BufferCopy& operator=(BufferCopy&&) = delete;

  // Get a memory view to the copied data.
  [[nodiscard]] const absl::Span<const char>& GetCopiedData() const {
    return copied_data_span_;
  }

 private:
  // Pointer to copied data.
  void* copied_data_ = nullptr;
  // Memory view to the copied data.
  absl::Span<const char> copied_data_span_;

  // Vulkan objects used
  VkBuffer buffer_copy_ = VK_NULL_HANDLE;
  VkDeviceMemory buffer_copy_memory_ = VK_NULL_HANDLE;
  DeviceData* device_data_;

  // Find a memory type in |memory_type_bits| that includes all of
  // |required_properties|.
  // |memory_type_bits| is bit field from |VkMemoryRequirements.memoryTypeBits|.
  uint32_t FindMemoryType(uint32_t memory_type_bits,
                          VkMemoryPropertyFlags required_properties);
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_BUFFER_COPY_H
