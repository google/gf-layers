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
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "gf_layers_layer_util/logging.h"
#include "gf_layers_layer_util/settings.h"
#include "gf_layers_layer_util/util.h"

#pragma GCC diagnostic push  // Clang, GCC.
#pragma warning(push, 1)     // MSVC: also reduces warning level to W1.

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"
#endif

// TODO(paulthomson): These are protobuf warning suppressions that could be
//  moved upstream to SPIRV-Tools or fixed in protobuf.
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#pragma GCC diagnostic ignored "-Wdeprecated-dynamic-exception-spec"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wextra-semi-stmt"
#pragma GCC diagnostic ignored "-Winconsistent-missing-destructor-override"
#pragma GCC diagnostic ignored "-Wunused-parameter"

// TODO(paulthomson): SPIRV-Tools warning suppressions.
#pragma GCC diagnostic ignored "-Wnewline-eof"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wweak-vtables"

// behavior change:
// __is_pod(google::protobuf::internal::AuxiliaryParseTableField) has different
// value in previous versions
#pragma warning(disable : 4647)

// reinterpret_cast used between related classes:
// 'google::protobuf::SourceContext' and 'google::protobuf::MessageLite'
#pragma warning(disable : 4946)

// 'initializing': conversion from '_Ty' to '_Ty1', signed/unsigned mismatch
#pragma warning(disable : 4365)

#include "google/protobuf/stubs/status.h"
#include "google/protobuf/util/json_util.h"
#include "source/fuzz/fuzzer.h"
#include "source/fuzz/fuzzer_util.h"
#include "source/fuzz/protobufs/spvtoolsfuzz.pb.h"
#include "source/fuzz/pseudo_random_generator.h"
#include "source/fuzz/random_generator.h"
#include "source/spirv_constant.h"
#include "spirv-tools/libspirv.h"
#include "spirv-tools/libspirv.hpp"
#include "tools/io.h"

#pragma warning(pop)
#pragma GCC diagnostic pop

namespace gf_layers::shader_fuzzer_layer {

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

  PFN_vkCreateShaderModule vkCreateShaderModule;
};

using InstanceMap = gf_layers::ProtectedTinyStaleMap<void*, InstanceData>;
using DeviceMap = gf_layers::ProtectedTinyStaleMap<void*, DeviceData>;

namespace {

// The number of digits used in output filenames.
// E.g. shader_00005_fuzzed.spv.
//             ^ 5 digits
const size_t kNumberPaddingInFilename = 6;

// Read-only once initialized.
struct ShaderFuzzerLayerSettings {
  bool init = false;

  // The output file path prefix.
  // E.g. output_prefix="/data/output/shader"
  // Some files:
  //   /data/output/shader_000001.spv
  //   /data/output/shader_000002.spv
  //   ...
  std::string output_prefix = "shader";
};

struct GlobalData {
  InstanceMap instance_map;
  DeviceMap device_map;

  // Counter used to make unique output filenames.
  std::atomic<uint64_t> shader_module_counter{};

  // In vkCreateInstance, we initialize |settings| by reading environment
  // variables while holding |settings_mutex|, after which |settings| is
  // read-only. Thus, in instance or device functions (such as
  // vkQueuePresentKHR) we can read |settings| without holding |settings_mutex|.
  gf_layers::MutexType settings_mutex;
  ShaderFuzzerLayerSettings settings;
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
    "VkLayer_GF_shader_fuzzer",     // layerName
    VK_MAKE_VERSION(1U, 1U, 130U),  // specVersion NOLINT(hicpp-signed-bitwise)
    1,                              // implementationVersion
    "Shader fuzzer layer.",         // description
}}};

bool IsThisLayer(const char* pLayerName) {
  return ((pLayerName != nullptr) &&
          strcmp(pLayerName, kLayerProperties[0].layerName) == 0);
}

