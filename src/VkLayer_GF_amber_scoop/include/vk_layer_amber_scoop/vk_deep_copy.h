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

#ifndef VK_LAYER_AMBER_SCOOP_VK_DEEP_COPY_H
#define VK_LAYER_AMBER_SCOOP_VK_DEEP_COPY_H

#include <vulkan/vulkan.h>

#include <cstdint>

namespace gf_layers::amber_scoop_layer {

template <typename T>
T* CopyArray(T const* p_data, uint32_t num_elements, uint32_t offset) {
  if (p_data == nullptr) {
    return nullptr;
  }
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  T* result = new T[num_elements - offset];
  for (uint32_t i = 0; i < num_elements; i++) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    result[i] = p_data[i + offset];
  }
  return result;
}

template <typename T>
T* CopyArray(T const* p_data, uint32_t num_elements) {
  return CopyArray(p_data, num_elements, 0);
}

VkRenderPassBeginInfo DeepCopy(
    VkRenderPassBeginInfo const* p_render_pass_begin_info);

void DeepDelete(VkRenderPassBeginInfo const* p_render_pass_begin_info);

}  // namespace gf_layers::amber_scoop_layer

#endif  // VK_LAYER_AMBER_SCOOP_VK_DEEP_COPY_H
