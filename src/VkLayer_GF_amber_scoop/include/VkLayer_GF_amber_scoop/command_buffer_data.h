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

#ifndef VKLAYER_GF_AMBER_SCOOP_COMMAND_BUFFER_DATA_H
#define VKLAYER_GF_AMBER_SCOOP_COMMAND_BUFFER_DATA_H

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "VkLayer_GF_amber_scoop/vulkan_commands.h"

namespace gf_layers::amber_scoop_layer {

// This class is used to keep track of a command buffer. This class stores
// commands recorded to a command buffer and keeps track of when the command
// buffer has been submitted. It also keeps track of whether the command buffer
// contains any draw calls. The class is not thread-safe, but it shouldn't
// matter because the Vulkan spec requires Vulkan applications to record
// commands without races.
class CommandBufferData {
 public:
  explicit CommandBufferData(VkCommandBufferAllocateInfo allocate_info)
      : allocate_info_(allocate_info) {}

  // Adds |cmd| to the command list.
  void AddCommand(std::unique_ptr<Cmd> cmd);

  // Returns true if the command list contains a draw call.
  [[nodiscard]] bool ContainsDrawCalls() const { return contains_draw_calls_; }

  // Returns the list of captured commands. The list can't be modified directly.
  // Use AddCommand() to append to the list.
  [[nodiscard]] const std::vector<std::unique_ptr<Cmd>>& GetCommandList()
      const {
    return command_list;
  }

  // Sets the command buffer as submitted.
  void SetSubmitted() { is_submitted_ = true; }

  const VkCommandBufferAllocateInfo* GetAllocateInfo() {
    return &allocate_info_;
  }

 private:
  // VkCommandBufferAllocateInfo used to allocate this command buffer.
  VkCommandBufferAllocateInfo allocate_info_;
  // Flag to tell if the command buffer has been submitted.
  bool is_submitted_ = false;
  // Flag to tell if the command list contains any draw calls.
  bool contains_draw_calls_ = false;
  // List of tracked commands.
  std::vector<std::unique_ptr<Cmd>> command_list = {};
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_COMMAND_BUFFER_DATA_H
