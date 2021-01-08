// Copyright 2021 The gf-layers Project Authors
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

#include "gf_layers_opencl_capture/opencl_capture.h"

#include <dlfcn.h>

#include <cstdio>

#include "gf_layers_layer_util/logging.h"

namespace gf_layers::opencl_capture {
namespace {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"

// Global variables (that are not constinit) are strongly discouraged.
// However, we should be fine as long as all global data is stored in this
// struct.
GlobalData global_data_;  // NOLINT(cert-err58-cpp)
#pragma clang diagnostic pop

GlobalData* GetGlobalData() { return &global_data_; }

}  // namespace

GlobalData::GlobalData() {
  void* opencl_pixel = dlopen("libOpenCL-pixel.so", RTLD_NOW | RTLD_LOCAL);
  RUNTIME_ASSERT(opencl_pixel);
  typedef void (*PFN_enable_opencl)();
  auto enable_opencl =
      reinterpret_cast<PFN_enable_opencl>(dlsym(opencl_pixel, "enableOpenCL"));
  RUNTIME_ASSERT(enable_opencl);
  enable_opencl();
  typedef void* (*PFN_load_opencl_pointer)(const char* name);
  auto load_opencl_pointer = reinterpret_cast<PFN_load_opencl_pointer>(
      dlsym(opencl_pixel, "loadOpenCLPointer"));
  RUNTIME_ASSERT(load_opencl_pointer);

#define LoadFunction(function) \
  opencl_pointers.function =   \
      reinterpret_cast<PFN_##function>(load_opencl_pointer(#function))

#include "gf_layers_opencl_capture/opencl_load_functions.inc"

#undef LoadFunction
}

}  // namespace gf_layers::opencl_capture

