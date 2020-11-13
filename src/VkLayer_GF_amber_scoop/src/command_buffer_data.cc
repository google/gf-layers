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

#include "VkLayer_GF_amber_scoop/command_buffer_data.h"

// <algorithm> is used by GCC's libstdc++, but not LLVM's libc++.
#include <algorithm>  // IWYU pragma: keep
#include <memory>
#include <utility>

#include "VkLayer_GF_amber_scoop/vulkan_commands.h"

namespace gf_layers::amber_scoop_layer {

void CommandBufferData::AddCommand(std::unique_ptr<Cmd> cmd) {
  // Set the flag if the command being added is a draw call.
  if (cmd->IsDrawCall()) {
    contains_draw_calls_ = true;
  }
  command_list.push_back(std::move(cmd));
}

void CommandBufferData::ResetState() {
  command_list.clear();
  contains_draw_calls_ = false;
}

}  // namespace gf_layers::amber_scoop_layer
