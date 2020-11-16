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
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "gf_layers_layer_util/logging.h"
#include "gf_layers_layer_util/settings.h"
#include "gf_layers_layer_util/util.h"

namespace gf_layers::frame_counter_layer {

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
  VkDevice device;
  InstanceData* instance_data;

  // Most layers must store this. Required to implement vkGetDeviceProcAddr.
  // Should not be used otherwise; all required device function pointers
  // should be obtained during vkCreateDevice.
  PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;

  // Other device functions:

  PFN_vkQueuePresentKHR vkQueuePresentKHR;
};

using InstanceMap = gf_layers::ProtectedTinyStaleMap<void*, InstanceData>;
using DeviceMap = gf_layers::ProtectedTinyStaleMap<void*, DeviceData>;

namespace {

// Read-only once initialized.
struct FrameCounterLayerSettings {
  bool init = false;
  uint64_t start_frame = 0;
  uint64_t end_frame = 0;
  std::string output_file;
};

struct GlobalData {
  InstanceMap instance_map;
  DeviceMap device_map;
  std::atomic<uint64_t> frame_counter{};

  gf_layers::MutexType start_time_mutex;
  std::chrono::steady_clock::time_point start_time;

  // In vkCreateInstance, we initialize |settings| by reading environment
  // variables while holding |settings_mutex|, after which |settings| is
  // read-only. Thus, in instance or device functions (such as
  // vkQueuePresentKHR) we can read |settings| without holding |settings_mutex|.
  gf_layers::MutexType settings_mutex;
  FrameCounterLayerSettings settings;
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
    "VkLayer_GF_frame_counter",     // layerName
    VK_MAKE_VERSION(1U, 1U, 130U),  // specVersion NOLINT(hicpp-signed-bitwise)
    1,                              // implementationVersion
    "Frame counter layer.",         // description
}}};

bool IsThisLayer(const char* pLayerName) {
  return ((pLayerName != nullptr) &&
          strcmp(pLayerName, kLayerProperties[0].layerName) == 0);
}

