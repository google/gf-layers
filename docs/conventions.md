# Coding conventions

We follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with one exception; when we implement Vulkan functions, we use the original **function name** and **parameter names**, even if they violate the naming style and even if the function is not externally visible. **Local variables** should still try to following the Google C++ style in most cases. For example:

```c++
namespace gf_layers::frame_counter_layer {
namespace {

// Note:
// - vkGetDeviceProcAddr
// - pName
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetDeviceProcAddr(VkDevice device, const char* pName) {

  // Local variables follow the Google C++ Style.
  DeviceData device_data = ... device ...;
  if (pName != nullptr) {
    std::string function_name = pName;
    // ...
  }
}

}
}
```
