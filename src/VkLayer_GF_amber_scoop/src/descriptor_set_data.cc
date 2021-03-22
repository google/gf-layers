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

#include "VkLayer_GF_amber_scoop/descriptor_set_data.h"

// <algorithm> is used by GCC's libstdc++, but not LLVM's libc++.
#include <algorithm>  // IWYU pragma: keep
// <cstdio> and <cstdlib> required by LOG and ASSERT macros, but IWYU doesn't
// notice it, so we need to manually keep the includes.
#include <cstdio>   // IWYU pragma: keep
#include <cstdlib>  // IWYU pragma: keep

#include "gf_layers_layer_util/logging.h"

namespace gf_layers::amber_scoop_layer {
namespace {

// Returns a flattened descriptor types. See documentation of the
// |FlattenedDescriptorType| enum for more info.
DescriptorSetData::FlattenedDescriptorType GetFlattenedDescriptorType(
    VkDescriptorType descriptor_type) {
  switch (descriptor_type) {
    case VK_DESCRIPTOR_TYPE_SAMPLER:
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
      // If descriptorType is any of the types above, pImageInfo must be a
      // valid pointer to an array of descriptorCount valid
      // VkDescriptorImageInfo structures
      return DescriptorSetData::IMAGE_SAMPLER;
    }
    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: {
      // If descriptorType is any of above types, each element of
      // pTexelBufferView must be either a valid VkBufferView handle (or
      // VK_NULL_HANDLE if the nullDescriptor feature is enabled).
      return DescriptorSetData::TEXEL_BUFFER;
    }
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: {
      // If descriptorType is any of the types above, pBufferInfo must be a
      // valid pointer to an array of descriptorCount valid
      // VkDescriptorBufferInfo structures.
      return DescriptorSetData::BUFFER;
    }
    case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT:
    case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
      return DescriptorSetData::UNSUPPORTED;
    case VK_DESCRIPTOR_TYPE_MAX_ENUM: {
      RUNTIME_ASSERT_MSG(false, "Should be unreachable.");
    }
  }
}
}  // namespace

DescriptorSetData::DescriptorSetData(const DescriptorSetLayoutData* layout_data)
    : descriptor_set_layout_data_(layout_data) {
  const VkDescriptorSetLayoutCreateInfo& create_info =
      layout_data->GetCreateInfo();

  // Initialize buffer/image/sampler binding vectors with correct sizes.
  for (uint32_t binding_number = 0; binding_number < create_info.bindingCount;
       binding_number++) {
    const VkDescriptorSetLayoutBinding& binding =
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        create_info.pBindings[binding_number];
    switch (GetFlattenedDescriptorType(binding.descriptorType)) {
      case IMAGE_SAMPLER:
        DEBUG_ASSERT_MSG(false, "Images and samplers are not implemented.");
        break;
      case BUFFER: {
        descriptor_buffer_bindings_[binding_number] =
            std::vector<VkDescriptorBufferInfo>(binding.descriptorCount);
        break;
      }
      case TEXEL_BUFFER:
        DEBUG_ASSERT_MSG(false, "Texel buffers not implemented.");
        break;
      case UNSUPPORTED:
        DEBUG_ASSERT_MSG(false, "Unsupported buffer type.");
        break;
    }
  }
}

void DescriptorSetData::WriteDescriptorSet(
    const VkWriteDescriptorSet& write_descriptor_set) {
  // From the Vulkan spec:
  // If VkDescriptorSetLayoutBinding for dstSet at dstBinding is not equal
  // to VK_DESCRIPTOR_TYPE_MUTABLE_VALVE, descriptorType must be the same
  // type as that specified in VkDescriptorSetLayoutBinding for dstSet at
  // dstBinding.

  const VkDescriptorSetLayoutBinding& binding =
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      descriptor_set_layout_data_->GetCreateInfo()
          .pBindings[write_descriptor_set.dstBinding];
  DEBUG_ASSERT_MSG(
      binding.descriptorType == write_descriptor_set.descriptorType,
      "Write descriptorType and descriptor's type defined in "
      "VkDescriptorSetLayout doesn't match.");

  switch (GetFlattenedDescriptorType(write_descriptor_set.descriptorType)) {
    case IMAGE_SAMPLER:
      // TODO(ilkkasaa): implement images and samplers here.
      RUNTIME_ASSERT_MSG(false, "Images and samplers are not implemented.");
    case BUFFER: {
      // From the Vulkan spec:
      // If the dstBinding has fewer than descriptorCount array
      // elements remaining starting from dstArrayElement, then the remainder
      // will be used to update the subsequent binding - dstBinding+1 starting
      // at array element zero. If a binding has a descriptorCount of zero, it
      // is skipped. This behavior applies recursively, with the update
      // affecting consecutive bindings as needed to update all descriptorCount
      // descriptors.
      uint32_t binding_idx = write_descriptor_set.dstBinding;
      uint32_t array_element = write_descriptor_set.dstArrayElement;
      for (uint32_t descriptor_read_idx = 0;
           descriptor_read_idx < write_descriptor_set.descriptorCount;
           descriptor_read_idx++) {
        // Jump to the next binding if the current binding has no more array
        // elements left. This will also skip bindings with descriptorCount of
        // zero. See the quote from the Vulkan spec above.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        while (array_element >= descriptor_set_layout_data_->GetCreateInfo()
                                    .pBindings[binding_idx]
                                    .descriptorCount) {
          binding_idx++;
          // Start from next binding's first array element.
          array_element = 0;
          DEBUG_ASSERT_MSG(
              binding_idx <
                  descriptor_set_layout_data_->GetCreateInfo().bindingCount,
              "Descriptor set binding overflow.");
        }
        descriptor_buffer_bindings_[binding_idx][array_element] =
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            write_descriptor_set.pBufferInfo[descriptor_read_idx];
        // Jump to the next array element.
        array_element++;
      }
      break;
    }
    case TEXEL_BUFFER:
      // TODO(ilkkasaa): implement uniform / storage texel buffers.
      RUNTIME_ASSERT_MSG(
          false, "Uniform / storage texel buffers are not implemented.");
    case UNSUPPORTED:
      break;
  }
}

}  // namespace gf_layers::amber_scoop_layer
