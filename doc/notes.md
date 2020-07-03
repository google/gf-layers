

Layers typically have a library name like "libVkLayer_blah.so"
and a name (the name in the JSON manifest or returned from vkEnumerateInstanceLayerProperties) like "VK_LAYER_ORG_blah",
where "ORG" might be "KHRONOS", "GOOG", "LUNARG", etc.
We previously decided to unify things to get a library "libVK_LAYER_ORG_blah.so" with a layer name of "VK_LAYER_ORG_blah".
However, it turns out that Android _requires_ the "libVkLayer" prefix (and ".so" suffix).
Thus, we now use "libVkLayer_ORG_blah.so" with a layer name of "VkLayer_ORG_blah".

* Manifest:
  * Use the latest file format version. 1.1.2?
  * name: `VkLayer_<ORG>_<more_words_here>`
  * api_version: depends on the Vulkan SDK/headers. 1.1.130?
  * functions: technically we only need
vkNegotiateLoaderLayerInterfaceVersion
but to remain backwards compatible on desktop
and to be compatible with Android,
we must:
    * name all exported functions like `<layer name><function name>` (with no space between).
    * export `<layer name>vkGetDeviceProcAddr` and `<layer name>vkGetInstanceProcAddr`
    * export the old introspection functions (without the `<layer name>` prefix) for Android (see below)

We implement C functions:
  * vkEnumerateInstanceLayerProperties
  * vkEnumerateDeviceLayerProperties
  * vkEnumerateInstanceExtensionProperties
  * vkEnumerateDeviceExtensionProperties

But we only export them on Android.


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
