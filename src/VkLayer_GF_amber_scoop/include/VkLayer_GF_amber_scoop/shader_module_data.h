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

#ifndef VKLAYER_GF_AMBER_SCOOP_SHADER_MODULE_DATA_H
#define VKLAYER_GF_AMBER_SCOOP_SHADER_MODULE_DATA_H

#include <gf_layers_layer_util/util.h>

#include <memory>
#include <set>
#include <vector>

#include "VkLayer_GF_amber_scoop/vulkan_commands.h"

namespace gf_layers::amber_scoop_layer {

// This class is used to track lifetime of a shader module. Shader modules can
// be destroyed but still be used by some pipelines. This class is used to
// detect when the intercepted shader module can be freed from the memory: it
// can be freed once the shader module is destroyed and it's not used by any
// pipeline. The class is thread-safe.
class ShaderModuleData {
 public:
  explicit ShaderModuleData(VkShaderModuleCreateInfo create_info)
      : create_info_(create_info) {}

  ~ShaderModuleData() { DeepDelete(create_info_); }

  // Disabled copy/move constructors and copy/move assign operators
  ShaderModuleData(const ShaderModuleData&) = delete;
  ShaderModuleData(ShaderModuleData&&) = delete;
  ShaderModuleData& operator=(const ShaderModuleData&) = delete;
  ShaderModuleData& operator=(ShaderModuleData&&) = delete;

  // Marks the given |vkPipeline| as user of this shader module.
  void AddPipeline(VkPipeline vkPipeline) {
    ScopedLock lock(mutex_);
    pipelines_.insert(vkPipeline);
  }

  // Returns the create info struct.
  const VkShaderModuleCreateInfo& GetCreateInfo() { return create_info_; }

  // Returns true if the shader modules is in use by at least one pipeline.
  [[nodiscard]] bool IsInUse() {
    ScopedLock lock(mutex_);
    return !pipelines_.empty() && !is_destroyed_;
  }

  // Marks this shader module as deleted. This should be called from
  // vkDestroyShaderModule function.
  void SetDestroyed() { is_destroyed_ = true; }

 private:
  // Create info of this shader module.
  VkShaderModuleCreateInfo create_info_;
  // Flag to determine if this shader module has been destroyed via
  // vkDestroyShaderModule.
  bool is_destroyed_ = false;
  // Pipelines using this shader module.
  std::set<VkPipeline> pipelines_;

  MutexType mutex_;
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_SHADER_MODULE_DATA_H
