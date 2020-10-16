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

#include "VkLayer_GF_amber_scoop/command_buffer_data.h"
#include "VkLayer_GF_amber_scoop/create_info_wrapper.h"
#include "VkLayer_GF_amber_scoop/graphics_pipeline_data.h"
#include "gf_layers_layer_util/util.h"

namespace gf_layers::amber_scoop_layer {

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

  PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = {};
  PFN_vkCreateShaderModule vkCreateShaderModule = {};
  PFN_vkDestroyShaderModule vkDestroyShaderModule = {};
  PFN_vkQueueSubmit vkQueueSubmit = {};
  PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = {};
  PFN_vkCmdDraw vkCmdDraw = {};
  PFN_vkCmdDrawIndexed vkCmdDrawIndexed = {};
  PFN_vkCmdBindPipeline vkCmdBindPipeline = {};

  // Tracked device data:

  ProtectedMap<VkCommandBuffer, CommandBufferData> command_buffers_data;
  ProtectedMap<VkPipeline, std::unique_ptr<GraphicsPipelineData>>
      graphics_pipelines;

  // Map of shader modules. Shared pointers are used because
  // |GraphicsPipelineData| may also hold a reference. |ShaderModuleData| will
  // be deleted when the last pipeline using it is destroyed and the shader
  // module itself is destroyed via vkDestroyShaderModule.
  ProtectedMap<VkShaderModule, std::shared_ptr<ShaderModuleData>>
      shader_modules_data;
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
