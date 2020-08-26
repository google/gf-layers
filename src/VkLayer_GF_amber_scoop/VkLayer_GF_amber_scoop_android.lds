# Linker script for Android.
# Generated file; do not edit.
{
global:
    VkLayer_GF_amber_scoopNegotiateLoaderLayerInterfaceVersion;
    VkLayer_GF_amber_scoopGetInstanceProcAddr;
    VkLayer_GF_amber_scoopGetDeviceProcAddr;
    # Introspection functions must be exported on Android.
    vkEnumerateInstanceLayerProperties;
    vkEnumerateDeviceLayerProperties;
    vkEnumerateInstanceExtensionProperties;
    vkEnumerateDeviceExtensionProperties;
local:
    *;
};
