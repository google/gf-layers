# Linker script for Android.
# Generated file; do not edit.
{
global:
    VkLayer_GF_frame_counterNegotiateLoaderLayerInterfaceVersion;
    VkLayer_GF_frame_counterGetInstanceProcAddr;
    VkLayer_GF_frame_counterGetDeviceProcAddr;
    # Introspection functions must be exported on Android.
    vkEnumerateInstanceLayerProperties;
    vkEnumerateDeviceLayerProperties;
    vkEnumerateInstanceExtensionProperties;
    vkEnumerateDeviceExtensionProperties;
local:
    *;
};
