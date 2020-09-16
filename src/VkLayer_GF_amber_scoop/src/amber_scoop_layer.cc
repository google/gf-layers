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

#include <vulkan/vk_layer.h>
#include <vulkan/vulkan.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "VkLayer_GF_amber_scoop/vulkan_commands.h"
#include "gf_layers_layer_util/logging.h"
#include "gf_layers_layer_util/settings.h"
#include "gf_layers_layer_util/util.h"

namespace gf_layers::amber_scoop_layer {

// This class is used to keep track of a command buffer. This class stores
// commands recorded to a command buffer and keeps track when the command buffer
// has been submitted. Also keeps track if the command buffer being tracked
// contains any draw calls. The class is not thread-safe, but it shouldn't
// matter because Vulkan spec requires Vulkan applications to record commands
// without races.
class CommandBufferTracker {
 public:
  // Add command to the command list.
  void AddCommand(std::unique_ptr<Cmd> cmd) {
    // Command buffer must be reset if it has been submitted and new commands
    // are being added to it, so we reset our command buffer tracker.
    if (is_submitted_) {
      command_list.clear();
      is_submitted_ = false;
      contains_draw_calls_ = false;
    }
    // Set the flag if the command being added is a draw call.
    if (cmd->AsDraw() != nullptr || cmd->AsDrawIndexed() != nullptr) {
      contains_draw_calls_ = true;
    }
    command_list.push_back(std::move(cmd));
  }

  // Return true if the command list contains a draw call.
  [[nodiscard]] bool ContainsDrawCalls() const { return contains_draw_calls_; }

  // Get reference to the list of captured commands. The list can't be modified
  // directly. Use AddCommand() to add append the list.
  [[nodiscard]] const std::vector<std::unique_ptr<Cmd>>* GetCommandList()
      const {
    return &command_list;
  }

  // Mark the command buffer to be submitted. The flag is reset automatically if
  // a new command is added after the command buffer has been submitted.
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