extern "C" {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
CL_API_ENTRY cl_int CL_API_CALL clGetDeviceIDsFromD3D10KHR(
    cl_platform_id platform, cl_d3d10_device_source_khr d3d_device_source,
    void* d3d_object, cl_d3d10_device_set_khr d3d_device_set,
    cl_uint num_entries, cl_device_id* devices,
    cl_uint* num_devices) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetDeviceIDsFromD3D10KHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetDeviceIDsFromD3D10KHR(
          platform, d3d_device_source, d3d_object, d3d_device_set, num_entries,
          devices, num_devices);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromD3D10BufferKHR(
    cl_context context, cl_mem_flags flags, ID3D10Buffer* resource,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateFromD3D10BufferKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromD3D10BufferKHR(context, flags, resource,
                                                   errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromD3D10Texture2DKHR(
    cl_context context, cl_mem_flags flags, ID3D10Texture2D* resource,
    UINT subresource, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateFromD3D10Texture2DKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromD3D10Texture2DKHR(context, flags, resource,
                                                      subresource, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromD3D10Texture3DKHR(
    cl_context context, cl_mem_flags flags, ID3D10Texture3D* resource,
    UINT subresource, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateFromD3D10Texture3DKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromD3D10Texture3DKHR(context, flags, resource,
                                                      subresource, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueAcquireD3D10ObjectsKHR(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueAcquireD3D10ObjectsKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueAcquireD3D10ObjectsKHR(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReleaseD3D10ObjectsKHR(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueReleaseD3D10ObjectsKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReleaseD3D10ObjectsKHR(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clGetDeviceIDsFromD3D11KHR(
    cl_platform_id platform, cl_d3d11_device_source_khr d3d_device_source,
    void* d3d_object, cl_d3d11_device_set_khr d3d_device_set,
    cl_uint num_entries, cl_device_id* devices,
    cl_uint* num_devices) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clGetDeviceIDsFromD3D11KHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetDeviceIDsFromD3D11KHR(
          platform, d3d_device_source, d3d_object, d3d_device_set, num_entries,
          devices, num_devices);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromD3D11BufferKHR(
    cl_context context, cl_mem_flags flags, ID3D11Buffer* resource,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clCreateFromD3D11BufferKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromD3D11BufferKHR(context, flags, resource,
                                                   errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromD3D11Texture2DKHR(
    cl_context context, cl_mem_flags flags, ID3D11Texture2D* resource,
    UINT subresource, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clCreateFromD3D11Texture2DKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromD3D11Texture2DKHR(context, flags, resource,
                                                      subresource, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromD3D11Texture3DKHR(
    cl_context context, cl_mem_flags flags, ID3D11Texture3D* resource,
    UINT subresource, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clCreateFromD3D11Texture3DKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromD3D11Texture3DKHR(context, flags, resource,
                                                      subresource, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueAcquireD3D11ObjectsKHR(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueAcquireD3D11ObjectsKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueAcquireD3D11ObjectsKHR(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReleaseD3D11ObjectsKHR(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueReleaseD3D11ObjectsKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReleaseD3D11ObjectsKHR(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clGetDeviceIDsFromDX9MediaAdapterKHR(
    cl_platform_id platform, cl_uint num_media_adapters,
    cl_dx9_media_adapter_type_khr* media_adapter_type, void* media_adapters,
    cl_dx9_media_adapter_set_khr media_adapter_set, cl_uint num_entries,
    cl_device_id* devices, cl_uint* num_devices) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clGetDeviceIDsFromDX9MediaAdapterKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetDeviceIDsFromDX9MediaAdapterKHR(
          platform, num_media_adapters, media_adapter_type, media_adapters,
          media_adapter_set, num_entries, devices, num_devices);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromDX9MediaSurfaceKHR(
    cl_context context, cl_mem_flags flags,
    cl_dx9_media_adapter_type_khr adapter_type, void* surface_info,
    cl_uint plane, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clCreateFromDX9MediaSurfaceKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromDX9MediaSurfaceKHR(
          context, flags, adapter_type, surface_info, plane, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueAcquireDX9MediaSurfacesKHR(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueAcquireDX9MediaSurfacesKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueAcquireDX9MediaSurfacesKHR(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReleaseDX9MediaSurfacesKHR(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueReleaseDX9MediaSurfacesKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReleaseDX9MediaSurfacesKHR(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clGetDeviceIDsFromDX9INTEL(
    cl_platform_id platform, cl_dx9_device_source_intel dx9_device_source,
    void* dx9_object, cl_dx9_device_set_intel dx9_device_set,
    cl_uint num_entries, cl_device_id* devices,
    cl_uint* num_devices) CL_EXT_SUFFIX__VERSION_1_1 {
  LOG("clGetDeviceIDsFromDX9INTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetDeviceIDsFromDX9INTEL(
          platform, dx9_device_source, dx9_object, dx9_device_set, num_entries,
          devices, num_devices);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromDX9MediaSurfaceINTEL(
    cl_context context, cl_mem_flags flags, IDirect3DSurface9* resource,
    HANDLE sharedHandle, UINT plane,
    cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_1 {
  LOG("clCreateFromDX9MediaSurfaceINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromDX9MediaSurfaceINTEL(
          context, flags, resource, sharedHandle, plane, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueAcquireDX9ObjectsINTEL(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_1 {
  LOG("clEnqueueAcquireDX9ObjectsINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueAcquireDX9ObjectsINTEL(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReleaseDX9ObjectsINTEL(
    cl_command_queue command_queue, cl_uint num_objects, cl_mem* mem_objects,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_1 {
  LOG("clEnqueueReleaseDX9ObjectsINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReleaseDX9ObjectsINTEL(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_event CL_API_CALL clCreateEventFromEGLSyncKHR(
    cl_context context, CLeglSyncKHR sync, CLeglDisplayKHR display,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateEventFromEGLSyncKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateEventFromEGLSyncKHR(context, sync, display,
                                                    errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromEGLImageKHR(
    cl_context context, CLeglDisplayKHR egldisplay, CLeglImageKHR eglimage,
    cl_mem_flags flags, const cl_egl_image_properties_khr* properties,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateFromEGLImageKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromEGLImageKHR(context, egldisplay, eglimage,
                                                flags, properties, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueAcquireEGLObjectsKHR(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueAcquireEGLObjectsKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueAcquireEGLObjectsKHR(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReleaseEGLObjectsKHR(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueReleaseEGLObjectsKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReleaseEGLObjectsKHR(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY void CL_API_CALL clLogMessagesToSystemLogAPPLE(
    const char* errstr, const void* private_info, size_t cb,
    void* user_data) CL_EXT_SUFFIX__VERSION_1_0 {
  LOG("clLogMessagesToSystemLogAPPLE");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clLogMessagesToSystemLogAPPLE(errstr, private_info, cb,
                                                      user_data);
}

CL_API_ENTRY void CL_API_CALL clLogMessagesToStdoutAPPLE(
    const char* errstr, const void* private_info, size_t cb,
    void* user_data) CL_EXT_SUFFIX__VERSION_1_0 {
  LOG("clLogMessagesToStdoutAPPLE");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clLogMessagesToStdoutAPPLE(errstr, private_info, cb,
                                                   user_data);
}

CL_API_ENTRY void CL_API_CALL clLogMessagesToStderrAPPLE(
    const char* errstr, const void* private_info, size_t cb,
    void* user_data) CL_EXT_SUFFIX__VERSION_1_0 {
  LOG("clLogMessagesToStderrAPPLE");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clLogMessagesToStderrAPPLE(errstr, private_info, cb,
                                                   user_data);
}

CL_API_ENTRY cl_int CL_API_CALL clIcdGetPlatformIDsKHR(
    cl_uint num_entries, cl_platform_id* platforms, cl_uint* num_platforms) {
  LOG("clIcdGetPlatformIDsKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clIcdGetPlatformIDsKHR(num_entries, platforms,
                                               num_platforms);
}

CL_API_ENTRY cl_program CL_API_CALL clCreateProgramWithILKHR(
    cl_context context, const void* il, size_t length, cl_int* errcode_ret) {
  LOG("clCreateProgramWithILKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateProgramWithILKHR(context, il, length,
                                                 errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clTerminateContextKHR(cl_context context)
    CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clTerminateContextKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clTerminateContextKHR(context);
}

CL_API_ENTRY cl_command_queue CL_API_CALL clCreateCommandQueueWithPropertiesKHR(
    cl_context context, cl_device_id device,
    const cl_queue_properties_khr* properties,
    cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clCreateCommandQueueWithPropertiesKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateCommandQueueWithPropertiesKHR(
          context, device, properties, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseDeviceEXT(cl_device_id device)
    CL_EXT_SUFFIX__VERSION_1_1 {
  LOG("clReleaseDeviceEXT");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clReleaseDeviceEXT(device);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainDeviceEXT(cl_device_id device)
    CL_EXT_SUFFIX__VERSION_1_1 {
  LOG("clRetainDeviceEXT");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clRetainDeviceEXT(
      device);
}

CL_API_ENTRY cl_int CL_API_CALL clCreateSubDevicesEXT(
    cl_device_id in_device, const cl_device_partition_property_ext* properties,
    cl_uint num_entries, cl_device_id* out_devices,
    cl_uint* num_devices) CL_EXT_SUFFIX__VERSION_1_1 {
  LOG("clCreateSubDevicesEXT");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateSubDevicesEXT(
          in_device, properties, num_entries, out_devices, num_devices);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueMigrateMemObjectEXT(
    cl_command_queue command_queue, cl_uint num_mem_objects,
    const cl_mem* mem_objects, cl_mem_migration_flags_ext flags,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) {
  LOG("clEnqueueMigrateMemObjectEXT");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueMigrateMemObjectEXT(
          command_queue, num_mem_objects, mem_objects, flags,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clGetDeviceImageInfoQCOM(
    cl_device_id device, size_t image_width, size_t image_height,
    const cl_image_format* image_format, cl_image_pitch_info_qcom param_name,
    size_t param_value_size, void* param_value, size_t* param_value_size_ret) {
  LOG("clGetDeviceImageInfoQCOM");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetDeviceImageInfoQCOM(
          device, image_width, image_height, image_format, param_name,
          param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueAcquireGrallocObjectsIMG(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueAcquireGrallocObjectsIMG");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueAcquireGrallocObjectsIMG(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueGenerateMipmapIMG(
    cl_command_queue command_queue, cl_mem src_image, cl_mem dst_image,
    cl_mipmap_filter_mode_img mipmap_filter_mode, const size_t* array_region,
    const size_t* mip_region, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueGenerateMipmapIMG");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueGenerateMipmapIMG(
          command_queue, src_image, dst_image, mipmap_filter_mode, array_region,
          mip_region, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReleaseGrallocObjectsIMG(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueReleaseGrallocObjectsIMG");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReleaseGrallocObjectsIMG(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clGetKernelSubGroupInfoKHR(
    cl_kernel in_kernel, cl_device_id in_device,
    cl_kernel_sub_group_info param_name, size_t input_value_size,
    const void* input_value, size_t param_value_size, void* param_value,
    size_t* param_value_size_ret) CL_EXT_SUFFIX__VERSION_2_0_DEPRECATED {
  LOG("clGetKernelSubGroupInfoKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetKernelSubGroupInfoKHR(
          in_kernel, in_device, param_name, input_value_size, input_value,
          param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL
clImportMemoryARM(cl_context context, cl_mem_flags flags,
                  const cl_import_properties_arm* properties, void* memory,
                  size_t size, cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_0 {
  LOG("clImportMemoryARM");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clImportMemoryARM(
      context, flags, properties, memory, size, errcode_ret);
}

CL_API_ENTRY void* CL_API_CALL
clSVMAllocARM(cl_context context, cl_svm_mem_flags_arm flags, size_t size,
              cl_uint alignment) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clSVMAllocARM");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clSVMAllocARM(
      context, flags, size, alignment);
}

CL_API_ENTRY void CL_API_CALL
clSVMFreeARM(cl_context context, void* svm_pointer) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clSVMFreeARM");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clSVMFreeARM(
      context, svm_pointer);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMFreeARM(
    cl_command_queue command_queue, cl_uint num_svm_pointers,
    void* svm_pointers[],
    void(CL_CALLBACK* pfn_free_func)(cl_command_queue queue,
                                     cl_uint num_svm_pointers,
                                     void* svm_pointers[], void* user_data),
    void* user_data, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueSVMFreeARM");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueSVMFreeARM(
          command_queue, num_svm_pointers, svm_pointers, pfn_free_func,
          user_data, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMemcpyARM(
    cl_command_queue command_queue, cl_bool blocking_copy, void* dst_ptr,
    const void* src_ptr, size_t size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueSVMMemcpyARM");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueSVMMemcpyARM(
          command_queue, blocking_copy, dst_ptr, src_ptr, size,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMemFillARM(
    cl_command_queue command_queue, void* svm_ptr, const void* pattern,
    size_t pattern_size, size_t size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueSVMMemFillARM");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueSVMMemFillARM(
          command_queue, svm_ptr, pattern, pattern_size, size,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMapARM(
    cl_command_queue command_queue, cl_bool blocking_map, cl_map_flags flags,
    void* svm_ptr, size_t size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueSVMMapARM");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueSVMMapARM(
          command_queue, blocking_map, flags, svm_ptr, size,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMUnmapARM(
    cl_command_queue command_queue, void* svm_ptr,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueSVMUnmapARM");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueSVMUnmapARM(command_queue, svm_ptr,
                                             num_events_in_wait_list,
                                             event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL
clSetKernelArgSVMPointerARM(cl_kernel kernel, cl_uint arg_index,
                            const void* arg_value) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clSetKernelArgSVMPointerARM");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetKernelArgSVMPointerARM(kernel, arg_index,
                                                    arg_value);
}

CL_API_ENTRY cl_int CL_API_CALL
clSetKernelExecInfoARM(cl_kernel kernel, cl_kernel_exec_info_arm param_name,
                       size_t param_value_size,
                       const void* param_value) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clSetKernelExecInfoARM");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetKernelExecInfoARM(kernel, param_name,
                                               param_value_size, param_value);
}

CL_API_ENTRY cl_accelerator_intel CL_API_CALL clCreateAcceleratorINTEL(
    cl_context context, cl_accelerator_type_intel accelerator_type,
    size_t descriptor_size, const void* descriptor,
    cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clCreateAcceleratorINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateAcceleratorINTEL(
          context, accelerator_type, descriptor_size, descriptor, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clGetAcceleratorInfoINTEL(
    cl_accelerator_intel accelerator, cl_accelerator_info_intel param_name,
    size_t param_value_size, void* param_value,
    size_t* param_value_size_ret) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clGetAcceleratorInfoINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetAcceleratorInfoINTEL(accelerator, param_name,
                                                  param_value_size, param_value,
                                                  param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainAcceleratorINTEL(
    cl_accelerator_intel accelerator) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clRetainAcceleratorINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clRetainAcceleratorINTEL(accelerator);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseAcceleratorINTEL(
    cl_accelerator_intel accelerator) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clReleaseAcceleratorINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clReleaseAcceleratorINTEL(accelerator);
}

CL_API_ENTRY cl_event CL_API_CALL
clCreateEventFromGLsyncKHR(cl_context context, cl_GLsync sync,
                           cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_1 {
  LOG("clCreateEventFromGLsyncKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateEventFromGLsyncKHR(context, sync, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clGetGLContextInfoKHR(
    const cl_context_properties* properties, cl_gl_context_info param_name,
    size_t param_value_size, void* param_value,
    size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetGLContextInfoKHR");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetGLContextInfoKHR(properties, param_name,
                                              param_value_size, param_value,
                                              param_value_size_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLBuffer(cl_context context, cl_mem_flags flags, cl_GLuint bufobj,
                     int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateFromGLBuffer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromGLBuffer(context, flags, bufobj,
                                             errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromGLTexture(
    cl_context context, cl_mem_flags flags, cl_GLenum target, cl_GLint miplevel,
    cl_GLuint texture, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clCreateFromGLTexture");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromGLTexture(context, flags, target, miplevel,
                                              texture, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromGLRenderbuffer(
    cl_context context, cl_mem_flags flags, cl_GLuint renderbuffer,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateFromGLRenderbuffer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromGLRenderbuffer(context, flags, renderbuffer,
                                                   errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetGLObjectInfo(cl_mem memobj, cl_gl_object_type* gl_object_type,
                  cl_GLuint* gl_object_name) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetGLObjectInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetGLObjectInfo(
      memobj, gl_object_type, gl_object_name);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetGLTextureInfo(cl_mem memobj, cl_gl_texture_info param_name,
                   size_t param_value_size, void* param_value,
                   size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetGLTextureInfo");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetGLTextureInfo(memobj, param_name, param_value_size,
                                           param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueAcquireGLObjects(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueAcquireGLObjects");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueAcquireGLObjects(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReleaseGLObjects(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueReleaseGLObjects");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReleaseGLObjects(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromGLTexture2D(
    cl_context context, cl_mem_flags flags, cl_GLenum target, cl_GLint miplevel,
    cl_GLuint texture,
    cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED {
  LOG("clCreateFromGLTexture2D");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromGLTexture2D(context, flags, target,
                                                miplevel, texture, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromGLTexture3D(
    cl_context context, cl_mem_flags flags, cl_GLenum target, cl_GLint miplevel,
    cl_GLuint texture,
    cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED {
  LOG("clCreateFromGLTexture3D");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromGLTexture3D(context, flags, target,
                                                miplevel, texture, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clGetDeviceIDsFromVA_APIMediaAdapterINTEL(
    cl_platform_id platform, cl_va_api_device_source_intel media_adapter_type,
    void* media_adapter, cl_va_api_device_set_intel media_adapter_set,
    cl_uint num_entries, cl_device_id* devices,
    cl_uint* num_devices) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clGetDeviceIDsFromVA_APIMediaAdapterINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetDeviceIDsFromVA_APIMediaAdapterINTEL(
          platform, media_adapter_type, media_adapter, media_adapter_set,
          num_entries, devices, num_devices);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateFromVA_APIMediaSurfaceINTEL(
    cl_context context, cl_mem_flags flags, VASurfaceID* surface, cl_uint plane,
    cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clCreateFromVA_APIMediaSurfaceINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateFromVA_APIMediaSurfaceINTEL(
          context, flags, surface, plane, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueAcquireVA_APIMediaSurfacesINTEL(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueAcquireVA_APIMediaSurfacesINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueAcquireVA_APIMediaSurfacesINTEL(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReleaseVA_APIMediaSurfacesINTEL(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_EXT_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueReleaseVA_APIMediaSurfacesINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReleaseVA_APIMediaSurfacesINTEL(
          command_queue, num_objects, mem_objects, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY void* CL_API_CALL clHostMemAllocINTEL(
    cl_context context, const cl_mem_properties_intel* properties, size_t size,
    cl_uint alignment, cl_int* errcode_ret) {
  LOG("clHostMemAllocINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clHostMemAllocINTEL(context, properties, size,
                                            alignment, errcode_ret);
}

CL_API_ENTRY void* CL_API_CALL
clDeviceMemAllocINTEL(cl_context context, cl_device_id device,
                      const cl_mem_properties_intel* properties, size_t size,
                      cl_uint alignment, cl_int* errcode_ret) {
  LOG("clDeviceMemAllocINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clDeviceMemAllocINTEL(context, device, properties, size,
                                              alignment, errcode_ret);
}

CL_API_ENTRY void* CL_API_CALL
clSharedMemAllocINTEL(cl_context context, cl_device_id device,
                      const cl_mem_properties_intel* properties, size_t size,
                      cl_uint alignment, cl_int* errcode_ret) {
  LOG("clSharedMemAllocINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSharedMemAllocINTEL(context, device, properties, size,
                                              alignment, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clMemFreeINTEL(cl_context context, void* ptr) {
  LOG("clMemFreeINTEL");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clMemFreeINTEL(
      context, ptr);
}

CL_API_ENTRY cl_int CL_API_CALL clGetMemAllocInfoINTEL(
    cl_context context, const void* ptr, cl_mem_info_intel param_name,
    size_t param_value_size, void* param_value, size_t* param_value_size_ret) {
  LOG("clGetMemAllocInfoINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetMemAllocInfoINTEL(context, ptr, param_name,
                                               param_value_size, param_value,
                                               param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clSetKernelArgMemPointerINTEL(
    cl_kernel kernel, cl_uint arg_index, const void* arg_value) {
  LOG("clSetKernelArgMemPointerINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetKernelArgMemPointerINTEL(kernel, arg_index,
                                                      arg_value);
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueMemsetINTEL(cl_command_queue command_queue, void* dst_ptr,
                     cl_int value, size_t size, cl_uint num_events_in_wait_list,
                     const cl_event* event_wait_list, cl_event* event) {
  LOG("clEnqueueMemsetINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueMemsetINTEL(command_queue, dst_ptr, value,
                                             size, num_events_in_wait_list,
                                             event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueMemFillINTEL(
    cl_command_queue command_queue, void* dst_ptr, const void* pattern,
    size_t pattern_size, size_t size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list, cl_event* event) {
  LOG("clEnqueueMemFillINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueMemFillINTEL(
          command_queue, dst_ptr, pattern, pattern_size, size,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueMemcpyINTEL(
    cl_command_queue command_queue, cl_bool blocking, void* dst_ptr,
    const void* src_ptr, size_t size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list, cl_event* event) {
  LOG("clEnqueueMemcpyINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueMemcpyINTEL(
          command_queue, blocking, dst_ptr, src_ptr, size,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueMigrateMemINTEL(
    cl_command_queue command_queue, const void* ptr, size_t size,
    cl_mem_migration_flags flags, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list, cl_event* event) {
  LOG("clEnqueueMigrateMemINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueMigrateMemINTEL(command_queue, ptr, size,
                                                 flags, num_events_in_wait_list,
                                                 event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueMemAdviseINTEL(
    cl_command_queue command_queue, const void* ptr, size_t size,
    cl_mem_advice_intel advice, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list, cl_event* event) {
  LOG("clEnqueueMemAdviseINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueMemAdviseINTEL(command_queue, ptr, size,
                                                advice, num_events_in_wait_list,
                                                event_wait_list, event);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateBufferWithPropertiesINTEL(
    cl_context context, const cl_mem_properties_intel* properties,
    cl_mem_flags flags, size_t size, void* host_ptr, cl_int* errcode_ret) {
  LOG("clCreateBufferWithPropertiesINTEL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateBufferWithPropertiesINTEL(
          context, properties, flags, size, host_ptr, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetPlatformIDs(cl_uint num_entries, cl_platform_id* platforms,
                 cl_uint* num_platforms) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetPlatformIDs");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetPlatformIDs(
      num_entries, platforms, num_platforms);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetPlatformInfo(cl_platform_id platform, cl_platform_info param_name,
                  size_t param_value_size, void* param_value,
                  size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetPlatformInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetPlatformInfo(
      platform, param_name, param_value_size, param_value,
      param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clGetDeviceIDs(
    cl_platform_id platform, cl_device_type device_type, cl_uint num_entries,
    cl_device_id* devices, cl_uint* num_devices) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetDeviceIDs");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetDeviceIDs(
      platform, device_type, num_entries, devices, num_devices);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceInfo(cl_device_id device, cl_device_info param_name,
                size_t param_value_size, void* param_value,
                size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetDeviceInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetDeviceInfo(
      device, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clCreateSubDevices(
    cl_device_id in_device, const cl_device_partition_property* properties,
    cl_uint num_devices, cl_device_id* out_devices,
    cl_uint* num_devices_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clCreateSubDevices");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateSubDevices(in_device, properties, num_devices,
                                           out_devices, num_devices_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainDevice(cl_device_id device)
    CL_API_SUFFIX__VERSION_1_2 {
  LOG("clRetainDevice");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clRetainDevice(
      device);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseDevice(cl_device_id device)
    CL_API_SUFFIX__VERSION_1_2 {
  LOG("clReleaseDevice");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clReleaseDevice(
      device);
}

CL_API_ENTRY cl_int CL_API_CALL clSetDefaultDeviceCommandQueue(
    cl_context context, cl_device_id device,
    cl_command_queue command_queue) CL_API_SUFFIX__VERSION_2_1 {
  LOG("clSetDefaultDeviceCommandQueue");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetDefaultDeviceCommandQueue(context, device,
                                                       command_queue);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceAndHostTimer(cl_device_id device, cl_ulong* device_timestamp,
                        cl_ulong* host_timestamp) CL_API_SUFFIX__VERSION_2_1 {
  LOG("clGetDeviceAndHostTimer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetDeviceAndHostTimer(device, device_timestamp,
                                                host_timestamp);
}

CL_API_ENTRY cl_int CL_API_CALL clGetHostTimer(
    cl_device_id device, cl_ulong* host_timestamp) CL_API_SUFFIX__VERSION_2_1 {
  LOG("clGetHostTimer");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetHostTimer(
      device, host_timestamp);
}

CL_API_ENTRY cl_context CL_API_CALL clCreateContext(
    const cl_context_properties* properties, cl_uint num_devices,
    const cl_device_id* devices,
    void(CL_CALLBACK* pfn_notify)(const char* errinfo, const void* private_info,
                                  size_t cb, void* user_data),
    void* user_data, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateContext");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreateContext(
      properties, num_devices, devices, pfn_notify, user_data, errcode_ret);
}

CL_API_ENTRY cl_context CL_API_CALL clCreateContextFromType(
    const cl_context_properties* properties, cl_device_type device_type,
    void(CL_CALLBACK* pfn_notify)(const char* errinfo, const void* private_info,
                                  size_t cb, void* user_data),
    void* user_data, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateContextFromType");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateContextFromType(
          properties, device_type, pfn_notify, user_data, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainContext(cl_context context)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clRetainContext");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clRetainContext(
      context);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseContext(cl_context context)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clReleaseContext");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clReleaseContext(
      context);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetContextInfo(cl_context context, cl_context_info param_name,
                 size_t param_value_size, void* param_value,
                 size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetContextInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetContextInfo(
      context, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clSetContextDestructorCallback(
    cl_context context,
    void(CL_CALLBACK* pfn_notify)(cl_context context, void* user_data),
    void* user_data) CL_API_SUFFIX__VERSION_3_0 {
  LOG("clSetContextDestructorCallback");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetContextDestructorCallback(context, pfn_notify,
                                                       user_data);
}

CL_API_ENTRY cl_command_queue CL_API_CALL clCreateCommandQueueWithProperties(
    cl_context context, cl_device_id device,
    const cl_queue_properties* properties,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clCreateCommandQueueWithProperties");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateCommandQueueWithProperties(
          context, device, properties, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainCommandQueue(
    cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clRetainCommandQueue");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clRetainCommandQueue(command_queue);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseCommandQueue(
    cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clReleaseCommandQueue");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clReleaseCommandQueue(command_queue);
}

CL_API_ENTRY cl_int CL_API_CALL clGetCommandQueueInfo(
    cl_command_queue command_queue, cl_command_queue_info param_name,
    size_t param_value_size, void* param_value,
    size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetCommandQueueInfo");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetCommandQueueInfo(command_queue, param_name,
                                              param_value_size, param_value,
                                              param_value_size_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL
clCreateBuffer(cl_context context, cl_mem_flags flags, size_t size,
               void* host_ptr, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateBuffer");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreateBuffer(
      context, flags, size, host_ptr, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateBufferWithProperties(
    cl_context context, const cl_mem_properties* properties, cl_mem_flags flags,
    size_t size, void* host_ptr,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_3_0 {
  LOG("clCreateBufferWithProperties");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateBufferWithProperties(
          context, properties, flags, size, host_ptr, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateSubBuffer(
    cl_mem buffer, cl_mem_flags flags, cl_buffer_create_type buffer_create_type,
    const void* buffer_create_info,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_1 {
  LOG("clCreateSubBuffer");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreateSubBuffer(
      buffer, flags, buffer_create_type, buffer_create_info, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateImage(
    cl_context context, cl_mem_flags flags, const cl_image_format* image_format,
    const cl_image_desc* image_desc, void* host_ptr,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clCreateImage");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreateImage(
      context, flags, image_format, image_desc, host_ptr, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateImageWithProperties(
    cl_context context, const cl_mem_properties* properties, cl_mem_flags flags,
    const cl_image_format* image_format, const cl_image_desc* image_desc,
    void* host_ptr, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_3_0 {
  LOG("clCreateImageWithProperties");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateImageWithProperties(context, properties, flags,
                                                    image_format, image_desc,
                                                    host_ptr, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL
clCreatePipe(cl_context context, cl_mem_flags flags, cl_uint pipe_packet_size,
             cl_uint pipe_max_packets, const cl_pipe_properties* properties,
             cl_int* errcode_ret) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clCreatePipe");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreatePipe(
      context, flags, pipe_packet_size, pipe_max_packets, properties,
      errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainMemObject(cl_mem memobj)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clRetainMemObject");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clRetainMemObject(
      memobj);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseMemObject(cl_mem memobj)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clReleaseMemObject");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clReleaseMemObject(memobj);
}

CL_API_ENTRY cl_int CL_API_CALL clGetSupportedImageFormats(
    cl_context context, cl_mem_flags flags, cl_mem_object_type image_type,
    cl_uint num_entries, cl_image_format* image_formats,
    cl_uint* num_image_formats) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetSupportedImageFormats");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetSupportedImageFormats(context, flags, image_type,
                                                   num_entries, image_formats,
                                                   num_image_formats);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetMemObjectInfo(cl_mem memobj, cl_mem_info param_name,
                   size_t param_value_size, void* param_value,
                   size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetMemObjectInfo");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetMemObjectInfo(memobj, param_name, param_value_size,
                                           param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetImageInfo(cl_mem image, cl_image_info param_name, size_t param_value_size,
               void* param_value,
               size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetImageInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetImageInfo(
      image, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetPipeInfo(cl_mem pipe, cl_pipe_info param_name, size_t param_value_size,
              void* param_value,
              size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clGetPipeInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetPipeInfo(
      pipe, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clSetMemObjectDestructorCallback(
    cl_mem memobj,
    void(CL_CALLBACK* pfn_notify)(cl_mem memobj, void* user_data),
    void* user_data) CL_API_SUFFIX__VERSION_1_1 {
  LOG("clSetMemObjectDestructorCallback");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetMemObjectDestructorCallback(memobj, pfn_notify,
                                                         user_data);
}

CL_API_ENTRY cl_int CL_API_CALL clSetMemObjectDestructorAPPLE(
    cl_mem memobj,
    void(CL_CALLBACK* pfn_notify)(cl_mem memobj, void* user_data),
    void* user_data) CL_EXT_SUFFIX__VERSION_1_0 {
  LOG("clSetMemObjectDestructorAPPLE");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetMemObjectDestructorAPPLE(memobj, pfn_notify,
                                                      user_data);
}

CL_API_ENTRY void* CL_API_CALL
clSVMAlloc(cl_context context, cl_svm_mem_flags flags, size_t size,
           cl_uint alignment) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clSVMAlloc");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clSVMAlloc(
      context, flags, size, alignment);
}

CL_API_ENTRY void CL_API_CALL clSVMFree(cl_context context, void* svm_pointer)
    CL_API_SUFFIX__VERSION_2_0 {
  LOG("clSVMFree");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clSVMFree(
      context, svm_pointer);
}

CL_API_ENTRY cl_sampler CL_API_CALL clCreateSamplerWithProperties(
    cl_context context, const cl_sampler_properties* sampler_properties,
    cl_int* errcode_ret) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clCreateSamplerWithProperties");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateSamplerWithProperties(
          context, sampler_properties, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainSampler(cl_sampler sampler)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clRetainSampler");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clRetainSampler(
      sampler);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseSampler(cl_sampler sampler)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clReleaseSampler");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clReleaseSampler(
      sampler);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetSamplerInfo(cl_sampler sampler, cl_sampler_info param_name,
                 size_t param_value_size, void* param_value,
                 size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetSamplerInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetSamplerInfo(
      sampler, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_program CL_API_CALL clCreateProgramWithSource(
    cl_context context, cl_uint count, const char** strings,
    const size_t* lengths, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateProgramWithSource");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateProgramWithSource(context, count, strings,
                                                  lengths, errcode_ret);
}

CL_API_ENTRY cl_program CL_API_CALL clCreateProgramWithBinary(
    cl_context context, cl_uint num_devices, const cl_device_id* device_list,
    const size_t* lengths, const unsigned char** binaries,
    cl_int* binary_status, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateProgramWithBinary");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateProgramWithBinary(
          context, num_devices, device_list, lengths, binaries, binary_status,
          errcode_ret);
}

CL_API_ENTRY cl_program CL_API_CALL clCreateProgramWithBuiltInKernels(
    cl_context context, cl_uint num_devices, const cl_device_id* device_list,
    const char* kernel_names, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clCreateProgramWithBuiltInKernels");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateProgramWithBuiltInKernels(
          context, num_devices, device_list, kernel_names, errcode_ret);
}

CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithIL(cl_context context, const void* il, size_t length,
                      cl_int* errcode_ret) CL_API_SUFFIX__VERSION_2_1 {
  LOG("clCreateProgramWithIL");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateProgramWithIL(context, il, length, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainProgram(cl_program program)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clRetainProgram");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clRetainProgram(
      program);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseProgram(cl_program program)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clReleaseProgram");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clReleaseProgram(
      program);
}

CL_API_ENTRY cl_int CL_API_CALL clBuildProgram(
    cl_program program, cl_uint num_devices, const cl_device_id* device_list,
    const char* options,
    void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data),
    void* user_data) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clBuildProgram");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clBuildProgram(
      program, num_devices, device_list, options, pfn_notify, user_data);
}

CL_API_ENTRY cl_int CL_API_CALL clCompileProgram(
    cl_program program, cl_uint num_devices, const cl_device_id* device_list,
    const char* options, cl_uint num_input_headers,
    const cl_program* input_headers, const char** header_include_names,
    void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data),
    void* user_data) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clCompileProgram");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCompileProgram(
      program, num_devices, device_list, options, num_input_headers,
      input_headers, header_include_names, pfn_notify, user_data);
}

CL_API_ENTRY cl_program CL_API_CALL clLinkProgram(
    cl_context context, cl_uint num_devices, const cl_device_id* device_list,
    const char* options, cl_uint num_input_programs,
    const cl_program* input_programs,
    void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data),
    void* user_data, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clLinkProgram");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clLinkProgram(
      context, num_devices, device_list, options, num_input_programs,
      input_programs, pfn_notify, user_data, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clSetProgramReleaseCallback(
    cl_program program,
    void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data),
    void* user_data) CL_EXT_SUFFIX__VERSION_2_2_DEPRECATED {
  LOG("clSetProgramReleaseCallback");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetProgramReleaseCallback(program, pfn_notify,
                                                    user_data);
}

CL_API_ENTRY cl_int CL_API_CALL clSetProgramSpecializationConstant(
    cl_program program, cl_uint spec_id, size_t spec_size,
    const void* spec_value) CL_API_SUFFIX__VERSION_2_2 {
  LOG("clSetProgramSpecializationConstant");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetProgramSpecializationConstant(
          program, spec_id, spec_size, spec_value);
}

CL_API_ENTRY cl_int CL_API_CALL
clUnloadPlatformCompiler(cl_platform_id platform) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clUnloadPlatformCompiler");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clUnloadPlatformCompiler(platform);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetProgramInfo(cl_program program, cl_program_info param_name,
                 size_t param_value_size, void* param_value,
                 size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetProgramInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetProgramInfo(
      program, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clGetProgramBuildInfo(
    cl_program program, cl_device_id device, cl_program_build_info param_name,
    size_t param_value_size, void* param_value,
    size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetProgramBuildInfo");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetProgramBuildInfo(program, device, param_name,
                                              param_value_size, param_value,
                                              param_value_size_ret);
}

CL_API_ENTRY cl_kernel CL_API_CALL
clCreateKernel(cl_program program, const char* kernel_name,
               cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateKernel");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreateKernel(
      program, kernel_name, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clCreateKernelsInProgram(
    cl_program program, cl_uint num_kernels, cl_kernel* kernels,
    cl_uint* num_kernels_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clCreateKernelsInProgram");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateKernelsInProgram(program, num_kernels, kernels,
                                                 num_kernels_ret);
}

CL_API_ENTRY cl_kernel CL_API_CALL clCloneKernel(
    cl_kernel source_kernel, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_2_1 {
  LOG("clCloneKernel");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCloneKernel(
      source_kernel, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainKernel(cl_kernel kernel)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clRetainKernel");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clRetainKernel(
      kernel);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseKernel(cl_kernel kernel)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clReleaseKernel");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clReleaseKernel(
      kernel);
}

CL_API_ENTRY cl_int CL_API_CALL
clSetKernelArg(cl_kernel kernel, cl_uint arg_index, size_t arg_size,
               const void* arg_value) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clSetKernelArg");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clSetKernelArg(
      kernel, arg_index, arg_size, arg_value);
}

CL_API_ENTRY cl_int CL_API_CALL
clSetKernelArgSVMPointer(cl_kernel kernel, cl_uint arg_index,
                         const void* arg_value) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clSetKernelArgSVMPointer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetKernelArgSVMPointer(kernel, arg_index, arg_value);
}

CL_API_ENTRY cl_int CL_API_CALL clSetKernelExecInfo(
    cl_kernel kernel, cl_kernel_exec_info param_name, size_t param_value_size,
    const void* param_value) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clSetKernelExecInfo");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetKernelExecInfo(kernel, param_name,
                                            param_value_size, param_value);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetKernelInfo(cl_kernel kernel, cl_kernel_info param_name,
                size_t param_value_size, void* param_value,
                size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetKernelInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetKernelInfo(
      kernel, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clGetKernelArgInfo(
    cl_kernel kernel, cl_uint arg_index, cl_kernel_arg_info param_name,
    size_t param_value_size, void* param_value,
    size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clGetKernelArgInfo");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetKernelArgInfo(kernel, arg_index, param_name,
                                           param_value_size, param_value,
                                           param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clGetKernelWorkGroupInfo(
    cl_kernel kernel, cl_device_id device, cl_kernel_work_group_info param_name,
    size_t param_value_size, void* param_value,
    size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetKernelWorkGroupInfo");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetKernelWorkGroupInfo(kernel, device, param_name,
                                                 param_value_size, param_value,
                                                 param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clGetKernelSubGroupInfo(
    cl_kernel kernel, cl_device_id device, cl_kernel_sub_group_info param_name,
    size_t input_value_size, const void* input_value, size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_2_1 {
  LOG("clGetKernelSubGroupInfo");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetKernelSubGroupInfo(
          kernel, device, param_name, input_value_size, input_value,
          param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clWaitForEvents(
    cl_uint num_events, const cl_event* event_list) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clWaitForEvents");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clWaitForEvents(
      num_events, event_list);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetEventInfo(cl_event event, cl_event_info param_name,
               size_t param_value_size, void* param_value,
               size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetEventInfo");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clGetEventInfo(
      event, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_event CL_API_CALL clCreateUserEvent(
    cl_context context, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_1 {
  LOG("clCreateUserEvent");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreateUserEvent(
      context, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainEvent(cl_event event)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clRetainEvent");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clRetainEvent(
      event);
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseEvent(cl_event event)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clReleaseEvent");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clReleaseEvent(
      event);
}

CL_API_ENTRY cl_int CL_API_CALL clSetUserEventStatus(
    cl_event event, cl_int execution_status) CL_API_SUFFIX__VERSION_1_1 {
  LOG("clSetUserEventStatus");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetUserEventStatus(event, execution_status);
}

CL_API_ENTRY cl_int CL_API_CALL clSetEventCallback(
    cl_event event, cl_int command_exec_callback_type,
    void(CL_CALLBACK* pfn_notify)(cl_event event, cl_int event_command_status,
                                  void* user_data),
    void* user_data) CL_API_SUFFIX__VERSION_1_1 {
  LOG("clSetEventCallback");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetEventCallback(event, command_exec_callback_type,
                                           pfn_notify, user_data);
}

CL_API_ENTRY cl_int CL_API_CALL clGetEventProfilingInfo(
    cl_event event, cl_profiling_info param_name, size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clGetEventProfilingInfo");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetEventProfilingInfo(event, param_name,
                                                param_value_size, param_value,
                                                param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clFlush(cl_command_queue command_queue)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clFlush");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clFlush(
      command_queue);
}

CL_API_ENTRY cl_int CL_API_CALL clFinish(cl_command_queue command_queue)
    CL_API_SUFFIX__VERSION_1_0 {
  LOG("clFinish");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clFinish(
      command_queue);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReadBuffer(
    cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read,
    size_t offset, size_t size, void* ptr, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueReadBuffer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReadBuffer(
          command_queue, buffer, blocking_read, offset, size, ptr,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReadBufferRect(
    cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read,
    const size_t* buffer_origin, const size_t* host_origin,
    const size_t* region, size_t buffer_row_pitch, size_t buffer_slice_pitch,
    size_t host_row_pitch, size_t host_slice_pitch, void* ptr,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_1 {
  LOG("clEnqueueReadBufferRect");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReadBufferRect(
          command_queue, buffer, blocking_read, buffer_origin, host_origin,
          region, buffer_row_pitch, buffer_slice_pitch, host_row_pitch,
          host_slice_pitch, ptr, num_events_in_wait_list, event_wait_list,
          event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueWriteBuffer(
    cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write,
    size_t offset, size_t size, const void* ptr,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueWriteBuffer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueWriteBuffer(
          command_queue, buffer, blocking_write, offset, size, ptr,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueWriteBufferRect(
    cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write,
    const size_t* buffer_origin, const size_t* host_origin,
    const size_t* region, size_t buffer_row_pitch, size_t buffer_slice_pitch,
    size_t host_row_pitch, size_t host_slice_pitch, const void* ptr,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_1 {
  LOG("clEnqueueWriteBufferRect");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueWriteBufferRect(
          command_queue, buffer, blocking_write, buffer_origin, host_origin,
          region, buffer_row_pitch, buffer_slice_pitch, host_row_pitch,
          host_slice_pitch, ptr, num_events_in_wait_list, event_wait_list,
          event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueFillBuffer(
    cl_command_queue command_queue, cl_mem buffer, const void* pattern,
    size_t pattern_size, size_t offset, size_t size,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueFillBuffer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueFillBuffer(
          command_queue, buffer, pattern, pattern_size, offset, size,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyBuffer(
    cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer,
    size_t src_offset, size_t dst_offset, size_t size,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueCopyBuffer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueCopyBuffer(
          command_queue, src_buffer, dst_buffer, src_offset, dst_offset, size,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyBufferRect(
    cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer,
    const size_t* src_origin, const size_t* dst_origin, const size_t* region,
    size_t src_row_pitch, size_t src_slice_pitch, size_t dst_row_pitch,
    size_t dst_slice_pitch, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_1 {
  LOG("clEnqueueCopyBufferRect");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueCopyBufferRect(
          command_queue, src_buffer, dst_buffer, src_origin, dst_origin, region,
          src_row_pitch, src_slice_pitch, dst_row_pitch, dst_slice_pitch,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueReadImage(
    cl_command_queue command_queue, cl_mem image, cl_bool blocking_read,
    const size_t* origin, const size_t* region, size_t row_pitch,
    size_t slice_pitch, void* ptr, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueReadImage");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueReadImage(
          command_queue, image, blocking_read, origin, region, row_pitch,
          slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueWriteImage(
    cl_command_queue command_queue, cl_mem image, cl_bool blocking_write,
    const size_t* origin, const size_t* region, size_t input_row_pitch,
    size_t input_slice_pitch, const void* ptr, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueWriteImage");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueWriteImage(
          command_queue, image, blocking_write, origin, region, input_row_pitch,
          input_slice_pitch, ptr, num_events_in_wait_list, event_wait_list,
          event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueFillImage(
    cl_command_queue command_queue, cl_mem image, const void* fill_color,
    const size_t* origin, const size_t* region, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueFillImage");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueFillImage(
          command_queue, image, fill_color, origin, region,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyImage(
    cl_command_queue command_queue, cl_mem src_image, cl_mem dst_image,
    const size_t* src_origin, const size_t* dst_origin, const size_t* region,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueCopyImage");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueCopyImage(
          command_queue, src_image, dst_image, src_origin, dst_origin, region,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyImageToBuffer(
    cl_command_queue command_queue, cl_mem src_image, cl_mem dst_buffer,
    const size_t* src_origin, const size_t* region, size_t dst_offset,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueCopyImageToBuffer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueCopyImageToBuffer(
          command_queue, src_image, dst_buffer, src_origin, region, dst_offset,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyBufferToImage(
    cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_image,
    size_t src_offset, const size_t* dst_origin, const size_t* region,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueCopyBufferToImage");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueCopyBufferToImage(
          command_queue, src_buffer, dst_image, src_offset, dst_origin, region,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY void* CL_API_CALL clEnqueueMapBuffer(
    cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_map,
    cl_map_flags map_flags, size_t offset, size_t size,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueMapBuffer");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueMapBuffer(
          command_queue, buffer, blocking_map, map_flags, offset, size,
          num_events_in_wait_list, event_wait_list, event, errcode_ret);
}

CL_API_ENTRY void* CL_API_CALL clEnqueueMapImage(
    cl_command_queue command_queue, cl_mem image, cl_bool blocking_map,
    cl_map_flags map_flags, const size_t* origin, const size_t* region,
    size_t* image_row_pitch, size_t* image_slice_pitch,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event, cl_int* errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueMapImage");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clEnqueueMapImage(
      command_queue, image, blocking_map, map_flags, origin, region,
      image_row_pitch, image_slice_pitch, num_events_in_wait_list,
      event_wait_list, event, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueUnmapMemObject(
    cl_command_queue command_queue, cl_mem memobj, void* mapped_ptr,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueUnmapMemObject");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueUnmapMemObject(
          command_queue, memobj, mapped_ptr, num_events_in_wait_list,
          event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueMigrateMemObjects(
    cl_command_queue command_queue, cl_uint num_mem_objects,
    const cl_mem* mem_objects, cl_mem_migration_flags flags,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueMigrateMemObjects");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueMigrateMemObjects(
          command_queue, num_mem_objects, mem_objects, flags,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueNDRangeKernel(
    cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim,
    const size_t* global_work_offset, const size_t* global_work_size,
    const size_t* local_work_size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueNDRangeKernel");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueNDRangeKernel(
          command_queue, kernel, work_dim, global_work_offset, global_work_size,
          local_work_size, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueNativeKernel(
    cl_command_queue command_queue, void(CL_CALLBACK* user_func)(void*),
    void* args, size_t cb_args, cl_uint num_mem_objects, const cl_mem* mem_list,
    const void** args_mem_loc, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
  LOG("clEnqueueNativeKernel");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueNativeKernel(
          command_queue, user_func, args, cb_args, num_mem_objects, mem_list,
          args_mem_loc, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueMarkerWithWaitList(
    cl_command_queue command_queue, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueMarkerWithWaitList");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueMarkerWithWaitList(
          command_queue, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueBarrierWithWaitList(
    cl_command_queue command_queue, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clEnqueueBarrierWithWaitList");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueBarrierWithWaitList(
          command_queue, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMFree(
    cl_command_queue command_queue, cl_uint num_svm_pointers,
    void* svm_pointers[],
    void(CL_CALLBACK* pfn_free_func)(cl_command_queue queue,
                                     cl_uint num_svm_pointers,
                                     void* svm_pointers[], void* user_data),
    void* user_data, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clEnqueueSVMFree");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clEnqueueSVMFree(
      command_queue, num_svm_pointers, svm_pointers, pfn_free_func, user_data,
      num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMemcpy(
    cl_command_queue command_queue, cl_bool blocking_copy, void* dst_ptr,
    const void* src_ptr, size_t size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clEnqueueSVMMemcpy");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueSVMMemcpy(
          command_queue, blocking_copy, dst_ptr, src_ptr, size,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMemFill(
    cl_command_queue command_queue, void* svm_ptr, const void* pattern,
    size_t pattern_size, size_t size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clEnqueueSVMMemFill");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueSVMMemFill(
          command_queue, svm_ptr, pattern, pattern_size, size,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMap(
    cl_command_queue command_queue, cl_bool blocking_map, cl_map_flags flags,
    void* svm_ptr, size_t size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clEnqueueSVMMap");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clEnqueueSVMMap(
      command_queue, blocking_map, flags, svm_ptr, size,
      num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMUnmap(
    cl_command_queue command_queue, void* svm_ptr,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_2_0 {
  LOG("clEnqueueSVMUnmap");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clEnqueueSVMUnmap(
      command_queue, svm_ptr, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMigrateMem(
    cl_command_queue command_queue, cl_uint num_svm_pointers,
    const void** svm_pointers, const size_t* sizes,
    cl_mem_migration_flags flags, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event) CL_API_SUFFIX__VERSION_2_1 {
  LOG("clEnqueueSVMMigrateMem");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueSVMMigrateMem(
          command_queue, num_svm_pointers, svm_pointers, sizes, flags,
          num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY void* CL_API_CALL clGetExtensionFunctionAddressForPlatform(
    cl_platform_id platform, const char* func_name) CL_API_SUFFIX__VERSION_1_2 {
  LOG("clGetExtensionFunctionAddressForPlatform");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetExtensionFunctionAddressForPlatform(platform,
                                                                 func_name);
}

CL_API_ENTRY cl_int CL_API_CALL clSetCommandQueueProperty(
    cl_command_queue command_queue, cl_command_queue_properties properties,
    cl_bool enable, cl_command_queue_properties* old_properties)
    CL_EXT_SUFFIX__VERSION_1_0_DEPRECATED {
  LOG("clSetCommandQueueProperty");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clSetCommandQueueProperty(command_queue, properties,
                                                  enable, old_properties);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateImage2D(
    cl_context context, cl_mem_flags flags, const cl_image_format* image_format,
    size_t image_width, size_t image_height, size_t image_row_pitch,
    void* host_ptr, cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED {
  LOG("clCreateImage2D");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreateImage2D(
      context, flags, image_format, image_width, image_height, image_row_pitch,
      host_ptr, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL clCreateImage3D(
    cl_context context, cl_mem_flags flags, const cl_image_format* image_format,
    size_t image_width, size_t image_height, size_t image_depth,
    size_t image_row_pitch, size_t image_slice_pitch, void* host_ptr,
    cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED {
  LOG("clCreateImage3D");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreateImage3D(
      context, flags, image_format, image_width, image_height, image_depth,
      image_row_pitch, image_slice_pitch, host_ptr, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueMarker(cl_command_queue command_queue,
                                                cl_event* event)
    CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED {
  LOG("clEnqueueMarker");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clEnqueueMarker(
      command_queue, event);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueWaitForEvents(
    cl_command_queue command_queue, cl_uint num_events,
    const cl_event* event_list) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED {
  LOG("clEnqueueWaitForEvents");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clEnqueueWaitForEvents(command_queue, num_events,
                                               event_list);
}

CL_API_ENTRY cl_int CL_API_CALL clEnqueueBarrier(cl_command_queue command_queue)
    CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED {
  LOG("clEnqueueBarrier");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clEnqueueBarrier(
      command_queue);
}

CL_API_ENTRY cl_int CL_API_CALL clUnloadCompiler()
    CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED {
  LOG("clUnloadCompiler");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clUnloadCompiler();
}

CL_API_ENTRY void* CL_API_CALL clGetExtensionFunctionAddress(
    const char* func_name) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED {
  LOG("clGetExtensionFunctionAddress");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clGetExtensionFunctionAddress(func_name);
}

CL_API_ENTRY cl_command_queue CL_API_CALL clCreateCommandQueue(
    cl_context context, cl_device_id device,
    cl_command_queue_properties properties,
    cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED {
  LOG("clCreateCommandQueue");
  gf_layers::opencl_capture::GetGlobalData()
      ->opencl_pointers.clCreateCommandQueue(context, device, properties,
                                             errcode_ret);
}

CL_API_ENTRY cl_sampler CL_API_CALL
clCreateSampler(cl_context context, cl_bool normalized_coords,
                cl_addressing_mode addressing_mode, cl_filter_mode filter_mode,
                cl_int* errcode_ret) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED {
  LOG("clCreateSampler");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clCreateSampler(
      context, normalized_coords, addressing_mode, filter_mode, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueTask(cl_command_queue command_queue, cl_kernel kernel,
              cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
              cl_event* event) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED {
  LOG("clEnqueueTask");
  gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.clEnqueueTask(
      command_queue, kernel, num_events_in_wait_list, event_wait_list, event);
}

#pragma clang diagnostic pop
}
