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

#ifndef VKLAYER_GF_AMBER_SCOOP_AMBER_SCOOP_LAYER_H
#define VKLAYER_GF_AMBER_SCOOP_AMBER_SCOOP_LAYER_H

#include <vulkan/vulkan.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "VkLayer_GF_amber_scoop/vulkan_commands.h"
#include "gf_layers_layer_util/util.h"

namespace gf_layers::amber_scoop_layer {

// This class is used to keep track of a command buffer. This class stores
// commands recorded to a command buffer and keeps track of when the command
// buffer has been submitted. It also keeps track of whether the command buffer
// contains any draw calls. The class is not thread-safe, but it shouldn't
// matter because the Vulkan spec requires Vulkan applications to record
// commands without races.
class CommandBufferData {
 public:
  // Adds |cmd| to the command list.
  void AddCommand(std::unique_ptr<Cmd> cmd) {
    // Command buffer must be reset if it has been submitted and new commands
    // are being added to it, so we reset our command buffer tracker.
    if (is_submitted_) {
      command_list.clear();
      is_submitted_ = false;
      contains_draw_calls_ = false;
    }
    // Set the flag if the command being added is a draw call.
    if (cmd->GetKind() == Cmd::kDraw || cmd->GetKind() == Cmd::kDrawIndexed) {
      contains_draw_calls_ = true;
    }
    command_list.push_back(std::move(cmd));
  }

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

 private:
  // Flag to tell if the command buffer has been submitted.
  bool is_submitted_ = false;
  // Flag to tell if the command list contains any draw calls.
  bool contains_draw_calls_ = false;
  // List of tracked commands.
  std::vector<std::unique_ptr<Cmd>> command_list = {};
};

struct InstanceData {
  VkInstance instance;

  // Most layers must store this. Required to implement vkGetInstanceProcAddr.
  // Should not be used otherwise; all required instance function pointers
  // should be obtained during vkCreateInstance.
  PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;

  // Most layers must store this. Required to implement
  // vkEnumerateDeviceExtensionProperties.
  PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;

  // Other instance functions: (none)
};

struct DeviceData {
  VkDevice device = {};
  InstanceData* instance_data = {};

  // Most layers must store this. Required to implement vkGetDeviceProcAddr.
  // Should not be used otherwise; all required device function pointers
  // should be obtained during vkCreateDevice.
  PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = {};

  // Other device functions:

  PFN_vkQueueSubmit vkQueueSubmit = {};
  PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = {};
  PFN_vkCmdDraw vkCmdDraw = {};
  PFN_vkCmdDrawIndexed vkCmdDrawIndexed = {};
  PFN_vkCmdBindPipeline vkCmdBindPipeline = {};

  // Tracked device data:

  ProtectedMap<VkCommandBuffer, CommandBufferData> command_buffers_data;
};

using InstanceMap = gf_layers::ProtectedTinyStaleMap<void*, InstanceData>;
using DeviceMap =
    gf_layers::ProtectedTinyStaleMap<void*, std::unique_ptr<DeviceData>>;

// Read-only once initialized.
struct AmberScoopLayerSettings {
  bool init = false;
  // Number of the first draw call to be captured. Can be set via env variable
  // "VkLayer_GF_amber_scoop_START_DRAW_CALL" or Android property
  // "debug.gf.as.start_draw_call"
  uint64_t start_draw_call = 0;
  // Number of the last draw call to be captured. Last draw call is
  // "start_draw_call" + "VkLayer_GF_amber_scoop_DRAW_CALL_COUNT".
  uint64_t last_draw_call = 0;
  // Prefix used in output file names. Can be set via env variable
  // "VkLayer_GF_amber_scoop_OUTPUT_FILE_PREFIX"
  std::string output_file_prefix = "amber_scoop_output";
};

struct GlobalData {
  InstanceMap instance_map;
  DeviceMap device_map;
  // Draw call counter used to detect the draw call to be captured.
  std::atomic<uint64_t> current_draw_call = {};

  // In vkCreateInstance, we initialize |settings| by reading environment
  // variables while holding |settings_mutex|, after which |settings| is
  // read-only. Thus, in instance or device functions (such as
  // vkQueuePresentKHR) we can read |settings| without holding |settings_mutex|.
  gf_layers::MutexType settings_mutex;
  AmberScoopLayerSettings settings;
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_AMBER_SCOOP_LAYER_H