void InitSettingsIfNeeded() {
  gf_layers::ScopedLock lock(GetGlobalData()->settings_mutex);

  FrameCounterLayerSettings& settings = GetGlobalData()->settings;

  if (!settings.init) {
    GetSettingUint64("VkLayer_GF_frame_counter_START_FRAME",
                     "debug.gf.fc.start_frame", &settings.start_frame);
    GetSettingUint64("VkLayer_GF_frame_counter_END_FRAME",
                     "debug.gf.fc.end_frame", &settings.end_frame);
    GetSettingString("VkLayer_GF_frame_counter_OUTPUT_FILE",
                     "debug.gf.fc.output_file", &settings.output_file);
    settings.init = true;
  }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {
  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data = global_data->device_map.Get(DeviceKey(queue));

  // Call the original function.
  VkResult result = device_data->vkQueuePresentKHR(queue, pPresentInfo);

  // If the function succeeded:
  if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) {
    // If the start and end frame are the same then there is nothing we can do.
    // Return early.
    if (global_data->settings.start_frame == global_data->settings.end_frame) {
      return result;
    }

    // Atomically increment our frame counter.
    uint64_t current_frame = global_data->frame_counter++;

    // If we have hit the start frame...
    if (current_frame == global_data->settings.start_frame) {
      // Start the timer.
      auto start_time = std::chrono::steady_clock::now();
      // Although unlikely, another thread might be calling vkQueuePresentKHR
      // (targeting a different VkQueue) so that the "else if" block below for
      // the end_frame is executing concurrently. Hence, we use a mutex.
      {
        ScopedLock lock(global_data->start_time_mutex);
        global_data->start_time = start_time;
      }
    } else if (current_frame == global_data->settings.end_frame) {
      // We have hit the end frame.
      // Calculate the duration.
      auto end_time = std::chrono::steady_clock::now();
      std::chrono::steady_clock::time_point start_time;
      {
        ScopedLock lock(global_data->start_time_mutex);
        start_time = global_data->start_time;
      }

      auto duration = end_time - start_time;

      if (start_time == std::chrono::steady_clock::time_point{}) {
        // Start time was not initialized; this is unlikely but could happen via
        // concurrent calls to vkQueuePresentKHR.
        // Set duration to 0.
        duration = std::chrono::steady_clock::duration{};
      }

      // Write out the information to the output file.
      {
        // Write to a string stream first so we can log the information on
        // failure.
        std::ostringstream ss;
        ss << "Start frame: " << global_data->settings.start_frame << std::endl;
        ss << "End frame: " << global_data->settings.end_frame << std::endl;
        ss << "Frame count: "
           << (global_data->settings.end_frame -
               global_data->settings.start_frame)
           << std::endl;
        ss << "Duration: " << std::chrono::nanoseconds(duration).count() << "ns"
           << std::endl;

        // Write to the file.
        std::ofstream output_file_stream(global_data->settings.output_file);
        output_file_stream << ss.str() << std::flush;
        output_file_stream.close();

        // Log on failure.
        if (output_file_stream.fail()) {
          LOG("Failed to write the duration info to file %s. The information "
              "was: %s",
              global_data->settings.output_file.c_str(), ss.str().c_str());
        }
      }
    }
  }
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

  *pProperties = kLayerProperties[0];

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

  HANDLE(vkEnumerateDeviceExtensionProperties)
#undef HANDLE

  instance_data.vkGetInstanceProcAddr = next_get_instance_proc_address;

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

  DeviceData device_data{};
  device_data.device = *pDevice;
  device_data.instance_data = instance_data;

#define HANDLE(func)                                  \
  device_data.func = reinterpret_cast<PFN_##func>(    \
      next_get_device_proc_address(*pDevice, #func)); \
  if (!device_data.func) {                            \
    return VK_ERROR_INITIALIZATION_FAILED;            \
  }

  HANDLE(vkQueuePresentKHR)

#undef HANDLE

  device_data.vkGetDeviceProcAddr = next_get_device_proc_address;

  GetGlobalData()->device_map.Put(DeviceKey(*pDevice), device_data);

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
  HANDLE(vkQueuePresentKHR)

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
}  // namespace gf_layers::frame_counter_layer

//
// Exported functions.
//

extern "C" {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
VkLayer_GF_frame_counterNegotiateLoaderLayerInterfaceVersion(
    VkNegotiateLayerInterface* pVersionStruct) {
  DEBUG_LOG(
      "Entry point: "
      "VkLayer_GF_frame_counterNegotiateLoaderLayerInterfaceVersion");

  DEBUG_ASSERT(pVersionStruct);
  DEBUG_ASSERT(pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT);

  pVersionStruct->pfnGetInstanceProcAddr =
      gf_layers::frame_counter_layer::vkGetInstanceProcAddr;
  pVersionStruct->pfnGetDeviceProcAddr =
      gf_layers::frame_counter_layer::vkGetDeviceProcAddr;
  pVersionStruct->pfnGetPhysicalDeviceProcAddr = nullptr;

  if (pVersionStruct->loaderLayerInterfaceVersion >
      CURRENT_LOADER_LAYER_INTERFACE_VERSION) {
    pVersionStruct->loaderLayerInterfaceVersion =
        CURRENT_LOADER_LAYER_INTERFACE_VERSION;
  }

  return VK_SUCCESS;
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
VkLayer_GF_frame_counterGetInstanceProcAddr(VkInstance instance,
                                            const char* pName) {
  DEBUG_LOG("Entry point: VkLayer_GF_frame_counterGetInstanceProcAddr");

  return gf_layers::frame_counter_layer::vkGetInstanceProcAddr(instance, pName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
VkLayer_GF_frame_counterGetDeviceProcAddr(VkDevice device, const char* pName) {
  DEBUG_LOG("Entry point: VkLayer_GF_frame_counterGetDeviceProcAddr");

  return gf_layers::frame_counter_layer::vkGetDeviceProcAddr(device, pName);
}

#if defined(__ANDROID__)

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount,
                                   VkLayerProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateInstanceLayerProperties");

  return gf_layers::frame_counter_layer::vkEnumerateInstanceLayerProperties(
      pPropertyCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount,
    VkLayerProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateDeviceLayerProperties");

  return gf_layers::frame_counter_layer::vkEnumerateDeviceLayerProperties(
      physicalDevice, pPropertyCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char* pLayerName,
                                       uint32_t* pPropertyCount,
                                       VkExtensionProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateInstanceExtensionProperties");

  return gf_layers::frame_counter_layer::vkEnumerateInstanceExtensionProperties(
      pLayerName, pPropertyCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                     const char* pLayerName,
                                     uint32_t* pPropertyCount,
                                     VkExtensionProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateDeviceExtensionProperties");

  return gf_layers::frame_counter_layer::vkEnumerateDeviceExtensionProperties(
      physicalDevice, pLayerName, pPropertyCount, pProperties);
}

#endif

#pragma clang diagnostic pop

}  // extern "C"