  ProtectedMap<VkCommandBuffer, CommandBufferTracker> command_buffers;
};

using InstanceMap = gf_layers::ProtectedTinyStaleMap<void*, InstanceData>;
using DeviceMap =
    gf_layers::ProtectedTinyStaleMap<void*, std::unique_ptr<DeviceData>>;

namespace {

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

// Draw call state tracker is used to store the state of a draw call while
// parsing the command buffers in vkQueueSubmit(...) -function. One draw call
// state tracker is created for each command buffer containing draw calls.
struct DrawCallStateTracker {
  VkCommandBuffer command_buffer_handle;
  VkQueue queue;
  VkRenderPassBeginInfo* current_render_pass = nullptr;
  uint32_t current_subpass = 0;
  VkPipeline graphics_pipeline;
  std::vector<uint8_t> push_constant_data;
  // Map of vertex buffer bindings. Key is the binding number and value is the
  // buffer handle.
  std::unordered_map<uint32_t, VkBuffer> bound_vertex_buffers;
  // Map of vertex buffer offsets. Key is the binding number of the buffer and
  // value is the vertex buffer offset.
  std::unordered_map<uint32_t, VkDeviceSize> vertex_buffer_offsets;
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"

// TODO(paulthomson): The GlobalData is almost "constinit", but we would need to
//  adjust our map data structures.

// Global variables (that are not constinit) are strongly discouraged.
// However, we should be fine as long as all global data is stored in this
// struct.
GlobalData global_data_;  // NOLINT(cert-err58-cpp)
#pragma clang diagnostic pop

GlobalData* GetGlobalData() { return &global_data_; }

const std::array<VkLayerProperties, 1> kLayerProperties{{{
    "VkLayer_GF_amber_scoop",       // layerName
    VK_MAKE_VERSION(1U, 1U, 130U),  // specVersion NOLINT(hicpp-signed-bitwise)
    1,                              // implementationVersion
    "Amber scoop layer.",           // description
}}};

bool IsThisLayer(const char* pLayerName) {
  return ((pLayerName != nullptr) &&
          strcmp(pLayerName, kLayerProperties[0].layerName) == 0);
}

void InitSettingsIfNeeded() {
  gf_layers::ScopedLock lock(GetGlobalData()->settings_mutex);

  AmberScoopLayerSettings& settings = GetGlobalData()->settings;

  if (!settings.init) {
    uint64_t draw_call_count = 1;
    get_setting_uint64("VkLayer_GF_amber_scoop_START_DRAW_CALL",
                       "debug.gf.as.start_draw_call",
                       &settings.start_draw_call);
    get_setting_uint64("VkLayer_GF_amber_scoop_DRAW_CALL_COUNT",
                       "debug.gf.as.draw_call_count", &draw_call_count);
    get_setting_string("VkLayer_GF_amber_scoop_OUTPUT_FILE_PREFIX",
                       "debug.gf.as.output_file_prefix",
                       &settings.output_file_prefix);
    settings.last_draw_call = settings.start_draw_call + draw_call_count;
    settings.init = true;
  }
}

// Adds the given Vulkan command buffer command to the list of tracked commands.
// Creates a new list of commands for the given command buffer if it doesn't
// already exist. Tracked commands will be parsed later when the command buffer
// is submitted via vkQueueSubmit function.
void AddCommand(DeviceData* device_data, VkCommandBuffer command_buffer,
                std::unique_ptr<Cmd> command) {
  auto* tracked_command_buffer =
      device_data->command_buffers.get(command_buffer);
  // Check if the command buffer isn't tracked and start tracking if it isn't
  // tracked.
  if (tracked_command_buffer == nullptr) {
    device_data->command_buffers.put(command_buffer, CommandBufferTracker());
    tracked_command_buffer = device_data->command_buffers.get(command_buffer);
  }
  tracked_command_buffer->AddCommand(std::move(command));
}

// This function handles all of the draw calls. An Amber file will be generated
// from the draw call if the draw call is set to be captured via settings.
//
// TODO(ilkkasaa): This function is going to be huge. Consider making a separate
//  file for handling draw calls / creating amber files.
void HandleDrawCall(const DrawCallStateTracker& draw_call_state_tracker,
                    const DeviceData& device_data, uint32_t first_index,
                    uint32_t index_count, uint32_t first_vertex,
                    uint32_t vertex_count, uint32_t first_instance,
                    uint32_t instance_count) {
  // Silence unused parameter warnings.
  // TODO(ilkkasaa) remove these when they are used.
  (void)device_data;
  (void)first_vertex;
  (void)vertex_count;

  // Graphics pipeline must be bound.
  DEBUG_ASSERT(draw_call_state_tracker.graphics_pipeline);
  DEBUG_ASSERT(draw_call_state_tracker.current_render_pass);

  auto* global_data = GetGlobalData();

  const uint64_t current_draw_call = global_data->current_draw_call;
  // Return if current draw call should not be captured.
  if (current_draw_call < global_data->settings.start_draw_call ||
      current_draw_call > global_data->settings.last_draw_call) {
    global_data->current_draw_call++;
    return;
  }
  global_data->current_draw_call++;

  // Initialize string streams for different parts of the amber file.
  std::ostringstream buffer_declaration_str;
  // TODO(ilkkasaa): add other string streams

  buffer_declaration_str << "BUFFER ...";

  std::string amber_file_name = global_data->settings.output_file_prefix + "_" +
                                std::to_string(current_draw_call) + ".amber";

  std::ofstream amber_file;
  amber_file.open(amber_file_name, std::ios::trunc | std::ios::out);

  amber_file << "#!amber" << std::endl << std::endl;

  amber_file << "SHADER vertex vertex_shader SPIRV-ASM" << std::endl;
  // TODO(ilkkasaa): add vertex shader spirv here
  amber_file << "TODO" << std::endl;
  amber_file << "END" << std::endl << std::endl;

  amber_file << "SHADER fragment fragment_shader SPIRV-ASM" << std::endl;
  // TODO(ilkkasaa): add fragment shader spirv here
  amber_file << "TODO" << std::endl;
  amber_file << "END" << std::endl << std::endl;

  // Pipeline
  amber_file << "PIPELINE graphics pipeline" << std::endl;
  amber_file << "  ATTACH vertex_shader";
  amber_file << std::endl;
  amber_file << "  ATTACH fragment_shader";
  // TODO(ilkkasaa): add other pipeline contents here
  amber_file << "END" << std::endl << std::endl;  // PIPELINE

  // TODO(ilkkasaa): get primitive topology from VkGraphicsPipelineCreateInfo
  const std::string topology = "TODO";

  if (index_count > 0) {
    amber_file << "RUN pipeline DRAW_ARRAY AS " << topology
               << " INDEXED START_IDX " << first_index << " COUNT "
               << index_count;
  } else {
    amber_file << "RUN pipeline DRAW_ARRAY AS " << topology;
  }
  if (instance_count > 0) {
    amber_file << " START_INSTANCE " << first_instance << " INSTANCE_COUNT "
               << instance_count;
  }
  amber_file << std::endl;
}

// Intercepted "vkCmd..." functions.

//
// Our vkCmdBeginRenderPass function.
//
VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) {
  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data =
      global_data->device_map.get(device_key(commandBuffer))->get();

  // Call the original function.
  device_data->vkCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);

