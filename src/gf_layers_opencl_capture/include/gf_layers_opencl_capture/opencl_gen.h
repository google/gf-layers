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

#ifndef GF_LAYERS_OPENCL_CAPTURE_OPENCL_GEN_H
#define GF_LAYERS_OPENCL_CAPTURE_OPENCL_GEN_H

#include <CL/cl_egl.h>
#include <CL/cl_ext_intel.h>
#include <CL/cl_va_api_media_sharing_intel.h>
#include <CL/opencl.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

namespace gf_layers::opencl_capture {

using PFN_clCreateEventFromEGLSyncKHR =
    std::add_pointer<CL_API_ENTRY cl_event CL_API_CALL(
        cl_context /*context*/, CLeglSyncKHR /*sync*/,
        CLeglDisplayKHR /*display*/, cl_int* /*errcode_ret*/)
                         CL_API_SUFFIX__VERSION_1_0>::type;

typedef cl_mem(CL_API_CALL* PFN_clCreateFromEGLImageKHR)(
    cl_context /*context*/, CLeglDisplayKHR /*egldisplay*/,
    CLeglImageKHR /*eglimage*/, cl_mem_flags /*flags*/,
    const cl_egl_image_properties_khr* /*properties*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueAcquireEGLObjectsKHR)(
    cl_command_queue /*command_queue*/, cl_uint /*num_objects*/,
    const cl_mem* /*mem_objects*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueReleaseEGLObjectsKHR)(
    cl_command_queue /*command_queue*/, cl_uint /*num_objects*/,
    const cl_mem* /*mem_objects*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef void(CL_API_CALL* PFN_clLogMessagesToSystemLogAPPLE)(
    const char* /*errstr*/, const void* /*private_info*/, size_t /*cb*/,
    void* /*user_data*/) CL_EXT_SUFFIX__VERSION_1_0;
typedef void(CL_API_CALL* PFN_clLogMessagesToStdoutAPPLE)(
    const char* /*errstr*/, const void* /*private_info*/, size_t /*cb*/,
    void* /*user_data*/) CL_EXT_SUFFIX__VERSION_1_0;
typedef void(CL_API_CALL* PFN_clLogMessagesToStderrAPPLE)(
    const char* /*errstr*/, const void* /*private_info*/, size_t /*cb*/,
    void* /*user_data*/) CL_EXT_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clIcdGetPlatformIDsKHR)(
    cl_uint /*num_entries*/, cl_platform_id* /*platforms*/,
    cl_uint* /*num_platforms*/);
typedef cl_program(CL_API_CALL* PFN_clCreateProgramWithILKHR)(
    cl_context /*context*/, const void* /*il*/, size_t /*length*/,
    cl_int* /*errcode_ret*/);
typedef cl_int(CL_API_CALL* PFN_clTerminateContextKHR)(cl_context /*context*/)
    CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_command_queue(
    CL_API_CALL* PFN_clCreateCommandQueueWithPropertiesKHR)(
    cl_context /*context*/, cl_device_id /*device*/,
    const cl_queue_properties_khr* /*properties*/,
    cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clReleaseDeviceEXT)(cl_device_id /*device*/)
    CL_EXT_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clRetainDeviceEXT)(cl_device_id /*device*/)
    CL_EXT_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clCreateSubDevicesEXT)(
    cl_device_id /*in_device*/,
    const cl_device_partition_property_ext* /*properties*/,
    cl_uint /*num_entries*/, cl_device_id* /*out_devices*/,
    cl_uint* /*num_devices*/) CL_EXT_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clEnqueueMigrateMemObjectEXT)(
    cl_command_queue /*command_queue*/, cl_uint /*num_mem_objects*/,
    const cl_mem* /*mem_objects*/, cl_mem_migration_flags_ext /*flags*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/);
typedef cl_int(CL_API_CALL* PFN_clGetDeviceImageInfoQCOM)(
    cl_device_id /*device*/, size_t /*image_width*/, size_t /*image_height*/,
    const cl_image_format* /*image_format*/,
    cl_image_pitch_info_qcom /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/, size_t* /*param_value_size_ret*/);
