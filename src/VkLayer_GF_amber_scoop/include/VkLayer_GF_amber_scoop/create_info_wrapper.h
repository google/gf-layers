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

#ifndef VKLAYER_GF_AMBER_SCOOP_CREATE_INFO_WRAPPER_H
#define VKLAYER_GF_AMBER_SCOOP_CREATE_INFO_WRAPPER_H

#include <memory>
#include <vector>

#include "VkLayer_GF_amber_scoop/vk_deep_copy.h"

namespace gf_layers::amber_scoop_layer {

// This class is used to store Vulkan's create info structs that needs to be
// deep copied, i.e. structs that contain pointers to other structs. DeepCopy is
// called in the constructor and DeepDelete in the destructor. The class can be
// inherited if more functionality is needed. There must be a DeepCopy and
// DeepDelete functions for the given |CreateInfoType|.
template <typename CreateInfoType>
class CreateInfoWrapper {
 public:
  explicit CreateInfoWrapper(const CreateInfoType& create_info)
      : create_info_(std::make_unique<CreateInfoType>(DeepCopy(create_info))) {}

  // Default move constructor and move assign operator. Moves the ownership of
  // the |create_info_| to the other object.
  CreateInfoWrapper(CreateInfoWrapper&& other) noexcept = default;
  CreateInfoWrapper& operator=(CreateInfoWrapper&& other) noexcept = default;

  virtual ~CreateInfoWrapper() {
    // |create_info| may be nullptr if the ownership has been moved.
    if (create_info_ != nullptr) {
      DeepDelete(*create_info_);
    }
  }

  // Disabled copy constructor and copy assign operator.
  CreateInfoWrapper(const CreateInfoWrapper&) = delete;
  CreateInfoWrapper& operator=(const CreateInfoWrapper&) = delete;

  // Returns the create info struct.
  [[nodiscard]] const CreateInfoType& GetCreateInfo() const {
    return *create_info_;
  }

 private:
  std::unique_ptr<CreateInfoType> create_info_;
};

// Type aliases for create info structs wrapped to CreateInfoWrapper.

using BufferData = CreateInfoWrapper<VkBufferCreateInfo>;
using DescriptorSetLayoutData =
    CreateInfoWrapper<VkDescriptorSetLayoutCreateInfo>;
using PipelineLayoutData = CreateInfoWrapper<VkPipelineLayoutCreateInfo>;
using ShaderModuleData = CreateInfoWrapper<VkShaderModuleCreateInfo>;

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_CREATE_INFO_WRAPPER_H
