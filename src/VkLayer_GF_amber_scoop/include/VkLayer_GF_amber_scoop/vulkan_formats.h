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

#ifndef VKLAYER_GF_AMBER_SCOOP_VULKAN_FORMATS_H
#define VKLAYER_GF_AMBER_SCOOP_VULKAN_FORMATS_H

#include <vulkan/vulkan.h>

namespace gf_layers::amber_scoop_layer {

// Returns a name of the given |vk_format| as string. The names are formatted as
// they are used in Amber i.e. don't contain the "VK_FORMAT_" prefix. Returns
// only format names that are supported by Amber. Asserts if an unsupported
// |vk_format| is given.
const char* VkFormatToAmberFormatName(VkFormat vk_format);

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_VULKAN_FORMATS_H
