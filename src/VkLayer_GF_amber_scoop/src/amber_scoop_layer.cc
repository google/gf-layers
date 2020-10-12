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

#include "VkLayer_GF_amber_scoop/amber_scoop_layer.h"

#include <vulkan/vk_layer.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "VkLayer_GF_amber_scoop/command_buffer_data.h"
#include "VkLayer_GF_amber_scoop/draw_call_tracker.h"
#include "VkLayer_GF_amber_scoop/vulkan_commands.h"
#include "gf_layers_layer_util/logging.h"
#include "gf_layers_layer_util/settings.h"
#include "gf_layers_layer_util/util.h"

namespace gf_layers::amber_scoop_layer {

namespace {

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
    GetSettingUint64("VkLayer_GF_amber_scoop_START_DRAW_CALL",
                     "debug.gf.as.start_draw_call", &settings.start_draw_call);
    GetSettingUint64("VkLayer_GF_amber_scoop_DRAW_CALL_COUNT",
                     "debug.gf.as.draw_call_count", &draw_call_count);
    GetSettingString("VkLayer_GF_amber_scoop_OUTPUT_FILE_PREFIX",
                     "debug.gf.as.output_file_prefix",
                     &settings.output_file_prefix);
    settings.last_draw_call = settings.start_draw_call + draw_call_count;
    settings.init = true;
  }
}

// Adds |command| to the list of tracked commands for |command_buffer|. Creates
// a list of commands |command_buffer| if one doesn't already exist. Tracked
// commands will be parsed later when |command_buffer| is submitted via the
// vkQueueSubmit function.
void AddCommand(DeviceData* device_data, VkCommandBuffer command_buffer,
                std::unique_ptr<Cmd> command) {
  auto* tracked_command_buffer =
      device_data->command_buffers_data.Get(command_buffer);
  // Check if the command buffer isn't tracked and start tracking if it isn't
  // tracked.
  if (tracked_command_buffer == nullptr) {
    device_data->command_buffers_data.Put(command_buffer, CommandBufferData());
    tracked_command_buffer =
        device_data->command_buffers_data.Get(command_buffer);
  }
  tracked_command_buffer->AddCommand(std::move(command));
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
      global_data->device_map.Get(DeviceKey(commandBuffer))->get();

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
      global_data->device_map.Get(DeviceKey(commandBuffer))->get();

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
      global_data->device_map.Get(DeviceKey(commandBuffer))->get();

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
      global_data->device_map.Get(DeviceKey(commandBuffer))->get();

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
// Our vkCreateGraphicsPipelines function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(
    VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data =
      global_data->device_map.Get(DeviceKey(device))->get();

  // Call the original function.
  VkResult result = device_data->vkCreateGraphicsPipelines(
      device, pipelineCache, createInfoCount, pCreateInfos, pAllocator,
      pPipelines);

  if (result != VK_SUCCESS) {
    return result;
  }

  // Deep copy all create info structs.
  for (uint32_t i = 0; i < createInfoCount; i++) {
    auto graphics_pipeline_data = std::make_unique<GraphicsPipelineData>(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        pCreateInfos[i]);

    // Add the shader modules to the |graphics_pipeline_data|.
    for (uint32_t stage_idx = 0;
         stage_idx < graphics_pipeline_data->GetCreateInfo().stageCount;
         stage_idx++) {
      VkShaderModule shader_module =
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
          graphics_pipeline_data->GetCreateInfo().pStages[stage_idx].module;

      std::shared_ptr<ShaderModuleData> shader_module_data =
          *device_data->shader_modules_data.Get(shader_module);

      // All shader modules should be tracked.
      DEBUG_ASSERT(shader_module_data != nullptr);

      graphics_pipeline_data->AddShaderModule(shader_module,
                                              shader_module_data);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    device_data->graphics_pipelines.Put(pPipelines[i],
                                        std::move(graphics_pipeline_data));
  }

  return result;
}

//
// Our vkCreateShaderModule function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(
    VkDevice device, VkShaderModuleCreateInfo const* pCreateInfo,
    VkAllocationCallbacks const* pAllocator, VkShaderModule* pShaderModule) {
  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data =
      global_data->device_map.Get(DeviceKey(device))->get();

  auto result = device_data->vkCreateShaderModule(device, pCreateInfo,
                                                  pAllocator, pShaderModule);
  if (result == VK_SUCCESS) {
    // Create a ShaderModuleData object to keep track of the shader module's
    // lifetime.
    device_data->shader_modules_data.Put(
        *pShaderModule, std::make_unique<ShaderModuleData>(*pCreateInfo));
  }
  return result;
}

