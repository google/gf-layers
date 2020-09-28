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

#ifndef VKLAYER_GF_AMBER_SCOOP_DRAW_CALL_TRACKER_H
#define VKLAYER_GF_AMBER_SCOOP_DRAW_CALL_TRACKER_H

#include <vulkan/vulkan.h>

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "VkLayer_GF_amber_scoop/amber_scoop_layer.h"

namespace gf_layers::amber_scoop_layer {

// Draw call tracker is used to store the state of a draw call while processing
// the command buffers in vkQueueSubmit(...) -function. One draw call tracker is
// created for each command buffer containing draw calls. This class is also
// responsible for generating the Amber files. See HandleDrawCall function.
class DrawCallTracker {
 public:
  // DrawCallState is a container for the state data.
  struct DrawCallState {
    VkCommandBuffer command_buffer;
    VkQueue queue;
    VkRenderPassBeginInfo* current_render_pass;
    uint32_t current_subpass;
    VkPipeline graphics_pipeline;
    std::vector<uint8_t> push_constant_data;
    // Map of vertex buffer bindings. Key is the binding number and value is the
    // buffer handle.
    std::unordered_map<uint32_t, VkBuffer> bound_vertex_buffers;
    // Map of vertex buffer offsets. Key is the binding number of the buffer and
    // value is the vertex buffer offset.
    std::unordered_map<uint32_t, VkDeviceSize> vertex_buffer_offsets;
  };

  explicit DrawCallTracker(GlobalData* global_data)
      : global_data_(global_data) {}

  // Gets a pointer to the editable draw call state struct.
  DrawCallState* GetDrawCallState() { return &draw_call_state_; }

  // Handles all of the draw calls. An Amber file will be generated
  // from the draw call if the draw call is set to be captured via settings.
  void HandleDrawCall(uint32_t first_index, uint32_t index_count,
                      uint32_t first_vertex, uint32_t vertex_count,
                      uint32_t first_instance, uint32_t instance_count);

 private:
  DrawCallState draw_call_state_ = {};
  // Pointer to the global data. Used in HandleDrawCall function.
  GlobalData* global_data_;
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_DRAW_CALL_TRACKER_H
