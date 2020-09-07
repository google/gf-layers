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

#include "vk_layer_amber_scoop/vk_deep_copy.h"

namespace gf_layers::amber_scoop_layer {

VkRenderPassBeginInfo DeepCopy(
    VkRenderPassBeginInfo const* p_render_pass_begin_info) {
  VkRenderPassBeginInfo result = *p_render_pass_begin_info;
  result.pClearValues = CopyArray(p_render_pass_begin_info->pClearValues,
                                  p_render_pass_begin_info->clearValueCount);
  return result;
}

void DeepDelete(VkRenderPassBeginInfo const* p_render_pass_begin_info) {
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] p_render_pass_begin_info->pClearValues;
}

}  // namespace gf_layers::amber_scoop_layer
