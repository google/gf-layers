
* Manifest:
  * Use the latest file format version. 1.1.2?
  * name: `VK_LAYER_<ORG>_<layer_name>`
  * api_version: depends on the Vulkan SDK/headers. 1.1.130?
  * functions: technically we only need
vkNegotiateLoaderLayerInterfaceVersion
but to remain backwards compatible on desktop
and to be compatible with Android,
we must:
    * name all exported functions like `<layer name><function name>` (with no space between).
    * export `<layer name>vkGetDeviceProcAddr` and `<layer name>vkGetInstanceProcAddr`

We implement C functions:
  * vkEnumerateInstanceLayerProperties
  * vkEnumerateDeviceLayerProperties
  * vkEnumerateInstanceExtensionProperties
  * vkEnumerateDeviceExtensionProperties

But we only export them on Android.


The .so name is not important, but we may as well
make it the same as the layer name.
This makes it easier to load implicit layers
on Android, where there is no .json manifest to see
what a layer is called.



Bug? Layer documentation says:
in vkCreateDevice to call
vkInstanceGetProcAddr with
NULL as the instance, but this seems to be invalid.
It is trivial to get the
VkInstance associated with the VkPhysicalDevice.


The following are dispatchable handles:

- VkInstance
- VkPhysicalDevice (same dispatch table as its VkInstance)

- VkDevice
- VkQueue (same dispatch table as its VkDevice)
- VkCommandBuffer (same dispatch table as its VkDevice)

Using the dispatch table pointer of such handles as keys means we don't need
to track physical devices, queues, or command buffers, unless we want to
store information about each of these objects.
 
Non-dispatchable handles are not guaranteed to be unique
within or across types.
However, they are likely to be unique most of the time,
and we can use the Khronos Vulkan validation layer,
which wraps all objects, making them unique.
