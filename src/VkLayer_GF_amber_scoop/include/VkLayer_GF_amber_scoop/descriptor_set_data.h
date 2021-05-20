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

#ifndef VKLAYER_GF_AMBER_SCOOP_DESCRIPTOR_SET_DATA_H
#define VKLAYER_GF_AMBER_SCOOP_DESCRIPTOR_SET_DATA_H

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "VkLayer_GF_amber_scoop/create_info_wrapper.h"

namespace gf_layers::amber_scoop_layer {

// This class is used to store descriptor set's state.
class DescriptorSetData {
 public:
  // Type alias used to clarify the maps' key types.
  using BindingNumber = uint32_t;

  // Flattened descriptor types. Used f.ex. to easily choose where the
  // descriptor writes are taken from and where they are stored.
  enum FlattenedDescriptorType {
    IMAGE_SAMPLER,
    BUFFER,
    TEXEL_BUFFER,
    UNSUPPORTED
  };

  // Constructor. Stores the given pointer to the layout used to allocate this
  // descriptor set and allocates the vectors where the information about
  // descriptors will be stored when updating the descriptor set.
  explicit DescriptorSetData(const DescriptorSetLayoutData* layout_data);

  // Returns the descriptor bindings of buffer type.
  [[nodiscard]] const std::unordered_map<BindingNumber,
                                         std::vector<VkDescriptorBufferInfo>>*
  GetDescriptorBufferBindings() const {
    return &descriptor_buffer_bindings_;
  }

  // Returns pointer to the layout used to create this descriptor set.
  [[nodiscard]] const DescriptorSetLayoutData& GetDescriptorSetLayoutData()
      const {
    return *descriptor_set_layout_data_;
  }

  // Updates the descriptor sets' state with the given VkWriteDescriptorSet.
  void WriteDescriptorSet(const VkWriteDescriptorSet& write_descriptor_set);

  // TODO(ilkkasaa): implement CopyDescriptorSet function.

 private:
  // Layout used to allocate this descriptor set.
  std::unique_ptr<DescriptorSetLayoutData> descriptor_set_layout_data_;
  // |VkDescriptorBufferInfo| for each descriptor of "buffer" type.
  std::unordered_map<BindingNumber, std::vector<VkDescriptorBufferInfo>>
      descriptor_buffer_bindings_;
  // Image and sampler bindings. Not used yet.
  std::unordered_map<BindingNumber, std::vector<VkDescriptorImageInfo>>
      image_and_sampler_bindings_;
  // Texel buffer bindings. Not used yet.
  std::unordered_map<BindingNumber, std::vector<VkBufferView>>
      texel_buffer_view_bindings_;
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_DESCRIPTOR_SET_DATA_H
