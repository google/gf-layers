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

#ifndef VKLAYER_GF_AMBER_SCOOP_VK_DEEP_COPY_H
#define VKLAYER_GF_AMBER_SCOOP_VK_DEEP_COPY_H

#include <vulkan/vulkan.h>

#include <cstdint>

namespace gf_layers::amber_scoop_layer {

// Allocates a new array of type T with size of "num_elements" and copies data
// from the array "p_data" starting from element with the given offset. The
// allocated memory is not freed automatically. Use "delete[]" to free the
// memory.
template <typename T>
T* CopyArray(T const* p_data, uint64_t num_elements, uint64_t offset) {
  if (p_data == nullptr) {
    return nullptr;
  }
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  T* result = new T[num_elements - offset];
  for (uint64_t i = 0; i < num_elements; i++) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    result[i] = p_data[i + offset];
  }
  return result;
}

// Shortcut for CopyArray with zero offset.
template <typename T>
T* CopyArray(T const* p_data, uint64_t num_elements) {
  return CopyArray(p_data, num_elements, 0);
}

// Commands for making a deep / recursive copy of Vulkan structs. Each DeepCopy
// function has associated DeepDelete function for freeing the memory in the
// DeepCopy. Typically these functions are called from constructors and
// destructors of gf_layers::amber_scoop_layer::CmdXXXX structs. See
// vulkan_commands.h.

// Makes a deep copy of VkGraphicsPipelineCreateInfo struct. The allocated
// memory is not freed automatically. Use
// DeepDelete(VkGraphicsPipelineCreateInfo const*) to recursively free all
// the allocated memory.
VkGraphicsPipelineCreateInfo DeepCopy(
    const VkGraphicsPipelineCreateInfo& create_info);

// Recursively deletes all the allocated memory of the given
// VkGraphicsPipelineCreateInfo struct. Should be used only for structs
// created with associated DeepCopy(...) function.
void DeepDelete(const VkGraphicsPipelineCreateInfo& create_info);

// Makes a deep copy of VkPipelineShaderStageCreateInfo struct. The allocated
// memory is not freed automatically. Use
// DeepDelete(VkPipelineShaderStageCreateInfo const*) to recursively free all
// the allocated memory.
VkPipelineShaderStageCreateInfo DeepCopy(
    const VkPipelineShaderStageCreateInfo& create_info);

// Recursively deletes all the allocated memory of the given
// VkPipelineShaderStageCreateInfo struct. Should be used only for structs
// created with associated DeepCopy(...) function.
void DeepDelete(const VkPipelineShaderStageCreateInfo& create_info);

// Makes a deep copy of VkRenderPassBeginInfo struct. The allocated memory is
// not freed automatically. Use DeepDelete(VkRenderPassBeginInfo const*) to
// recursively free all the allocated memory.
VkRenderPassBeginInfo DeepCopy(
    VkRenderPassBeginInfo const* p_render_pass_begin_info);

// Recursively deletes all the allocated memory of the given
// VkRenderPassBeginInfo struct. Should be used only for structs created with
// associated DeepCopy(...) function.
void DeepDelete(VkRenderPassBeginInfo const* p_render_pass_begin_info);

// Makes a deep copy of VkShaderModuleCreateInfo struct. The allocated memory is
// not freed automatically. Use DeepDelete(VkShaderModuleCreateInfo const*) to
// recursively free all the allocated memory.
VkShaderModuleCreateInfo DeepCopy(const VkShaderModuleCreateInfo& create_info);

// Recursively deletes all the allocated memory of the given
// VkShaderModuleCreateInfo struct. Should be used only for structs created with
// associated DeepCopy(...) function.
void DeepDelete(const VkShaderModuleCreateInfo& create_info);

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_VK_DEEP_COPY_H
