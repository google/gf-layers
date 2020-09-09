# Linker script for Android.
# Generated file; do not edit.
{
global:
    VkLayer_GF_shader_fuzzerNegotiateLoaderLayerInterfaceVersion;
    VkLayer_GF_shader_fuzzerGetInstanceProcAddr;
    VkLayer_GF_shader_fuzzerGetDeviceProcAddr;
    # Introspection functions must be exported on Android.
    vkEnumerateInstanceLayerProperties;
    vkEnumerateDeviceLayerProperties;
    vkEnumerateInstanceExtensionProperties;
    vkEnumerateDeviceExtensionProperties;
local:
    *;
};
