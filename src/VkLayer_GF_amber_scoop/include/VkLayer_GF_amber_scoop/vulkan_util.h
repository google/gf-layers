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

#ifndef VKLAYER_GF_AMBER_SCOOP_VULKAN_UTIL_H
#define VKLAYER_GF_AMBER_SCOOP_VULKAN_UTIL_H

#include <vulkan/vulkan_core.h>

#include "gf_layers_layer_util/logging.h"

namespace gf_layers::amber_scoop_layer {

// Helper function for checking return value of any Vulkan command that returns
// |VkResult|. Asserts if the command fails.
inline void RequireSuccess(VkResult result) {
  RUNTIME_ASSERT(VK_SUCCESS == result);
}

// Helper function for checking return value of any Vulkan command that returns
// |VkResult| and also logs the given |error_message| if the command fails.
// Asserts if the command fails.
inline void RequireSuccess(VkResult result, const char* error_message) {
  if (VK_SUCCESS != result) {
    LOG("%s", error_message);
    RUNTIME_ASSERT(false);
  }
}

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_VULKAN_UTIL_H
