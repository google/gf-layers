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

#include "gf_layers_layer_util/util.h"

#include <vulkan/vk_layer.h>

namespace gf_layers {

void* instance_key(VkInstance handle) {
  return *reinterpret_cast<void**>(handle);
}

void* instance_key(VkPhysicalDevice handle) {
  return *reinterpret_cast<void**>(handle);
}

void* device_key(VkDevice handle) { return *reinterpret_cast<void**>(handle); }

VkLayerInstanceCreateInfo* get_layer_instance_create_info(
    const VkInstanceCreateInfo* pCreateInfo) {
  // pCreateInfo->pNext is a linked list of unknown struct types, discriminated
  // by the first member |sType|.
  // The struct type we want then has a further discriminated union field |u|
  // with discriminator |function|. We check both.

  auto* next = const_cast<VkLayerInstanceCreateInfo*>(
      static_cast<const VkLayerInstanceCreateInfo*>(pCreateInfo->pNext));

  while (next != nullptr) {
    if (next->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO &&
        next->function == VK_LAYER_LINK_INFO) {
      return next;
    }
    next = const_cast<VkLayerInstanceCreateInfo*>(
        static_cast<const VkLayerInstanceCreateInfo*>(next->pNext));
  }
  return next;
}

VkLayerDeviceCreateInfo* get_layer_device_create_info(
    const VkDeviceCreateInfo* pCreateInfo) {
  // pCreateInfo->pNext is a linked list of unknown struct types, discriminated
  // by the first member |sType|.
  // The struct type we want then has a further discriminated union field |u|
  // with discriminator |function|. We check both.

  auto* next = const_cast<VkLayerDeviceCreateInfo*>(
      static_cast<const VkLayerDeviceCreateInfo*>(pCreateInfo->pNext));

  while (next != nullptr) {
    if (next->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO &&
        next->function == VK_LAYER_LINK_INFO) {
      return next;
    }
    next = const_cast<VkLayerDeviceCreateInfo*>(
        static_cast<const VkLayerDeviceCreateInfo*>(next->pNext));
  }
  return next;
}

}  // namespace gf_layers