//
// Our vkDestroyShaderModule function.
//
void vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule,
                           VkAllocationCallbacks const* pAllocator) {
  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data =
      global_data->device_map.Get(DeviceKey(device))->get();

  // Call the original function.
  device_data->vkDestroyShaderModule(device, shaderModule, pAllocator);

  // Remove the shader module from the map.
  MutexType local_mutex;
  ScopedLock scoped_lock(local_mutex);
  auto* shader_module_map =
      device_data->shader_modules_data.Access(&scoped_lock);
  shader_module_map->erase(shaderModule);
}

//
// Our vkQueueSubmit function.
//
VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(VkQueue queue,
                                             uint32_t submitCount,
                                             VkSubmitInfo const* pSubmits,
                                             VkFence fence) {
  // Go through all tracked commands in all of the submitted command buffers.
  // Every command buffer containing a draw call will be parsed to get the
  // state of every resource required by a draw call. If the draw call is
  // requested to be captured, an Amber file will be generated.

  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data =
      global_data->device_map.Get(DeviceKey(queue))->get();

  // Go through each queue submit.
  for (uint32_t submit_idx = 0; submit_idx < submitCount; submit_idx++) {
    // Go through each command buffer in each submit.
    for (uint32_t cmd_buffer_idx = 0;
         // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
         cmd_buffer_idx < pSubmits[submit_idx].commandBufferCount;
         cmd_buffer_idx++) {
      VkCommandBuffer command_buffer =
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
          pSubmits[submit_idx].pCommandBuffers[cmd_buffer_idx];

      CommandBufferData* command_buffer_data =
          device_data->command_buffers_data.Get(command_buffer);

      // Ignore command buffers that are not tracked, i.e. command buffers that
      // don't contain any interesting commands.
      if (command_buffer_data == nullptr) {
        continue;
      }
      // Mark the command buffer as submitted.
      command_buffer_data->SetSubmitted();

      // Skip all command buffers that don't contain any draw calls.
      if (!command_buffer_data->ContainsDrawCalls()) {
        continue;
      }

      // Create a new draw call state tracker. Tracker stores the state of the
      // command buffer being processed, i.e. what pipeline is bound, push
      // constant values, bound vertex and index buffers, etc. This information
      // is needed when a draw call needs to be processed to an Amber file.
      DrawCallTracker draw_call_tracker(global_data);
      draw_call_tracker.GetDrawCallState()->command_buffer = command_buffer;
      draw_call_tracker.GetDrawCallState()->queue = queue;

      // Process all submitted commands. Most of the commands update the state
      // of the draw call tracker and draw commands generate the Amber files.
      for (const std::unique_ptr<Cmd>& cmd :
           command_buffer_data->GetCommandList()) {
        cmd->ProcessSubmittedCommand(&draw_call_tracker);
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
        GetGlobalData()->instance_map.Get(InstanceKey(physicalDevice));

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
      GetLayerInstanceCreateInfo(pCreateInfo);

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

  GetGlobalData()->instance_map.Put(InstanceKey(*pInstance), instance_data);

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
      GetLayerDeviceCreateInfo(pCreateInfo);

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
      GetGlobalData()->instance_map.Get(InstanceKey(physicalDevice));

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

  HANDLE(vkCreateGraphicsPipelines)
  HANDLE(vkCreateShaderModule)
  HANDLE(vkDestroyShaderModule)
  HANDLE(vkGetDeviceProcAddr)
  HANDLE(vkQueueSubmit)
  HANDLE(vkCmdBeginRenderPass)
  HANDLE(vkCmdBindPipeline)
  HANDLE(vkCmdDraw)
  HANDLE(vkCmdDrawIndexed)

#undef HANDLE

  DEBUG_ASSERT(next_get_device_proc_address ==
               device_data->vkGetDeviceProcAddr);

  GetGlobalData()->device_map.Put(DeviceKey(*pDevice), std::move(device_data));

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
  HANDLE(vkCreateGraphicsPipelines)
  HANDLE(vkCreateShaderModule)
  HANDLE(vkDestroyShaderModule)
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
      ->device_map.Get(DeviceKey(device))
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
      ->instance_map.Get(InstanceKey(instance))
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
