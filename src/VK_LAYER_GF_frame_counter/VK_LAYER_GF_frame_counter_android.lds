# Linker script for Android.
# Generated file; do not edit.
{
global:
    VK_LAYER_GF_frame_counterNegotiateLoaderLayerInterfaceVersion;
    VK_LAYER_GF_frame_counterGetInstanceProcAddr;
    VK_LAYER_GF_frame_counterGetDeviceProcAddr;
    # Introspection functions must be exported on Android.
    vkEnumerateInstanceLayerProperties;
    vkEnumerateDeviceLayerProperties;
    vkEnumerateInstanceExtensionProperties;
    vkEnumerateDeviceExtensionProperties;
local:
    *;
};