  AddCommand(device_data, commandBuffer,
             std::make_unique<CmdBeginRenderPass>(pRenderPassBegin, contents));
}

//
// Our vkCmdBindPipeline function.
//
VKAPI_ATTR void VKAPI_CALL
vkCmdBindPipeline(VkCommandBuffer commandBuffer,
                  VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {
  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data =
      global_data->device_map.get(device_key(commandBuffer))->get();

  // Call the original function.
  device_data->vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);

  AddCommand(device_data, commandBuffer,
             std::make_unique<CmdBindPipeline>(pipelineBindPoint, pipeline));
}

//
// Our vkCmdDraw function.
//
VKAPI_ATTR void VKAPI_CALL vkCmdDraw(VkCommandBuffer commandBuffer,
                                     uint32_t vertexCount,
                                     uint32_t instanceCount,
                                     uint32_t firstVertex,
                                     uint32_t firstInstance) {
  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data =
      global_data->device_map.get(device_key(commandBuffer))->get();

  // Call the original function.
  device_data->vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex,
                         firstInstance);

  AddCommand(device_data, commandBuffer,
             std::make_unique<CmdDraw>(vertexCount, instanceCount, firstVertex,
                                       firstInstance));
}

//
// Our vkCmdDrawIndexed function.
//
VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(
    VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount,
    uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data =
      global_data->device_map.get(device_key(commandBuffer))->get();

  // Call the original function.
  device_data->vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount,
                                firstIndex, vertexOffset, firstInstance);

  AddCommand(
      device_data, commandBuffer,
      std::make_unique<CmdDrawIndexed>(indexCount, instanceCount, firstIndex,
                                       vertexOffset, firstInstance));
}

// Other intercepted vulkan functions.

