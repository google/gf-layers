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

#ifndef GF_LAYERS_LAYER_UTIL_UTIL_H
#define GF_LAYERS_LAYER_UTIL_UTIL_H

#include <vulkan/vk_layer.h>
#include <vulkan/vulkan.h>

#include <cstddef>
#include <mutex>
#include <unordered_map>
#include <utility>

namespace gf_layers {
using MutexType = std::mutex;

// Warning: we currently require the values to have pointer stability.
// This is guaranteed for std::unordered_map.
template <typename K, typename V>
using MapTemplate = std::unordered_map<K, V>;

using ScopedLock = std::unique_lock<MutexType>;

// A "tiny" map that should typically only be used to map from the dispatch
// table for VkInstance and VkDevice. Entries cannot be deleted from this map
// and so it is generally only suitable for VkInstance and VkDevice data under
// the assumption that applications will only create a few of these objects
// (typically just one of each) throughout the lifetime of the application.
// A thread-local cache of size 1 is used to enable fast lookup and so locking
// the mutex will usually be avoided.
// See the comment in the implementation of |get| for more details.
template <typename KeyType, typename ValueType>
class ProtectedTinyStaleMap {
 public:
  // Warning: we currently require the values to have pointer stability.
  // This is guaranteed for std::unordered_map.
  using InternalMapType = MapTemplate<KeyType, ValueType>;
  using ThreadLocalCacheType = std::pair<KeyType, ValueType*>;

  ValueType* get(KeyType key) {
    // If we allowed removal from the map then this method could return stale
    // values due to its thread-local cache. E.g. if one thread removed an entry
    // from the map, then another thread might still see the cached entry. This
    // would normally be fine for most situations in Vulkan, as applications
    // must not access destroyed Vulkan objects. However, there would still be a
    // risk of returning stale data, albeit extremely unlikely in practice: a
    // thread destroys a Vulkan object, then creates a new object that gets the
    // same handle as the old object. Other threads may have cached the
    // key-value pair for the old object, and so would get a value related to
    // the old object when validly using the new Vulkan handle as the key. To
    // workaround this, we disallow deletion from this map. Thus, if an old
    // Vulkan handle is re-used, the value is simply updated. One might think
    // there is still a potential for data races on the value when it is being
    // updated, as the reader thread(s) won't be holding the mutex. However, if
    // a thread validly uses the new Vulkan object (causing a map lookup) then
    // the new object must have been created in this thread or there must have
    // been some synchronization between the creating thread where the handle
    // was returned and this thread, otherwise this thread is using a
    // potentially invalid handle (racing on the handle creation itself).

    // Use thread-local cache to avoid mutex and map lookup in most cases.
    ThreadLocalCacheType& thread_local_cache = get_thread_local_cache();
    if (thread_local_cache.first == key) {
      return thread_local_cache.second;
    }

    // Otherwise, lock and lookup.
    ScopedLock lock(mutex_);

    ValueType* result = nullptr;
    auto it = map_.find(key);
    if (it != map_.end()) {
      result = &it->second;
    }

    // Update cache.
    thread_local_cache.first = key;
    thread_local_cache.second = result;

    return result;
  }

  void put(KeyType key, ValueType value) {
    ScopedLock lock(mutex_);
    map_[key] = std::move(value);
  }

 private:
  ThreadLocalCacheType& get_thread_local_cache() {
    static thread_local ThreadLocalCacheType thread_local_cache;
    return thread_local_cache;
  }

  InternalMapType map_;
  MutexType mutex_;
};

template <typename KeyType, typename ValueType>
class ProtectedMap {
 public:
  // Warning: we currently require the values to have pointer stability.
  // This is guaranteed for std::unordered_map.
  using InternalMapType = MapTemplate<KeyType, ValueType>;

  size_t count(KeyType key) const {
    ScopedLock lock(mutex_);
    return map_.count(key);
  }

  ValueType* get(KeyType key) {
    ScopedLock lock(mutex_);
    ValueType* result = nullptr;
    auto it = map_.find(key);
    if (it != map_.end()) {
      result = &it->second;
    }
    return result;
  }

  bool put(const KeyType key, ValueType value) {
    ScopedLock lock(mutex_);
    return map_.emplace(key, std::move(value)).second;
  }

  InternalMapType* access(ScopedLock* lock) {
    ScopedLock local_lock(mutex_);
    lock->swap(local_lock);
    return &map_;
  }

 private:
  InternalMapType map_;
  mutable MutexType mutex_;
};

// The following are dispatchable handles:
//
// - VkInstance
// - VkPhysicalDevice (same dispatch table as its VkInstance)
//
// - VkDevice
// - VkQueue (same dispatch table as its VkDevice)
// - VkCommandBuffer (same dispatch table as its VkDevice)
//
// Using the dispatch table pointer of such handles as keys means we don't need
// to track physical devices, queues, or command buffers, unless we want to
// store information about each of these objects. We must track VkInstance and
// VkDevice objects because nearly all Vulkan function addresses can be
// specialized for each VkInstance/VkDevice object, and thus we need to store
// the next Vulkan function address for those functions we wish to intercept
// so that we can call the correct function in the next layer.

// Returns the VkInstance dispatch table pointer for |handle|.
void* instance_key(VkInstance handle);

// Returns the VkInstance (not VkPhysicalDevice) dispatch table pointer for
// |handle|.
void* instance_key(VkPhysicalDevice handle);

// Returns the VkDevice dispatch table pointer for |handle|.
void* device_key(VkDevice handle);

// Returns the VkQueue dispatch table pointer (same dispatch table as its
// VkDevice) for |handle|.
void* device_key(VkQueue handle);

// Returns the VkCommandBuffer dispatch table pointer (same dispatch table as
// its VkDevice) for |handle|.
void* device_key(VkCommandBuffer handle);

// Returns the VkLayerInstanceCreateInfo from the |pCreateInfo| given in a call
// to vkCreateInstance. The VkLayerInstanceCreateInfo is needed to (a) obtain
// the next GetInstanceProcAddr function and; (b) advance the "layer info"
// linked list so that the next layer will be able to get its layer info.
VkLayerInstanceCreateInfo* get_layer_instance_create_info(
    const VkInstanceCreateInfo* pCreateInfo);

// Returns the VkLayerDeviceCreateInfo from the |pCreateInfo| given in a call to
// vkCreateDevice. The VkLayerDeviceCreateInfo is needed to (a) obtain the next
// Get{Instance,Device}ProcAddr functions and; (b) advance the "layer info"
// linked list so that the next layer will be able to get its layer info.
VkLayerDeviceCreateInfo* get_layer_device_create_info(
    const VkDeviceCreateInfo* pCreateInfo);

}  // namespace gf_layers

#endif  // GF_LAYERS_LAYER_UTIL_UTIL_H