typedef cl_int(CL_API_CALL* PFN_clEnqueueAcquireGrallocObjectsIMG)(
    cl_command_queue /*command_queue*/, cl_uint /*num_objects*/,
    const cl_mem* /*mem_objects*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueGenerateMipmapIMG)(
    cl_command_queue /*command_queue*/, cl_mem /*src_image*/,
    cl_mem /*dst_image*/, cl_mipmap_filter_mode_img /*mipmap_filter_mode*/,
    const size_t* /*array_region*/, const size_t* /*mip_region*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueReleaseGrallocObjectsIMG)(
    cl_command_queue /*command_queue*/, cl_uint /*num_objects*/,
    const cl_mem* /*mem_objects*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clGetKernelSubGroupInfoKHR)(
    cl_kernel /*in_kernel*/, cl_device_id /*in_device*/,
    cl_kernel_sub_group_info /*param_name*/, size_t /*input_value_size*/,
    const void* /*input_value*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_EXT_SUFFIX__VERSION_2_0_DEPRECATED;
typedef cl_mem(CL_API_CALL* PFN_clImportMemoryARM)(
    cl_context /*context*/, cl_mem_flags /*flags*/,
    const cl_import_properties_arm* /*properties*/, void* /*memory*/,
    size_t /*size*/, cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_0;
typedef void*(CL_API_CALL* PFN_clSVMAllocARM)(
    cl_context /*context*/, cl_svm_mem_flags_arm /*flags*/, size_t /*size*/,
    cl_uint /*alignment*/)CL_EXT_SUFFIX__VERSION_1_2;
typedef void(CL_API_CALL* PFN_clSVMFreeARM)(
    cl_context /*context*/, void* /*svm_pointer*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMFreeARM)(
    cl_command_queue /*command_queue*/, cl_uint /*num_svm_pointers*/,
    void*[] /*svm_pointers[]*/,
    void(CL_CALLBACK* pfn_free_func)(cl_command_queue queue,
                                     cl_uint num_svm_pointers,
                                     void* svm_pointers[], void* user_data),
    void* /*user_data*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMMemcpyARM)(
    cl_command_queue /*command_queue*/, cl_bool /*blocking_copy*/,
    void* /*dst_ptr*/, const void* /*src_ptr*/, size_t /*size*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMMemFillARM)(
    cl_command_queue /*command_queue*/, void* /*svm_ptr*/,
    const void* /*pattern*/, size_t /*pattern_size*/, size_t /*size*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMMapARM)(
    cl_command_queue /*command_queue*/, cl_bool /*blocking_map*/,
    cl_map_flags /*flags*/, void* /*svm_ptr*/, size_t /*size*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMUnmapARM)(
    cl_command_queue /*command_queue*/, void* /*svm_ptr*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clSetKernelArgSVMPointerARM)(
    cl_kernel /*kernel*/, cl_uint /*arg_index*/,
    const void* /*arg_value*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clSetKernelExecInfoARM)(
    cl_kernel /*kernel*/, cl_kernel_exec_info_arm /*param_name*/,
    size_t /*param_value_size*/,
    const void* /*param_value*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_accelerator_intel(CL_API_CALL* PFN_clCreateAcceleratorINTEL)(
    cl_context /*context*/, cl_accelerator_type_intel /*accelerator_type*/,
    size_t /*descriptor_size*/, const void* /*descriptor*/,
    cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clGetAcceleratorInfoINTEL)(
    cl_accelerator_intel /*accelerator*/,
    cl_accelerator_info_intel /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clRetainAcceleratorINTEL)(
    cl_accelerator_intel /*accelerator*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clReleaseAcceleratorINTEL)(
    cl_accelerator_intel /*accelerator*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_event(CL_API_CALL* PFN_clCreateEventFromGLsyncKHR)(
    cl_context /*context*/, cl_GLsync /*sync*/,
    cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clGetGLContextInfoKHR)(
    const cl_context_properties* /*properties*/,
    cl_gl_context_info /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_mem(CL_API_CALL* PFN_clCreateFromGLBuffer)(
    cl_context /*context*/, cl_mem_flags /*flags*/, cl_GLuint /*bufobj*/,
    int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_mem(CL_API_CALL* PFN_clCreateFromGLTexture)(
    cl_context /*context*/, cl_mem_flags /*flags*/, cl_GLenum /*target*/,
    cl_GLint /*miplevel*/, cl_GLuint /*texture*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_mem(CL_API_CALL* PFN_clCreateFromGLRenderbuffer)(
    cl_context /*context*/, cl_mem_flags /*flags*/, cl_GLuint /*renderbuffer*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetGLObjectInfo)(
    cl_mem /*memobj*/, cl_gl_object_type* /*gl_object_type*/,
    cl_GLuint* /*gl_object_name*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetGLTextureInfo)(
    cl_mem /*memobj*/, cl_gl_texture_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueAcquireGLObjects)(
    cl_command_queue /*command_queue*/, cl_uint /*num_objects*/,
    const cl_mem* /*mem_objects*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueReleaseGLObjects)(
    cl_command_queue /*command_queue*/, cl_uint /*num_objects*/,
    const cl_mem* /*mem_objects*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_mem(CL_API_CALL* PFN_clCreateFromGLTexture2D)(
    cl_context /*context*/, cl_mem_flags /*flags*/, cl_GLenum /*target*/,
    cl_GLint /*miplevel*/, cl_GLuint /*texture*/,
    cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
typedef cl_mem(CL_API_CALL* PFN_clCreateFromGLTexture3D)(
    cl_context /*context*/, cl_mem_flags /*flags*/, cl_GLenum /*target*/,
    cl_GLint /*miplevel*/, cl_GLuint /*texture*/,
    cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
typedef cl_int(CL_API_CALL* PFN_clGetDeviceIDsFromVA_APIMediaAdapterINTEL)(
    cl_platform_id /*platform*/,
    cl_va_api_device_source_intel /*media_adapter_type*/,
    void* /*media_adapter*/, cl_va_api_device_set_intel /*media_adapter_set*/,
    cl_uint /*num_entries*/, cl_device_id* /*devices*/,
    cl_uint* /*num_devices*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_mem(CL_API_CALL* PFN_clCreateFromVA_APIMediaSurfaceINTEL)(
    cl_context /*context*/, cl_mem_flags /*flags*/, VASurfaceID* /*surface*/,
    cl_uint /*plane*/, cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueAcquireVA_APIMediaSurfacesINTEL)(
    cl_command_queue /*command_queue*/, cl_uint /*num_objects*/,
    const cl_mem* /*mem_objects*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueReleaseVA_APIMediaSurfacesINTEL)(
    cl_command_queue /*command_queue*/, cl_uint /*num_objects*/,
    const cl_mem* /*mem_objects*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2;
typedef void*(CL_API_CALL* PFN_clHostMemAllocINTEL)(
    cl_context /*context*/, const cl_mem_properties_intel* /*properties*/,
    size_t /*size*/, cl_uint /*alignment*/, cl_int* /*errcode_ret*/);
typedef void*(CL_API_CALL* PFN_clDeviceMemAllocINTEL)(
    cl_context /*context*/, cl_device_id /*device*/,
    const cl_mem_properties_intel* /*properties*/, size_t /*size*/,
    cl_uint /*alignment*/, cl_int* /*errcode_ret*/);
typedef void*(CL_API_CALL* PFN_clSharedMemAllocINTEL)(
    cl_context /*context*/, cl_device_id /*device*/,
    const cl_mem_properties_intel* /*properties*/, size_t /*size*/,
    cl_uint /*alignment*/, cl_int* /*errcode_ret*/);
typedef cl_int(CL_API_CALL* PFN_clMemFreeINTEL)(cl_context /*context*/,
                                                void* /*ptr*/);
typedef cl_int(CL_API_CALL* PFN_clGetMemAllocInfoINTEL)(
    cl_context /*context*/, const void* /*ptr*/,
    cl_mem_info_intel /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/, size_t* /*param_value_size_ret*/);
typedef cl_int(CL_API_CALL* PFN_clSetKernelArgMemPointerINTEL)(
    cl_kernel /*kernel*/, cl_uint /*arg_index*/, const void* /*arg_value*/);
typedef cl_int(CL_API_CALL* PFN_clEnqueueMemsetINTEL)(
    cl_command_queue /*command_queue*/, void* /*dst_ptr*/, cl_int /*value*/,
    size_t /*size*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/, cl_event* /*event*/);
typedef cl_int(CL_API_CALL* PFN_clEnqueueMemFillINTEL)(
    cl_command_queue /*command_queue*/, void* /*dst_ptr*/,
    const void* /*pattern*/, size_t /*pattern_size*/, size_t /*size*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/);
typedef cl_int(CL_API_CALL* PFN_clEnqueueMemcpyINTEL)(
    cl_command_queue /*command_queue*/, cl_bool /*blocking*/, void* /*dst_ptr*/,
    const void* /*src_ptr*/, size_t /*size*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/);
typedef cl_int(CL_API_CALL* PFN_clEnqueueMigrateMemINTEL)(
    cl_command_queue /*command_queue*/, const void* /*ptr*/, size_t /*size*/,
    cl_mem_migration_flags /*flags*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/, cl_event* /*event*/);
typedef cl_int(CL_API_CALL* PFN_clEnqueueMemAdviseINTEL)(
    cl_command_queue /*command_queue*/, const void* /*ptr*/, size_t /*size*/,
    cl_mem_advice_intel /*advice*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/, cl_event* /*event*/);
typedef cl_mem(CL_API_CALL* PFN_clCreateBufferWithPropertiesINTEL)(
    cl_context /*context*/, const cl_mem_properties_intel* /*properties*/,
    cl_mem_flags /*flags*/, size_t /*size*/, void* /*host_ptr*/,
    cl_int* /*errcode_ret*/);
typedef cl_int(CL_API_CALL* PFN_clGetPlatformIDs)(
    cl_uint /*num_entries*/, cl_platform_id* /*platforms*/,
    cl_uint* /*num_platforms*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetPlatformInfo)(
    cl_platform_id /*platform*/, cl_platform_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetDeviceIDs)(
    cl_platform_id /*platform*/, cl_device_type /*device_type*/,
    cl_uint /*num_entries*/, cl_device_id* /*devices*/,
    cl_uint* /*num_devices*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetDeviceInfo)(
    cl_device_id /*device*/, cl_device_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clCreateSubDevices)(
    cl_device_id /*in_device*/,
    const cl_device_partition_property* /*properties*/, cl_uint /*num_devices*/,
    cl_device_id* /*out_devices*/,
    cl_uint* /*num_devices_ret*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clRetainDevice)(cl_device_id /*device*/)
    CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clReleaseDevice)(cl_device_id /*device*/)
    CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clSetDefaultDeviceCommandQueue)(
    cl_context /*context*/, cl_device_id /*device*/,
    cl_command_queue /*command_queue*/) CL_API_SUFFIX__VERSION_2_1;
typedef cl_int(CL_API_CALL* PFN_clGetDeviceAndHostTimer)(
    cl_device_id /*device*/, cl_ulong* /*device_timestamp*/,
    cl_ulong* /*host_timestamp*/) CL_API_SUFFIX__VERSION_2_1;
typedef cl_int(CL_API_CALL* PFN_clGetHostTimer)(cl_device_id /*device*/,
                                                cl_ulong* /*host_timestamp*/)
    CL_API_SUFFIX__VERSION_2_1;
typedef cl_context(CL_API_CALL* PFN_clCreateContext)(
    const cl_context_properties* /*properties*/, cl_uint /*num_devices*/,
    const cl_device_id* /*devices*/,
    void(CL_CALLBACK* pfn_notify)(const char* errinfo, const void* private_info,
                                  size_t cb, void* user_data),
    void* /*user_data*/, cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_context(CL_API_CALL* PFN_clCreateContextFromType)(
    const cl_context_properties* /*properties*/, cl_device_type /*device_type*/,
    void(CL_CALLBACK* pfn_notify)(const char* errinfo, const void* private_info,
                                  size_t cb, void* user_data),
    void* /*user_data*/, cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clRetainContext)(cl_context /*context*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clReleaseContext)(cl_context /*context*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetContextInfo)(
    cl_context /*context*/, cl_context_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clSetContextDestructorCallback)(
    cl_context /*context*/,
    void(CL_CALLBACK* pfn_notify)(cl_context context, void* user_data),
    void* /*user_data*/) CL_API_SUFFIX__VERSION_3_0;
typedef cl_command_queue(CL_API_CALL* PFN_clCreateCommandQueueWithProperties)(
    cl_context /*context*/, cl_device_id /*device*/,
    const cl_queue_properties* /*properties*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clRetainCommandQueue)(
    cl_command_queue /*command_queue*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clReleaseCommandQueue)(
    cl_command_queue /*command_queue*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetCommandQueueInfo)(
    cl_command_queue /*command_queue*/, cl_command_queue_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_mem(CL_API_CALL* PFN_clCreateBuffer)(
    cl_context /*context*/, cl_mem_flags /*flags*/, size_t /*size*/,
    void* /*host_ptr*/, cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_mem(CL_API_CALL* PFN_clCreateBufferWithProperties)(
    cl_context /*context*/, const cl_mem_properties* /*properties*/,
    cl_mem_flags /*flags*/, size_t /*size*/, void* /*host_ptr*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_3_0;
typedef cl_mem(CL_API_CALL* PFN_clCreateSubBuffer)(
    cl_mem /*buffer*/, cl_mem_flags /*flags*/,
    cl_buffer_create_type /*buffer_create_type*/,
    const void* /*buffer_create_info*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_1;
typedef cl_mem(CL_API_CALL* PFN_clCreateImage)(
    cl_context /*context*/, cl_mem_flags /*flags*/,
    const cl_image_format* /*image_format*/,
    const cl_image_desc* /*image_desc*/, void* /*host_ptr*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_mem(CL_API_CALL* PFN_clCreateImageWithProperties)(
    cl_context /*context*/, const cl_mem_properties* /*properties*/,
    cl_mem_flags /*flags*/, const cl_image_format* /*image_format*/,
    const cl_image_desc* /*image_desc*/, void* /*host_ptr*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_3_0;
typedef cl_mem(CL_API_CALL* PFN_clCreatePipe)(
    cl_context /*context*/, cl_mem_flags /*flags*/,
    cl_uint /*pipe_packet_size*/, cl_uint /*pipe_max_packets*/,
    const cl_pipe_properties* /*properties*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clRetainMemObject)(cl_mem /*memobj*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clReleaseMemObject)(cl_mem /*memobj*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetSupportedImageFormats)(
    cl_context /*context*/, cl_mem_flags /*flags*/,
    cl_mem_object_type /*image_type*/, cl_uint /*num_entries*/,
    cl_image_format* /*image_formats*/,
    cl_uint* /*num_image_formats*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetMemObjectInfo)(
    cl_mem /*memobj*/, cl_mem_info /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetImageInfo)(
    cl_mem /*image*/, cl_image_info /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetPipeInfo)(
    cl_mem /*pipe*/, cl_pipe_info /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clSetMemObjectDestructorCallback)(
    cl_mem /*memobj*/,
    void(CL_CALLBACK* pfn_notify)(cl_mem memobj, void* user_data),
    void* /*user_data*/) CL_API_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clSetMemObjectDestructorAPPLE)(
    cl_mem /*memobj*/,
    void(CL_CALLBACK* pfn_notify)(cl_mem memobj, void* user_data),
    void* /*user_data*/) CL_EXT_SUFFIX__VERSION_1_0;
typedef void*(CL_API_CALL* PFN_clSVMAlloc)(
    cl_context /*context*/, cl_svm_mem_flags /*flags*/, size_t /*size*/,
    cl_uint /*alignment*/)CL_API_SUFFIX__VERSION_2_0;
typedef void(CL_API_CALL* PFN_clSVMFree)(
    cl_context /*context*/, void* /*svm_pointer*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_sampler(CL_API_CALL* PFN_clCreateSamplerWithProperties)(
    cl_context /*context*/, const cl_sampler_properties* /*sampler_properties*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clRetainSampler)(cl_sampler /*sampler*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clReleaseSampler)(cl_sampler /*sampler*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetSamplerInfo)(
    cl_sampler /*sampler*/, cl_sampler_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_program(CL_API_CALL* PFN_clCreateProgramWithSource)(
    cl_context /*context*/, cl_uint /*count*/, const char** /*strings*/,
    const size_t* /*lengths*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_program(CL_API_CALL* PFN_clCreateProgramWithBinary)(
    cl_context /*context*/, cl_uint /*num_devices*/,
    const cl_device_id* /*device_list*/, const size_t* /*lengths*/,
    const unsigned char** /*binaries*/, cl_int* /*binary_status*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_program(CL_API_CALL* PFN_clCreateProgramWithBuiltInKernels)(
    cl_context /*context*/, cl_uint /*num_devices*/,
    const cl_device_id* /*device_list*/, const char* /*kernel_names*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_program(CL_API_CALL* PFN_clCreateProgramWithIL)(
    cl_context /*context*/, const void* /*il*/, size_t /*length*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_2_1;
typedef cl_int(CL_API_CALL* PFN_clRetainProgram)(cl_program /*program*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clReleaseProgram)(cl_program /*program*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clBuildProgram)(
    cl_program /*program*/, cl_uint /*num_devices*/,
    const cl_device_id* /*device_list*/, const char* /*options*/,
    void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data),
    void* /*user_data*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clCompileProgram)(
    cl_program /*program*/, cl_uint /*num_devices*/,
    const cl_device_id* /*device_list*/, const char* /*options*/,
    cl_uint /*num_input_headers*/, const cl_program* /*input_headers*/,
    const char** /*header_include_names*/,
    void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data),
    void* /*user_data*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_program(CL_API_CALL* PFN_clLinkProgram)(
    cl_context /*context*/, cl_uint /*num_devices*/,
    const cl_device_id* /*device_list*/, const char* /*options*/,
    cl_uint /*num_input_programs*/, const cl_program* /*input_programs*/,
    void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data),
    void* /*user_data*/, cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clSetProgramReleaseCallback)(
    cl_program /*program*/,
    void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data),
    void* /*user_data*/) CL_EXT_SUFFIX__VERSION_2_2_DEPRECATED;
typedef cl_int(CL_API_CALL* PFN_clSetProgramSpecializationConstant)(
    cl_program /*program*/, cl_uint /*spec_id*/, size_t /*spec_size*/,
    const void* /*spec_value*/) CL_API_SUFFIX__VERSION_2_2;
typedef cl_int(CL_API_CALL* PFN_clUnloadPlatformCompiler)(
    cl_platform_id /*platform*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clGetProgramInfo)(
    cl_program /*program*/, cl_program_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetProgramBuildInfo)(
    cl_program /*program*/, cl_device_id /*device*/,
    cl_program_build_info /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_kernel(CL_API_CALL* PFN_clCreateKernel)(
    cl_program /*program*/, const char* /*kernel_name*/,
    cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clCreateKernelsInProgram)(
    cl_program /*program*/, cl_uint /*num_kernels*/, cl_kernel* /*kernels*/,
    cl_uint* /*num_kernels_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_kernel(CL_API_CALL* PFN_clCloneKernel)(cl_kernel /*source_kernel*/,
                                                  cl_int* /*errcode_ret*/)
    CL_API_SUFFIX__VERSION_2_1;
typedef cl_int(CL_API_CALL* PFN_clRetainKernel)(cl_kernel /*kernel*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clReleaseKernel)(cl_kernel /*kernel*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clSetKernelArg)(
    cl_kernel /*kernel*/, cl_uint /*arg_index*/, size_t /*arg_size*/,
    const void* /*arg_value*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clSetKernelArgSVMPointer)(
    cl_kernel /*kernel*/, cl_uint /*arg_index*/,
    const void* /*arg_value*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clSetKernelExecInfo)(
    cl_kernel /*kernel*/, cl_kernel_exec_info /*param_name*/,
    size_t /*param_value_size*/,
    const void* /*param_value*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clGetKernelInfo)(
    cl_kernel /*kernel*/, cl_kernel_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetKernelArgInfo)(
    cl_kernel /*kernel*/, cl_uint /*arg_index*/,
    cl_kernel_arg_info /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clGetKernelWorkGroupInfo)(
    cl_kernel /*kernel*/, cl_device_id /*device*/,
    cl_kernel_work_group_info /*param_name*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetKernelSubGroupInfo)(
    cl_kernel /*kernel*/, cl_device_id /*device*/,
    cl_kernel_sub_group_info /*param_name*/, size_t /*input_value_size*/,
    const void* /*input_value*/, size_t /*param_value_size*/,
    void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_2_1;
typedef cl_int(CL_API_CALL* PFN_clWaitForEvents)(cl_uint /*num_events*/,
                                                 const cl_event* /*event_list*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clGetEventInfo)(
    cl_event /*event*/, cl_event_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_event(CL_API_CALL* PFN_clCreateUserEvent)(
    cl_context /*context*/, cl_int* /*errcode_ret*/) CL_API_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clRetainEvent)(cl_event /*event*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clReleaseEvent)(cl_event /*event*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clSetUserEventStatus)(
    cl_event /*event*/, cl_int /*execution_status*/) CL_API_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clSetEventCallback)(
    cl_event /*event*/, cl_int /*command_exec_callback_type*/,
    void(CL_CALLBACK* pfn_notify)(cl_event event, cl_int event_command_status,
                                  void* user_data),
    void* /*user_data*/) CL_API_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clGetEventProfilingInfo)(
    cl_event /*event*/, cl_profiling_info /*param_name*/,
    size_t /*param_value_size*/, void* /*param_value*/,
    size_t* /*param_value_size_ret*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clFlush)(cl_command_queue /*command_queue*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clFinish)(cl_command_queue /*command_queue*/)
    CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueReadBuffer)(
    cl_command_queue /*command_queue*/, cl_mem /*buffer*/,
    cl_bool /*blocking_read*/, size_t /*offset*/, size_t /*size*/,
    void* /*ptr*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueReadBufferRect)(
    cl_command_queue /*command_queue*/, cl_mem /*buffer*/,
    cl_bool /*blocking_read*/, const size_t* /*buffer_origin*/,
    const size_t* /*host_origin*/, const size_t* /*region*/,
    size_t /*buffer_row_pitch*/, size_t /*buffer_slice_pitch*/,
    size_t /*host_row_pitch*/, size_t /*host_slice_pitch*/, void* /*ptr*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clEnqueueWriteBuffer)(
    cl_command_queue /*command_queue*/, cl_mem /*buffer*/,
    cl_bool /*blocking_write*/, size_t /*offset*/, size_t /*size*/,
    const void* /*ptr*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueWriteBufferRect)(
    cl_command_queue /*command_queue*/, cl_mem /*buffer*/,
    cl_bool /*blocking_write*/, const size_t* /*buffer_origin*/,
    const size_t* /*host_origin*/, const size_t* /*region*/,
    size_t /*buffer_row_pitch*/, size_t /*buffer_slice_pitch*/,
    size_t /*host_row_pitch*/, size_t /*host_slice_pitch*/, const void* /*ptr*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clEnqueueFillBuffer)(
    cl_command_queue /*command_queue*/, cl_mem /*buffer*/,
    const void* /*pattern*/, size_t /*pattern_size*/, size_t /*offset*/,
    size_t /*size*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueCopyBuffer)(
    cl_command_queue /*command_queue*/, cl_mem /*src_buffer*/,
    cl_mem /*dst_buffer*/, size_t /*src_offset*/, size_t /*dst_offset*/,
    size_t /*size*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueCopyBufferRect)(
    cl_command_queue /*command_queue*/, cl_mem /*src_buffer*/,
    cl_mem /*dst_buffer*/, const size_t* /*src_origin*/,
    const size_t* /*dst_origin*/, const size_t* /*region*/,
    size_t /*src_row_pitch*/, size_t /*src_slice_pitch*/,
    size_t /*dst_row_pitch*/, size_t /*dst_slice_pitch*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_1;
typedef cl_int(CL_API_CALL* PFN_clEnqueueReadImage)(
    cl_command_queue /*command_queue*/, cl_mem /*image*/,
    cl_bool /*blocking_read*/, const size_t* /*origin*/,
    const size_t* /*region*/, size_t /*row_pitch*/, size_t /*slice_pitch*/,
    void* /*ptr*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueWriteImage)(
    cl_command_queue /*command_queue*/, cl_mem /*image*/,
    cl_bool /*blocking_write*/, const size_t* /*origin*/,
    const size_t* /*region*/, size_t /*input_row_pitch*/,
    size_t /*input_slice_pitch*/, const void* /*ptr*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueFillImage)(
    cl_command_queue /*command_queue*/, cl_mem /*image*/,
    const void* /*fill_color*/, const size_t* /*origin*/,
    const size_t* /*region*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueCopyImage)(
    cl_command_queue /*command_queue*/, cl_mem /*src_image*/,
    cl_mem /*dst_image*/, const size_t* /*src_origin*/,
    const size_t* /*dst_origin*/, const size_t* /*region*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueCopyImageToBuffer)(
    cl_command_queue /*command_queue*/, cl_mem /*src_image*/,
    cl_mem /*dst_buffer*/, const size_t* /*src_origin*/,
    const size_t* /*region*/, size_t /*dst_offset*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueCopyBufferToImage)(
    cl_command_queue /*command_queue*/, cl_mem /*src_buffer*/,
    cl_mem /*dst_image*/, size_t /*src_offset*/, const size_t* /*dst_origin*/,
    const size_t* /*region*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef void*(CL_API_CALL* PFN_clEnqueueMapBuffer)(
    cl_command_queue /*command_queue*/, cl_mem /*buffer*/,
    cl_bool /*blocking_map*/, cl_map_flags /*map_flags*/, size_t /*offset*/,
    size_t /*size*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/, cl_event* /*event*/,
    cl_int* /*errcode_ret*/)CL_API_SUFFIX__VERSION_1_0;
typedef void*(CL_API_CALL* PFN_clEnqueueMapImage)(
    cl_command_queue /*command_queue*/, cl_mem /*image*/,
    cl_bool /*blocking_map*/, cl_map_flags /*map_flags*/,
    const size_t* /*origin*/, const size_t* /*region*/,
    size_t* /*image_row_pitch*/, size_t* /*image_slice_pitch*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/, cl_int* /*errcode_ret*/)CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueUnmapMemObject)(
    cl_command_queue /*command_queue*/, cl_mem /*memobj*/, void* /*mapped_ptr*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueMigrateMemObjects)(
    cl_command_queue /*command_queue*/, cl_uint /*num_mem_objects*/,
    const cl_mem* /*mem_objects*/, cl_mem_migration_flags /*flags*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueNDRangeKernel)(
    cl_command_queue /*command_queue*/, cl_kernel /*kernel*/,
    cl_uint /*work_dim*/, const size_t* /*global_work_offset*/,
    const size_t* /*global_work_size*/, const size_t* /*local_work_size*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueNativeKernel)(
    cl_command_queue /*command_queue*/, void(CL_CALLBACK* user_func)(void*),
    void* /*args*/, size_t /*cb_args*/, cl_uint /*num_mem_objects*/,
    const cl_mem* /*mem_list*/, const void** /*args_mem_loc*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueMarkerWithWaitList)(
    cl_command_queue /*command_queue*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueBarrierWithWaitList)(
    cl_command_queue /*command_queue*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMFree)(
    cl_command_queue /*command_queue*/, cl_uint /*num_svm_pointers*/,
    void*[] /*svm_pointers[]*/,
    void(CL_CALLBACK* pfn_free_func)(cl_command_queue queue,
                                     cl_uint num_svm_pointers,
                                     void* svm_pointers[], void* user_data),
    void* /*user_data*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMMemcpy)(
    cl_command_queue /*command_queue*/, cl_bool /*blocking_copy*/,
    void* /*dst_ptr*/, const void* /*src_ptr*/, size_t /*size*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMMemFill)(
    cl_command_queue /*command_queue*/, void* /*svm_ptr*/,
    const void* /*pattern*/, size_t /*pattern_size*/, size_t /*size*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMMap)(
    cl_command_queue /*command_queue*/, cl_bool /*blocking_map*/,
    cl_map_flags /*flags*/, void* /*svm_ptr*/, size_t /*size*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMUnmap)(
    cl_command_queue /*command_queue*/, void* /*svm_ptr*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_2_0;
typedef cl_int(CL_API_CALL* PFN_clEnqueueSVMMigrateMem)(
    cl_command_queue /*command_queue*/, cl_uint /*num_svm_pointers*/,
    const void** /*svm_pointers*/, const size_t* /*sizes*/,
    cl_mem_migration_flags /*flags*/, cl_uint /*num_events_in_wait_list*/,
    const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_API_SUFFIX__VERSION_2_1;
typedef void*(CL_API_CALL* PFN_clGetExtensionFunctionAddressForPlatform)(
    cl_platform_id /*platform*/,
    const char* /*func_name*/)CL_API_SUFFIX__VERSION_1_2;
typedef cl_int(CL_API_CALL* PFN_clSetCommandQueueProperty)(
    cl_command_queue /*command_queue*/,
    cl_command_queue_properties /*properties*/, cl_bool /*enable*/,
    cl_command_queue_properties* /*old_properties*/)
    CL_EXT_SUFFIX__VERSION_1_0_DEPRECATED;
typedef cl_mem(CL_API_CALL* PFN_clCreateImage2D)(
    cl_context /*context*/, cl_mem_flags /*flags*/,
    const cl_image_format* /*image_format*/, size_t /*image_width*/,
    size_t /*image_height*/, size_t /*image_row_pitch*/, void* /*host_ptr*/,
    cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
typedef cl_mem(CL_API_CALL* PFN_clCreateImage3D)(
    cl_context /*context*/, cl_mem_flags /*flags*/,
    const cl_image_format* /*image_format*/, size_t /*image_width*/,
    size_t /*image_height*/, size_t /*image_depth*/, size_t /*image_row_pitch*/,
    size_t /*image_slice_pitch*/, void* /*host_ptr*/,
    cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
typedef cl_int(CL_API_CALL* PFN_clEnqueueMarker)(
    cl_command_queue /*command_queue*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
typedef cl_int(CL_API_CALL* PFN_clEnqueueWaitForEvents)(
    cl_command_queue /*command_queue*/, cl_uint /*num_events*/,
    const cl_event* /*event_list*/) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
typedef cl_int(CL_API_CALL* PFN_clEnqueueBarrier)(
    cl_command_queue /*command_queue*/) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
typedef cl_int(CL_API_CALL* PFN_clUnloadCompiler)()
    CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
typedef void*(CL_API_CALL* PFN_clGetExtensionFunctionAddress)(
    const char* /*func_name*/)CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
typedef cl_command_queue(CL_API_CALL* PFN_clCreateCommandQueue)(
    cl_context /*context*/, cl_device_id /*device*/,
    cl_command_queue_properties /*properties*/,
    cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED;
typedef cl_sampler(CL_API_CALL* PFN_clCreateSampler)(
    cl_context /*context*/, cl_bool /*normalized_coords*/,
    cl_addressing_mode /*addressing_mode*/, cl_filter_mode /*filter_mode*/,
    cl_int* /*errcode_ret*/) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED;
typedef cl_int(CL_API_CALL* PFN_clEnqueueTask)(
    cl_command_queue /*command_queue*/, cl_kernel /*kernel*/,
    cl_uint /*num_events_in_wait_list*/, const cl_event* /*event_wait_list*/,
    cl_event* /*event*/) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED;

struct OpenCLPointers {
  PFN_clCreateEventFromEGLSyncKHR clCreateEventFromEGLSyncKHR = nullptr;
  PFN_clCreateFromEGLImageKHR clCreateFromEGLImageKHR = nullptr;
  PFN_clEnqueueAcquireEGLObjectsKHR clEnqueueAcquireEGLObjectsKHR = nullptr;
  PFN_clEnqueueReleaseEGLObjectsKHR clEnqueueReleaseEGLObjectsKHR = nullptr;
  PFN_clLogMessagesToSystemLogAPPLE clLogMessagesToSystemLogAPPLE = nullptr;
  PFN_clLogMessagesToStdoutAPPLE clLogMessagesToStdoutAPPLE = nullptr;
  PFN_clLogMessagesToStderrAPPLE clLogMessagesToStderrAPPLE = nullptr;
  PFN_clIcdGetPlatformIDsKHR clIcdGetPlatformIDsKHR = nullptr;
  PFN_clCreateProgramWithILKHR clCreateProgramWithILKHR = nullptr;
  PFN_clTerminateContextKHR clTerminateContextKHR = nullptr;
  PFN_clCreateCommandQueueWithPropertiesKHR
      clCreateCommandQueueWithPropertiesKHR = nullptr;
  PFN_clReleaseDeviceEXT clReleaseDeviceEXT = nullptr;
  PFN_clRetainDeviceEXT clRetainDeviceEXT = nullptr;
  PFN_clCreateSubDevicesEXT clCreateSubDevicesEXT = nullptr;
  PFN_clEnqueueMigrateMemObjectEXT clEnqueueMigrateMemObjectEXT = nullptr;
  PFN_clGetDeviceImageInfoQCOM clGetDeviceImageInfoQCOM = nullptr;
  PFN_clEnqueueAcquireGrallocObjectsIMG clEnqueueAcquireGrallocObjectsIMG =
      nullptr;
  PFN_clEnqueueGenerateMipmapIMG clEnqueueGenerateMipmapIMG = nullptr;
  PFN_clEnqueueReleaseGrallocObjectsIMG clEnqueueReleaseGrallocObjectsIMG =
      nullptr;
  PFN_clGetKernelSubGroupInfoKHR clGetKernelSubGroupInfoKHR = nullptr;
  PFN_clImportMemoryARM clImportMemoryARM = nullptr;
  PFN_clSVMAllocARM clSVMAllocARM = nullptr;
  PFN_clSVMFreeARM clSVMFreeARM = nullptr;
  PFN_clEnqueueSVMFreeARM clEnqueueSVMFreeARM = nullptr;
  PFN_clEnqueueSVMMemcpyARM clEnqueueSVMMemcpyARM = nullptr;
  PFN_clEnqueueSVMMemFillARM clEnqueueSVMMemFillARM = nullptr;
  PFN_clEnqueueSVMMapARM clEnqueueSVMMapARM = nullptr;
  PFN_clEnqueueSVMUnmapARM clEnqueueSVMUnmapARM = nullptr;
  PFN_clSetKernelArgSVMPointerARM clSetKernelArgSVMPointerARM = nullptr;
  PFN_clSetKernelExecInfoARM clSetKernelExecInfoARM = nullptr;
  PFN_clCreateAcceleratorINTEL clCreateAcceleratorINTEL = nullptr;
  PFN_clGetAcceleratorInfoINTEL clGetAcceleratorInfoINTEL = nullptr;
  PFN_clRetainAcceleratorINTEL clRetainAcceleratorINTEL = nullptr;
  PFN_clReleaseAcceleratorINTEL clReleaseAcceleratorINTEL = nullptr;
  PFN_clCreateEventFromGLsyncKHR clCreateEventFromGLsyncKHR = nullptr;
  PFN_clGetGLContextInfoKHR clGetGLContextInfoKHR = nullptr;
  PFN_clCreateFromGLBuffer clCreateFromGLBuffer = nullptr;
  PFN_clCreateFromGLTexture clCreateFromGLTexture = nullptr;
  PFN_clCreateFromGLRenderbuffer clCreateFromGLRenderbuffer = nullptr;
  PFN_clGetGLObjectInfo clGetGLObjectInfo = nullptr;
  PFN_clGetGLTextureInfo clGetGLTextureInfo = nullptr;
  PFN_clEnqueueAcquireGLObjects clEnqueueAcquireGLObjects = nullptr;
  PFN_clEnqueueReleaseGLObjects clEnqueueReleaseGLObjects = nullptr;
  PFN_clCreateFromGLTexture2D clCreateFromGLTexture2D = nullptr;
  PFN_clCreateFromGLTexture3D clCreateFromGLTexture3D = nullptr;
  PFN_clGetDeviceIDsFromVA_APIMediaAdapterINTEL
      clGetDeviceIDsFromVA_APIMediaAdapterINTEL = nullptr;
  PFN_clCreateFromVA_APIMediaSurfaceINTEL clCreateFromVA_APIMediaSurfaceINTEL =
      nullptr;
  PFN_clEnqueueAcquireVA_APIMediaSurfacesINTEL
      clEnqueueAcquireVA_APIMediaSurfacesINTEL = nullptr;
  PFN_clEnqueueReleaseVA_APIMediaSurfacesINTEL
      clEnqueueReleaseVA_APIMediaSurfacesINTEL = nullptr;
  PFN_clHostMemAllocINTEL clHostMemAllocINTEL = nullptr;
  PFN_clDeviceMemAllocINTEL clDeviceMemAllocINTEL = nullptr;
  PFN_clSharedMemAllocINTEL clSharedMemAllocINTEL = nullptr;
  PFN_clMemFreeINTEL clMemFreeINTEL = nullptr;
  PFN_clGetMemAllocInfoINTEL clGetMemAllocInfoINTEL = nullptr;
  PFN_clSetKernelArgMemPointerINTEL clSetKernelArgMemPointerINTEL = nullptr;
  PFN_clEnqueueMemsetINTEL clEnqueueMemsetINTEL = nullptr;
  PFN_clEnqueueMemFillINTEL clEnqueueMemFillINTEL = nullptr;
  PFN_clEnqueueMemcpyINTEL clEnqueueMemcpyINTEL = nullptr;
  PFN_clEnqueueMigrateMemINTEL clEnqueueMigrateMemINTEL = nullptr;
  PFN_clEnqueueMemAdviseINTEL clEnqueueMemAdviseINTEL = nullptr;
  PFN_clCreateBufferWithPropertiesINTEL clCreateBufferWithPropertiesINTEL =
      nullptr;
  PFN_clGetPlatformIDs clGetPlatformIDs = nullptr;
  PFN_clGetPlatformInfo clGetPlatformInfo = nullptr;
  PFN_clGetDeviceIDs clGetDeviceIDs = nullptr;
  PFN_clGetDeviceInfo clGetDeviceInfo = nullptr;
  PFN_clCreateSubDevices clCreateSubDevices = nullptr;
  PFN_clRetainDevice clRetainDevice = nullptr;
  PFN_clReleaseDevice clReleaseDevice = nullptr;
  PFN_clSetDefaultDeviceCommandQueue clSetDefaultDeviceCommandQueue = nullptr;
  PFN_clGetDeviceAndHostTimer clGetDeviceAndHostTimer = nullptr;
  PFN_clGetHostTimer clGetHostTimer = nullptr;
  PFN_clCreateContext clCreateContext = nullptr;
  PFN_clCreateContextFromType clCreateContextFromType = nullptr;
  PFN_clRetainContext clRetainContext = nullptr;
  PFN_clReleaseContext clReleaseContext = nullptr;
  PFN_clGetContextInfo clGetContextInfo = nullptr;
  PFN_clSetContextDestructorCallback clSetContextDestructorCallback = nullptr;
  PFN_clCreateCommandQueueWithProperties clCreateCommandQueueWithProperties =
      nullptr;
  PFN_clRetainCommandQueue clRetainCommandQueue = nullptr;
  PFN_clReleaseCommandQueue clReleaseCommandQueue = nullptr;
  PFN_clGetCommandQueueInfo clGetCommandQueueInfo = nullptr;
  PFN_clCreateBuffer clCreateBuffer = nullptr;
  PFN_clCreateBufferWithProperties clCreateBufferWithProperties = nullptr;
  PFN_clCreateSubBuffer clCreateSubBuffer = nullptr;
  PFN_clCreateImage clCreateImage = nullptr;
  PFN_clCreateImageWithProperties clCreateImageWithProperties = nullptr;
  PFN_clCreatePipe clCreatePipe = nullptr;
  PFN_clRetainMemObject clRetainMemObject = nullptr;
  PFN_clReleaseMemObject clReleaseMemObject = nullptr;
  PFN_clGetSupportedImageFormats clGetSupportedImageFormats = nullptr;
  PFN_clGetMemObjectInfo clGetMemObjectInfo = nullptr;
  PFN_clGetImageInfo clGetImageInfo = nullptr;
  PFN_clGetPipeInfo clGetPipeInfo = nullptr;
  PFN_clSetMemObjectDestructorCallback clSetMemObjectDestructorCallback =
      nullptr;
  PFN_clSetMemObjectDestructorAPPLE clSetMemObjectDestructorAPPLE = nullptr;
  PFN_clSVMAlloc clSVMAlloc = nullptr;
  PFN_clSVMFree clSVMFree = nullptr;
  PFN_clCreateSamplerWithProperties clCreateSamplerWithProperties = nullptr;
  PFN_clRetainSampler clRetainSampler = nullptr;
  PFN_clReleaseSampler clReleaseSampler = nullptr;
  PFN_clGetSamplerInfo clGetSamplerInfo = nullptr;
  PFN_clCreateProgramWithSource clCreateProgramWithSource = nullptr;
  PFN_clCreateProgramWithBinary clCreateProgramWithBinary = nullptr;
  PFN_clCreateProgramWithBuiltInKernels clCreateProgramWithBuiltInKernels =
      nullptr;
  PFN_clCreateProgramWithIL clCreateProgramWithIL = nullptr;
  PFN_clRetainProgram clRetainProgram = nullptr;
  PFN_clReleaseProgram clReleaseProgram = nullptr;
  PFN_clBuildProgram clBuildProgram = nullptr;
  PFN_clCompileProgram clCompileProgram = nullptr;
  PFN_clLinkProgram clLinkProgram = nullptr;
  PFN_clSetProgramReleaseCallback clSetProgramReleaseCallback = nullptr;
  PFN_clSetProgramSpecializationConstant clSetProgramSpecializationConstant =
      nullptr;
  PFN_clUnloadPlatformCompiler clUnloadPlatformCompiler = nullptr;
  PFN_clGetProgramInfo clGetProgramInfo = nullptr;
  PFN_clGetProgramBuildInfo clGetProgramBuildInfo = nullptr;
  PFN_clCreateKernel clCreateKernel = nullptr;
  PFN_clCreateKernelsInProgram clCreateKernelsInProgram = nullptr;
  PFN_clCloneKernel clCloneKernel = nullptr;
  PFN_clRetainKernel clRetainKernel = nullptr;
  PFN_clReleaseKernel clReleaseKernel = nullptr;
  PFN_clSetKernelArg clSetKernelArg = nullptr;
  PFN_clSetKernelArgSVMPointer clSetKernelArgSVMPointer = nullptr;
  PFN_clSetKernelExecInfo clSetKernelExecInfo = nullptr;
  PFN_clGetKernelInfo clGetKernelInfo = nullptr;
  PFN_clGetKernelArgInfo clGetKernelArgInfo = nullptr;
  PFN_clGetKernelWorkGroupInfo clGetKernelWorkGroupInfo = nullptr;
  PFN_clGetKernelSubGroupInfo clGetKernelSubGroupInfo = nullptr;
  PFN_clWaitForEvents clWaitForEvents = nullptr;
  PFN_clGetEventInfo clGetEventInfo = nullptr;
  PFN_clCreateUserEvent clCreateUserEvent = nullptr;
  PFN_clRetainEvent clRetainEvent = nullptr;
  PFN_clReleaseEvent clReleaseEvent = nullptr;
  PFN_clSetUserEventStatus clSetUserEventStatus = nullptr;
  PFN_clSetEventCallback clSetEventCallback = nullptr;
  PFN_clGetEventProfilingInfo clGetEventProfilingInfo = nullptr;
  PFN_clFlush clFlush = nullptr;
  PFN_clFinish clFinish = nullptr;
  PFN_clEnqueueReadBuffer clEnqueueReadBuffer = nullptr;
  PFN_clEnqueueReadBufferRect clEnqueueReadBufferRect = nullptr;
  PFN_clEnqueueWriteBuffer clEnqueueWriteBuffer = nullptr;
  PFN_clEnqueueWriteBufferRect clEnqueueWriteBufferRect = nullptr;
  PFN_clEnqueueFillBuffer clEnqueueFillBuffer = nullptr;
  PFN_clEnqueueCopyBuffer clEnqueueCopyBuffer = nullptr;
  PFN_clEnqueueCopyBufferRect clEnqueueCopyBufferRect = nullptr;
  PFN_clEnqueueReadImage clEnqueueReadImage = nullptr;
  PFN_clEnqueueWriteImage clEnqueueWriteImage = nullptr;
  PFN_clEnqueueFillImage clEnqueueFillImage = nullptr;
  PFN_clEnqueueCopyImage clEnqueueCopyImage = nullptr;
  PFN_clEnqueueCopyImageToBuffer clEnqueueCopyImageToBuffer = nullptr;
  PFN_clEnqueueCopyBufferToImage clEnqueueCopyBufferToImage = nullptr;
  PFN_clEnqueueMapBuffer clEnqueueMapBuffer = nullptr;
  PFN_clEnqueueMapImage clEnqueueMapImage = nullptr;
  PFN_clEnqueueUnmapMemObject clEnqueueUnmapMemObject = nullptr;
  PFN_clEnqueueMigrateMemObjects clEnqueueMigrateMemObjects = nullptr;
  PFN_clEnqueueNDRangeKernel clEnqueueNDRangeKernel = nullptr;
  PFN_clEnqueueNativeKernel clEnqueueNativeKernel = nullptr;
  PFN_clEnqueueMarkerWithWaitList clEnqueueMarkerWithWaitList = nullptr;
  PFN_clEnqueueBarrierWithWaitList clEnqueueBarrierWithWaitList = nullptr;
  PFN_clEnqueueSVMFree clEnqueueSVMFree = nullptr;
  PFN_clEnqueueSVMMemcpy clEnqueueSVMMemcpy = nullptr;
  PFN_clEnqueueSVMMemFill clEnqueueSVMMemFill = nullptr;
  PFN_clEnqueueSVMMap clEnqueueSVMMap = nullptr;
  PFN_clEnqueueSVMUnmap clEnqueueSVMUnmap = nullptr;
  PFN_clEnqueueSVMMigrateMem clEnqueueSVMMigrateMem = nullptr;
  PFN_clGetExtensionFunctionAddressForPlatform
      clGetExtensionFunctionAddressForPlatform = nullptr;
  PFN_clSetCommandQueueProperty clSetCommandQueueProperty = nullptr;
  PFN_clCreateImage2D clCreateImage2D = nullptr;
  PFN_clCreateImage3D clCreateImage3D = nullptr;
  PFN_clEnqueueMarker clEnqueueMarker = nullptr;
  PFN_clEnqueueWaitForEvents clEnqueueWaitForEvents = nullptr;
  PFN_clEnqueueBarrier clEnqueueBarrier = nullptr;
  PFN_clUnloadCompiler clUnloadCompiler = nullptr;
  PFN_clGetExtensionFunctionAddress clGetExtensionFunctionAddress = nullptr;
  PFN_clCreateCommandQueue clCreateCommandQueue = nullptr;
  PFN_clCreateSampler clCreateSampler = nullptr;
  PFN_clEnqueueTask clEnqueueTask = nullptr;
};

}  // namespace gf_layers::opencl_capture

#pragma clang diagnostic pop

#endif  // GF_LAYERS_OPENCL_CAPTURE_OPENCL_GEN_H