//
// Our vkQueueSubmit function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(VkQueue queue,
                                             uint32_t submitCount,
                                             VkSubmitInfo const* pSubmits,
                                             VkFence fence) {
  // Go through all tracked commands in all of the submitted command buffers.
  // Every command buffer containing a draw call will be parsed command by
  // command to get the states of every resource required by a draw call.
  // If the draw call is requested to be captured, an Amber file will be
  // generated from it.

  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data =
      global_data->device_map.get(device_key(queue))->get();

  // Go through each queue submit.
  for (uint32_t submit_idx = 0; submit_idx < submitCount; submit_idx++) {
    // Go through all command buffers in each submit.
    for (uint32_t cmd_buffer_idx = 0;
         // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
         cmd_buffer_idx < pSubmits[submit_idx].commandBufferCount;
         cmd_buffer_idx++) {
      auto* command_buffer_handle =
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
          pSubmits[submit_idx].pCommandBuffers[cmd_buffer_idx];

      auto* command_buffer =
          device_data->command_buffers.get(command_buffer_handle);

      // Ignore command buffers that are not tracked, i.e. command buffers that
      // don't contain any interesting commands.
      if (command_buffer == nullptr) {
        continue;
      }
      // Mark the command buffer as submitted.
      command_buffer->SetSubmitted();

      // Skip all command buffers that don't contain any draw calls.
      if (!command_buffer->ContainsDrawCalls()) {
        continue;
      }

      // Create a new draw call state tracker. Tracker stores the state of the
      // command buffer being processed, i.e. what pipeline is bound, push
      // constant values, bound vertex and index buffers, etc. This information
      // is needed when a draw call needs to be processed to an Amber file.
      DrawCallStateTracker draw_call_state = {};
      draw_call_state.command_buffer_handle = command_buffer_handle;
      draw_call_state.queue = queue;

      for (const auto& cmd : *command_buffer->GetCommandList()) {
        if (auto* cmd_begin_renderpass = cmd->AsBeginRenderPass()) {
          draw_call_state.current_render_pass =
              &cmd_begin_renderpass->render_pass_begin;
          draw_call_state.current_subpass = 0;
        } else if (auto* cmd_draw = cmd->AsDraw()) {
          HandleDrawCall(draw_call_state, *device_data, 0, 0,
                         cmd_draw->first_vertex, cmd_draw->vertex_count,
                         cmd_draw->first_instance, cmd_draw->instance_count);
        } else if (auto* cmd_draw_indexed = cmd->AsDrawIndexed()) {
          HandleDrawCall(
              draw_call_state, *device_data, cmd_draw_indexed->first_index,
              cmd_draw_indexed->index_count, 0, 0,
              cmd_draw_indexed->first_index, cmd_draw_indexed->instance_count);
        } else if (auto* cmd_bind_pipeline = cmd->AsBindPipeline()) {
          // Currently we are interested in graphics pipelines only.
          if (cmd_bind_pipeline->pipeline_bind_point ==
              VK_PIPELINE_BIND_POINT_GRAPHICS) {
            draw_call_state.graphics_pipeline = cmd_bind_pipeline->pipeline;
          }
        }
      }
    }
  }

  // Call the original function.
  VkResult result =
      device_data->vkQueueSubmit(queue, submitCount, pSubmits, fence);

  return result;
}

// The following functions are standard Vulkan functions that most Vulkan layers
// must implement.

//
// Our vkEnumerateInstanceLayerProperties function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(
    uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
  DEBUG_LOG("vkEnumerateInstanceLayerProperties");

  if (pProperties == nullptr) {
    *pPropertyCount = 1;
    return VK_SUCCESS;
  }

  if (*pPropertyCount == 0) {
    return VK_INCOMPLETE;
  }

  *pPropertyCount = 1;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  pProperties[0] = kLayerProperties[0];

  return VK_SUCCESS;
}

//
// Our vkEnumerateDeviceLayerProperties function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice /*physicalDevice*/, uint32_t* pPropertyCount,
    VkLayerProperties* pProperties) {
  DEBUG_LOG(
      "vkEnumerateDeviceLayerProperties (calling "
      "vkEnumerateInstanceLayerProperties)");

  return vkEnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}

//
// Our vkEnumerateInstanceExtensionProperties function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
    const char* pLayerName, uint32_t* pPropertyCount,
    VkExtensionProperties* /*pProperties*/) {
  DEBUG_LOG("vkEnumerateInstanceExtensionProperties");

  if (!IsThisLayer(pLayerName)) {
    return VK_ERROR_LAYER_NOT_PRESENT;
  }
  *pPropertyCount = 0;
  return VK_SUCCESS;
}