void InitSettingsIfNeeded() {
  gf_layers::ScopedLock lock(GetGlobalData()->settings_mutex);

  ShaderFuzzerLayerSettings& settings = GetGlobalData()->settings;

  if (!settings.init) {
    get_setting_string("VkLayer_GF_shader_fuzzer_OUTPUT_PREFIX",
                       "debug.gf.sf.output_prefix", &settings.output_prefix);
    settings.init = true;
  }
}

// Returns an empty vector if fuzzing was not possible.  Otherwise, returns a
// vector representing the fuzzed version of the shader referred to by
// |pCreateInfo->pCode|.
std::vector<uint32_t> TryFuzzingShader(
    VkShaderModuleCreateInfo const* pCreateInfo, GlobalData* global_data) {
  // Grab a new suffix for this shader module.
  uint64_t shader_module_number = global_data->shader_module_counter++;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  uint32_t version_word = pCreateInfo->pCode[1];

  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  uint8_t major_version = SPV_SPIRV_VERSION_MAJOR_PART(version_word);
  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  uint8_t minor_version = SPV_SPIRV_VERSION_MINOR_PART(version_word);

  if (major_version != 1) {
    LOG("Unknown SPIR-V major version %u; shader %" PRIu64
        " will not be fuzzed.",
        major_version, shader_module_number);
    return {};
  }

  spv_target_env target_env;  // NOLINT(cppcoreguidelines-init-variables)

  switch (minor_version) {
    case 0:
      target_env = SPV_ENV_UNIVERSAL_1_0;
      break;
    case 1:
      target_env = SPV_ENV_UNIVERSAL_1_1;
      break;
    case 2:
      target_env = SPV_ENV_UNIVERSAL_1_2;
      break;
    case 3:
      target_env = SPV_ENV_UNIVERSAL_1_3;
      break;
    case 4:
      target_env = SPV_ENV_UNIVERSAL_1_4;
      break;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    case 5:
      target_env = SPV_ENV_UNIVERSAL_1_5;
      break;
    default:
      LOG("Unknown SPIR-V minor version %u; shader %" PRIu64
          " will not be fuzzed.",
          minor_version, shader_module_number);
      return std::vector<uint32_t>();
  }

  // |pCreateInfo->codeSize| gives the size in bytes; convert it to words.
  const uint32_t code_size_in_words =
      static_cast<uint32_t>(pCreateInfo->codeSize) / 4;

  spvtools::SpirvTools tools(target_env);

  if (!tools.IsValid()) {
    LOG("Did not manage to create a SPIRV-Tools instance; shaders will not be "
        "fuzzed.");
    return {};
  }

  // Create a fuzzer and the various parameters required for fuzzing.
  spvtools::ValidatorOptions validator_options;
  std::vector<uint32_t> binary_in(
      pCreateInfo->pCode,
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      pCreateInfo->pCode + code_size_in_words);

  spvtools::fuzz::protobufs::FactSequence no_facts;
  std::vector<spvtools::fuzz::fuzzerutil::ModuleSupplier> no_donors;

  spvtools::MessageConsumer message_consumer =
      [](spv_message_level_t level, const char* source,
         const spv_position_t& position, const char* message) {
        (void)source;  // This parameter is deliberately unused.
        switch (level) {
          case SPV_MSG_FATAL:
          case SPV_MSG_INTERNAL_ERROR:
          case SPV_MSG_ERROR:
            LOG("error: line %zu: %s", position.index, message);
            break;
          case SPV_MSG_WARNING:
            LOG("warning: line %zu: %s", position.index, message);
            break;
          case SPV_MSG_INFO:
            LOG("info: line %zu: %s", position.index, message);
            break;
          case SPV_MSG_DEBUG:
            LOG("debug: line %zu: %s", position.index, message);
            break;
        }
      };

  // Fuzz the shader.
  auto fuzzer_result =
      spvtools::fuzz::Fuzzer(
          target_env, message_consumer, binary_in, no_facts, no_donors,
          std::make_unique<spvtools::fuzz::PseudoRandomGenerator>(
              static_cast<uint32_t>(shader_module_number)),
          false,
          spvtools::fuzz::Fuzzer::RepeatedPassStrategy::
              kLoopedWithRecommendations,
          true, validator_options)
          .Run();

  if (fuzzer_result.status !=
      spvtools::fuzz::Fuzzer::FuzzerResultStatus::kComplete) {
    LOG("Fuzzing failed.");
    return {};
  }

  // Get the shader module number as a string.
  std::string shader_module_number_padded;
  {
    std::stringstream shader_module_number_stream;
    shader_module_number_stream << std::setfill('0')
                                << std::setw(kNumberPaddingInFilename)
                                << shader_module_number;
    shader_module_number_padded = shader_module_number_stream.str();
  }

  // Write out the original shader module.
  {
    std::stringstream original_shader_binary_name;
    original_shader_binary_name << global_data->settings.output_prefix << "_"
                                << shader_module_number_padded
                                << "_original.spv";
    WriteFile<uint32_t>(original_shader_binary_name.str().c_str(), "wb",
                        pCreateInfo->pCode, code_size_in_words);
  }

  // Write out the fuzzed shader module
  {
    std::stringstream fuzzed_shader_binary_name;
    fuzzed_shader_binary_name << global_data->settings.output_prefix << "_"
                              << shader_module_number_padded << "_fuzzed.spv";
    WriteFile<uint32_t>(fuzzed_shader_binary_name.str().c_str(), "wb",
                        fuzzer_result.transformed_binary.data(),
                        fuzzer_result.transformed_binary.size());
  }

  // Write out the transformations
  {
    std::stringstream transformations_name;
    transformations_name << global_data->settings.output_prefix << "_"
                         << shader_module_number_padded
                         << "_fuzzed.transformations";
    std::ofstream transformations_file(transformations_name.str(),
                                       std::ios::out | std::ios::binary);
    fuzzer_result.applied_transformations.SerializeToOstream(
        &transformations_file);
  }

  // Write out the transformations in JSON format
  {
    std::stringstream transformations_json_name;
    transformations_json_name << global_data->settings.output_prefix << "_"
                              << shader_module_number_padded
                              << "_fuzzed.transformations_json";

    std::string json_string;
    auto json_options = google::protobuf::util::JsonPrintOptions();
    json_options.add_whitespace = true;

    auto json_generation_status = google::protobuf::util::MessageToJsonString(
        fuzzer_result.applied_transformations, &json_string, json_options);

    if (json_generation_status == google::protobuf::util::Status::OK) {
      std::ofstream transformations_json_file(transformations_json_name.str());
      transformations_json_file << json_string;
    }
  }

  return fuzzer_result.transformed_binary;
}  // namespace

VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(
    VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) {
  GlobalData* global_data = GetGlobalData();
  DeviceData* device_data = global_data->device_map.get(device_key(device));

  // Fuzzing the provided shader will either yield an empty vector - if
  // something went wrong - or a vector whose contents is the fuzzed shader
  // binary.
  std::vector<uint32_t> fuzzed = TryFuzzingShader(pCreateInfo, global_data);

  // If we did not succeed in fuzzing the shader, just call the original
  // function.
  if (fuzzed.empty()) {
    return device_data->vkCreateShaderModule(device, pCreateInfo, pAllocator,
                                             pShaderModule);
  }

  // We succeeded in fuzzing the shader, so pass on a pointer to a new
  // VkShaderModuleCreateInfo object identical to the original, except with
  // the fuzzed shader data.
  VkShaderModuleCreateInfo fuzzed_shader_module_create_info{
      pCreateInfo->sType,  // sType
      pCreateInfo->pNext,  // pNext
      pCreateInfo->flags,  // flags
      fuzzed.size() * 4,   // codeSize
      fuzzed.data(),       // pCode
  };

  // Call the original function with our create info.
  return device_data->vkCreateShaderModule(
      device, &fuzzed_shader_module_create_info, pAllocator, pShaderModule);
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

  DeviceData device_data{};
  device_data.device = *pDevice;
  device_data.instance_data = instance_data;

#define HANDLE(func)                                  \
  device_data.func = reinterpret_cast<PFN_##func>(    \
      next_get_device_proc_address(*pDevice, #func)); \
  if (!device_data.func) {                            \
    return VK_ERROR_INITIALIZATION_FAILED;            \
  }

  HANDLE(vkGetDeviceProcAddr)
  HANDLE(vkCreateShaderModule)

#undef HANDLE

  DEBUG_ASSERT(next_get_device_proc_address == device_data.vkGetDeviceProcAddr);

  GetGlobalData()->device_map.put(device_key(*pDevice), device_data);

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
  HANDLE(vkCreateShaderModule)

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
}  // namespace gf_layers::shader_fuzzer_layer

//
// Exported functions.
//

extern "C" {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
VkLayer_GF_shader_fuzzerNegotiateLoaderLayerInterfaceVersion(
    VkNegotiateLayerInterface* pVersionStruct) {
  DEBUG_LOG(
      "Entry point: "
      "VkLayer_GF_shader_fuzzerNegotiateLoaderLayerInterfaceVersion");

  DEBUG_ASSERT(pVersionStruct);
  DEBUG_ASSERT(pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT);

  pVersionStruct->pfnGetInstanceProcAddr =
      gf_layers::shader_fuzzer_layer::vkGetInstanceProcAddr;
  pVersionStruct->pfnGetDeviceProcAddr =
      gf_layers::shader_fuzzer_layer::vkGetDeviceProcAddr;
  pVersionStruct->pfnGetPhysicalDeviceProcAddr = nullptr;

  if (pVersionStruct->loaderLayerInterfaceVersion >
      CURRENT_LOADER_LAYER_INTERFACE_VERSION) {
    pVersionStruct->loaderLayerInterfaceVersion =
        CURRENT_LOADER_LAYER_INTERFACE_VERSION;
  }

  return VK_SUCCESS;
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
VkLayer_GF_shader_fuzzerGetInstanceProcAddr(VkInstance instance,
                                            const char* pName) {
  DEBUG_LOG("Entry point: VkLayer_GF_shader_fuzzerGetInstanceProcAddr");

  return gf_layers::shader_fuzzer_layer::vkGetInstanceProcAddr(instance, pName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
VkLayer_GF_shader_fuzzerGetDeviceProcAddr(VkDevice device, const char* pName) {
  DEBUG_LOG("Entry point: VkLayer_GF_shader_fuzzerGetDeviceProcAddr");

  return gf_layers::shader_fuzzer_layer::vkGetDeviceProcAddr(device, pName);
}

#if defined(__ANDROID__)

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount,
                                   VkLayerProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateInstanceLayerProperties");

  return gf_layers::shader_fuzzer_layer::vkEnumerateInstanceLayerProperties(
      pPropertyCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount,
    VkLayerProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateDeviceLayerProperties");

  return gf_layers::shader_fuzzer_layer::vkEnumerateDeviceLayerProperties(
      physicalDevice, pPropertyCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char* pLayerName,
                                       uint32_t* pPropertyCount,
                                       VkExtensionProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateInstanceExtensionProperties");

  return gf_layers::shader_fuzzer_layer::vkEnumerateInstanceExtensionProperties(
      pLayerName, pPropertyCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                     const char* pLayerName,
                                     uint32_t* pPropertyCount,
                                     VkExtensionProperties* pProperties) {
  DEBUG_LOG("Entry point: vkEnumerateDeviceExtensionProperties");

  return gf_layers::shader_fuzzer_layer::vkEnumerateDeviceExtensionProperties(
      physicalDevice, pLayerName, pPropertyCount, pProperties);
}

#endif

#pragma clang diagnostic pop

}  // extern "C"
