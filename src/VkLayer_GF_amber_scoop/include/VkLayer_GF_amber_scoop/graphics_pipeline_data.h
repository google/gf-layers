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

#ifndef VKLAYER_GF_AMBER_SCOOP_GRAPHICS_PIPELINE_DATA_H
#define VKLAYER_GF_AMBER_SCOOP_GRAPHICS_PIPELINE_DATA_H

#include <vulkan/vulkan.h>

#include <memory>
#include <unordered_map>
#include <utility>

#include "VkLayer_GF_amber_scoop/create_info_wrapper.h"

namespace gf_layers::amber_scoop_layer {

// This class is used to store VkGraphicsPipelineCreateInfo struct and
// references to shader modules used by the pipeline.
class GraphicsPipelineData
    : public CreateInfoWrapper<VkGraphicsPipelineCreateInfo> {
 public:
  explicit GraphicsPipelineData(const VkGraphicsPipelineCreateInfo& create_info)
      : CreateInfoWrapper(create_info) {}

  ~GraphicsPipelineData() override;

  // Disabled copy/move constructors and copy/move assign operators.
  GraphicsPipelineData(const GraphicsPipelineData&) = delete;
  GraphicsPipelineData(GraphicsPipelineData&&) = delete;
  GraphicsPipelineData& operator=(const GraphicsPipelineData&) = delete;
  GraphicsPipelineData& operator=(GraphicsPipelineData&&) = delete;

  // Adds the given |shader module| to the map of shader modules used by this
  // pipeline. This function should be called from vkCreateGraphicsPipelines
  // function.
  void AddShaderModule(
      VkShaderModule shader_module,
      const std::shared_ptr<ShaderModuleData>& shader_module_data) {
    shader_modules_.emplace(shader_module, shader_module_data);
  }

  // Returns reference to the ShaderModuleData related to the given
  // |shader_module|. Returns nullptr if the given |shader_module| is not used
  // by this pipeline.
  ShaderModuleData* GetShaderModuleData(VkShaderModule shader_module) const {
    ShaderModuleData* result = nullptr;
    auto it = shader_modules_.find(shader_module);
    if (it != shader_modules_.end()) {
      result = it->second.get();
    }
    return result;
  }

 private:
  // Shader modules used by this graphics pipeline.
  std::unordered_map<VkShaderModule, std::shared_ptr<ShaderModuleData>>
      shader_modules_ = {};
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_GRAPHICS_PIPELINE_DATA_H