//
// Our vkEnumerateDeviceExtensionProperties function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char* pLayerName,
    uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
  DEBUG_LOG("vkEnumerateDeviceExtensionProperties");

  if (!IsThisLayer(pLayerName)) {
    DEBUG_ASSERT(physicalDevice);

    InstanceData* instance_data =
        GetGlobalData()->instance_map.get(instance_key(physicalDevice));

    return instance_data->vkEnumerateDeviceExtensionProperties(
        physicalDevice, pLayerName, pPropertyCount, pProperties);
  }

  *pPropertyCount = 0;
  return VK_SUCCESS;
}

//
// Our vkCreateInstance function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(
    const VkInstanceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
  DEBUG_LOG("vkCreateInstance");

  InitSettingsIfNeeded();

  // Get the layer instance create info, which we need so we can:
  // (a) obtain the next GetInstanceProcAddr function and;
  // (b) advance the "layer info" linked list so that the next layer will be
  //     able to get its layer info.

  VkLayerInstanceCreateInfo* layer_instance_create_info =
      get_layer_instance_create_info(pCreateInfo);

  DEBUG_ASSERT(layer_instance_create_info);
  DEBUG_ASSERT(layer_instance_create_info->u.pLayerInfo);

  // Obtain the next GetInstanceProcAddr function.
  PFN_vkGetInstanceProcAddr next_get_instance_proc_address =
      layer_instance_create_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;

  // Use it to get vkCreateInstance.
  auto vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(
      next_get_instance_proc_address(nullptr, "vkCreateInstance"));

  if (vkCreateInstance == nullptr) {
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  // Advance the layer info before calling the next vkCreateInstance.
  layer_instance_create_info->u.pLayerInfo =
      layer_instance_create_info->u.pLayerInfo->pNext;

  VkResult result = vkCreateInstance(pCreateInfo, pAllocator, pInstance);

  if (result != VK_SUCCESS) {
    return result;
  }

  // Initialize our InstanceData, including all instance function pointers that
  // we will need.

  InstanceData instance_data{};

  instance_data.instance = *pInstance;

#define HANDLE(func)                                      \
  instance_data.func = reinterpret_cast<PFN_##func>(      \
      next_get_instance_proc_address(*pInstance, #func)); \
  if (!instance_data.func) {                              \
    return VK_ERROR_INITIALIZATION_FAILED;                \
  }

  HANDLE(vkGetInstanceProcAddr)
  HANDLE(vkEnumerateDeviceExtensionProperties)
#undef HANDLE

  DEBUG_ASSERT(next_get_instance_proc_address ==
               instance_data.vkGetInstanceProcAddr);

  GetGlobalData()->instance_map.put(instance_key(*pInstance), instance_data);

  return result;
}

//
// Our vkCreateDevice function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(
    VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
  DEBUG_LOG("vkCreateDevice");

  // Get the layer device create info, which we need so we can:
  // (a) obtain the next Get{Instance,Device}ProcAddr functions and;
  // (b) advance the "layer info" linked list so that the next layer will be
  //     able to get its layer info.

  VkLayerDeviceCreateInfo* layer_device_create_info =
      get_layer_device_create_info(pCreateInfo);

  DEBUG_ASSERT(layer_device_create_info);
  DEBUG_ASSERT(layer_device_create_info->u.pLayerInfo);

  // Obtain the next *ProcAddr functions.
  PFN_vkGetInstanceProcAddr next_get_instance_proc_address =
      layer_device_create_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
  PFN_vkGetDeviceProcAddr next_get_device_proc_address =
      layer_device_create_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;

  // Warning: most guides suggest calling vkGetInstanceProcAddr here with a NULL
  // instance but this appears to be invalid and so the next layer could fail.
  // Thus, we get the instance_data associated with the physicalDevice so that
  // we can pass the correct VkInstance.

  InstanceData* instance_data =
      GetGlobalData()->instance_map.get(instance_key(physicalDevice));

  // Use next_get_instance_proc_address to get vkCreateDevice.
  auto vkCreateDevice =
      reinterpret_cast<PFN_vkCreateDevice>(next_get_instance_proc_address(
          instance_data->instance, "vkCreateDevice"));

  if (vkCreateDevice == nullptr) {
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  // Advance the layer info before calling the next vkCreateDevice.
  layer_device_create_info->u.pLayerInfo =
      layer_device_create_info->u.pLayerInfo->pNext;

  // Call the next layer.
  VkResult result =
      vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);

  if (result != VK_SUCCESS) {
    return result;
  }

  // Initialize our DeviceData, including all device function pointers that
  // we will need.

  std::unique_ptr<DeviceData> device_data = std::make_unique<DeviceData>();

  device_data->device = *pDevice;
  device_data->instance_data = instance_data;

#define HANDLE(func)                                  \
  device_data->func = reinterpret_cast<PFN_##func>(   \
      next_get_device_proc_address(*pDevice, #func)); \
  if (!device_data->func) {                           \
    return VK_ERROR_INITIALIZATION_FAILED;            \
  }

  HANDLE(vkGetDeviceProcAddr)
  HANDLE(vkQueueSubmit)
  HANDLE(vkCmdBeginRenderPass)
  HANDLE(vkCmdBindPipeline)
  HANDLE(vkCmdDraw)
  HANDLE(vkCmdDrawIndexed)

#undef HANDLE

  DEBUG_ASSERT(next_get_device_proc_address ==
               device_data->vkGetDeviceProcAddr);

  GetGlobalData()->device_map.put(device_key(*pDevice), std::move(device_data));

  return result;
}

//
// Our vkGetDeviceProcAddr function.
//
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetDeviceProcAddr(VkDevice device, const char* pName) {
  DEBUG_ASSERT(pName);
  DEBUG_LOG("vkGetDeviceProcAddr: %s", pName);

#define HANDLE(func)                                   \
  if (strcmp(pName, #func) == 0) {                     \
    return reinterpret_cast<PFN_vkVoidFunction>(func); \
  }

  // The device functions provided by this layer:

  // Standard device functions that most layers must implement:
  HANDLE(vkGetDeviceProcAddr)  // Self-reference.

  // Other device functions that this layer intercepts:
  HANDLE(vkQueueSubmit)
  HANDLE(vkCmdBeginRenderPass)
  HANDLE(vkCmdBindPipeline)
  HANDLE(vkCmdDraw)
  HANDLE(vkCmdDrawIndexed)
#undef HANDLE

  if (device == nullptr) {
    return nullptr;
  }

  // At this point, the function must be a device function that we are not
  // intercepting. We must have already intercepted the creation of the
  // device and so we have the appropriate function pointer for the next
  // vkGetDeviceProcAddr. We call the next layer in the chain.
  return GetGlobalData()
      ->device_map.get(device_key(device))
      ->get()
      ->vkGetDeviceProcAddr(device, pName);
}

//
// Our vkGetInstanceProcAddr function.
//
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
  DEBUG_ASSERT(pName);

  DEBUG_LOG("vkGetInstanceProcAddr: %s", pName);

#define HANDLE(func)                                   \
  if (strcmp(pName, #func) == 0) {                     \
    return reinterpret_cast<PFN_vkVoidFunction>(func); \
  }

  // The global and instance functions provided by this layer:

  // The standard introspection functions.
  HANDLE(vkEnumerateInstanceLayerProperties)
  HANDLE(vkEnumerateDeviceLayerProperties)
  HANDLE(vkEnumerateInstanceExtensionProperties)
  HANDLE(vkEnumerateDeviceExtensionProperties)

  // Standard functions that most layers must implement:
  HANDLE(vkCreateInstance)
  HANDLE(vkCreateDevice)
  HANDLE(vkGetInstanceProcAddr)  // Self-reference.
  HANDLE(vkGetDeviceProcAddr)

  // Other instance functions that this layer intercepts: (none)

#undef HANDLE

  // We could move this up, as instance must be non-null in all but a few cases.
  // But it is not the job of this layer to detect invalid calls, so we can just
  // leave this check until the last possible moment.
  if (instance == nullptr) {
    return nullptr;
  }

  // At this point, the function must really be an instance function (as opposed
  // to a global function) that we are not intercepting. We must have already
  // intercepted the creation of the instance and so we have the function
  // pointer for the next vkGetInstanceProcAddr. We call the next layer in the
  // chain.
  return GetGlobalData()
      ->instance_map.get(instance_key(instance))
      ->vkGetInstanceProcAddr(instance, pName);
}

}  // namespace
}  // namespace gf_layers::amber_scoop_layer

//
// Exported functions.
//

extern "C" {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma ide diagnostic ignored "UnusedGlobalDeclarationInspection"

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
VkLayer_GF_amber_scoopNegotiateLoaderLayerInterfaceVersion(
    VkNegotiateLayerInterface* pVersionStruct) {
  DEBUG_LOG(
      "Entry point: "
      "VkLayer_GF_amber_scoopNegotiateLoaderLayerInterfaceVersion");

  DEBUG_ASSERT(pVersionStruct);
  DEBUG_ASSERT(pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT);

  pVersionStruct->pfnGetInstanceProcAddr =
      gf_layers::amber_scoop_layer::vkGetInstanceProcAddr;
  pVersionStruct->pfnGetDeviceProcAddr =
      gf_layers::amber_scoop_layer::vkGetDeviceProcAddr;
  pVersionStruct->pfnGetPhysicalDeviceProcAddr = nullptr;

  if (pVersionStruct->loaderLayerInterfaceVersion >
      CURRENT_LOADER_LAYER_INTERFACE_VERSION) {
    pVersionStruct->loaderLayerInterfaceVersion =
        CURRENT_LOADER_LAYER_INTERFACE_VERSION;
  }

  return VK_SUCCESS;
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
VkLayer_GF_amber_scoopGetInstanceProcAddr(VkInstance instance,
                                          const char* pName) {
  DEBUG_LOG("Entry point: VkLayer_GF_amber_scoopGetInstanceProcAddr");

  return gf_layers::amber_scoop_layer::vkGetInstanceProcAddr(instance, pName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
VkLayer_GF_amber_scoopGetDeviceProcAddr(VkDevice device, const char* pName) {
  DEBUG_LOG("Entry point: VkLayer_GF_amber_scoopGetDeviceProcAddr");

  return gf_layers::amber_scoop_layer::vkGetDeviceProcAddr(device, pName);
}

#if defined(__ANDROID__)

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount,
                                   VkLayerProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateInstanceLayerProperties");

  return gf_layers::amber_scoop_layer::vkEnumerateInstanceLayerProperties(
      pPropertyCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount,
    VkLayerProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateDeviceLayerProperties");

  return gf_layers::amber_scoop_layer::vkEnumerateDeviceLayerProperties(
      physicalDevice, pPropertyCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char* pLayerName,
                                       uint32_t* pPropertyCount,
                                       VkExtensionProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateInstanceExtensionProperties");

  return gf_layers::amber_scoop_layer::vkEnumerateInstanceExtensionProperties(
      pLayerName, pPropertyCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                     const char* pLayerName,
                                     uint32_t* pPropertyCount,
                                     VkExtensionProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateDeviceExtensionProperties");

  return gf_layers::amber_scoop_layer::vkEnumerateDeviceExtensionProperties(
      physicalDevice, pLayerName, pPropertyCount, pProperties);
}

#endif

#pragma clang diagnostic pop

}  // extern "C"
