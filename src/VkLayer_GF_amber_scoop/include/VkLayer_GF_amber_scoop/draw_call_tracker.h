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
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "VkLayer_GF_amber_scoop/amber_scoop_layer.h"
#include "VkLayer_GF_amber_scoop/create_info_wrapper.h"
#include "VkLayer_GF_amber_scoop/graphics_pipeline_data.h"
#include "VkLayer_GF_amber_scoop/vulkan_commands.h"
#include "gf_layers_layer_util/util.h"

namespace gf_layers::amber_scoop_layer {

// Draw call tracker is used to store the state of a draw call while processing
// the command buffers in vkQueueSubmit(...) -function. One draw call tracker is
// created for each command buffer containing draw calls. This class is also
// responsible for generating the Amber files. See HandleDrawCall function.
class DrawCallTracker {
 public:
  // Type aliases used to clarify the maps' key and value types.
  using DescriptorSetNumber = uint32_t;
  using BindingNumber = uint32_t;
  using DynamicOffset = uint32_t;

  struct DescriptorSetWrapper {
    VkDescriptorSet descriptor_set = {};
    // Dynamic offsets used in this descriptor set. Key is the binding number of
    // the descriptor(s) and value is a list of dynamic offset(s) for the
    // binding.
    std::unordered_map<BindingNumber, std::vector<DynamicOffset>>
        dynamic_offsets;
  };

  // Struct used to store index buffer binding information from
  // |vkCmdBindIndexBuffer| command.
  struct IndexBufferBinding {
    VkBuffer buffer;
    VkDeviceSize offset;
    VkIndexType index_type;
  };

  // Struct used to store vertex buffer binding information from
  // |vkCmdBindVertexBuffers| command.
  struct VertexBufferBinding {
    VkBuffer buffer;
    VkDeviceSize offset;
  };

  // DrawCallState is a container for the state data.
  struct DrawCallState {
    VkCommandBuffer command_buffer;
    VkQueue queue;
    IndexBufferBinding bound_index_buffer;
    VkRenderPassBeginInfo* current_render_pass;
    uint32_t current_subpass;
    // Bound graphics pipeline;
    VkPipeline graphics_pipeline;
    // Graphics pipeline's layout data.
    const PipelineLayoutData* graphics_pipeline_layout_data;
    std::unordered_map<DescriptorSetNumber, DescriptorSetWrapper>
        graphics_pipeline_descriptor_sets;
    std::vector<uint8_t> push_constant_data;
    // Map of vertex buffer bindings and offsets. Key is the binding number and
    // value is a struct of buffer handle and offset.
    std::unordered_map<uint32_t, VertexBufferBinding> bound_vertex_buffers;
    // List of pipeline barrier commands. This is used to keep list of pipeline
    // barriers that may affect the data used in a draw call that will be
    // captured, i.e. barriers that have interesting bits set in |dstStageMask|.
    std::vector<const CmdPipelineBarrier*> pipeline_barriers;
  };

  // |global_data| Global data.
  // |command_buffer| Command buffer related to this draw call tracker.
  // |queue| Queue where the commands will be submitted.
  DrawCallTracker(GlobalData* global_data, VkCommandBuffer command_buffer,
                  VkQueue queue)
      : global_data_(global_data) {
    draw_call_state_.command_buffer = command_buffer;
    draw_call_state_.queue = queue;
  }

  // Gets a pointer to the editable draw call state struct.
  DrawCallState* GetDrawCallState() { return &draw_call_state_; }

  // Returns pointer to the device data of the device.
  DeviceData* GetDeviceData() {
    return global_data_->device_map
        .Get(DeviceKey(draw_call_state_.command_buffer))
        ->get();
  }

  // Binds the given graphics pipeline and performs pipeline layout
  // compatibility check (not implemented yet).
  void BindGraphicsPipeline(VkPipeline graphics_pipeline) {
    draw_call_state_.graphics_pipeline = graphics_pipeline;
    GraphicsPipelineData* pipeline_data =
        GetDeviceData()->graphics_pipelines.Get(graphics_pipeline)->get();
    draw_call_state_.graphics_pipeline_layout_data =
        GetDeviceData()->pipeline_layouts.Get(
            pipeline_data->GetCreateInfo().layout);
  }

  // Stores the given descriptor set binding. Also stores the dynamic offsets.
  // |set_number| Descriptor set number.
  // |descriptor_set| Vulkan handle to the descriptor set.
  // |dynamic_offsets| Vector of dynamic offsets. This vector contains all of
  // the dynamic offsets from the vkCmdBindDescriptorSets command. Therefore it
  // may contain offsets related to other descriptor sets.
  // |dynamic_offset_idx| Index used to read proper content's from the
  // dynamic offsets vector. The index is increased by one in this function when
  // a value is read from the vector.
  void BindGraphicsDescriptorSet(uint32_t set_number,
                                 VkDescriptorSet descriptor_set,
                                 const std::vector<uint32_t>& dynamic_offsets,
                                 uint32_t* dynamic_offset_idx);

  // Handles all of the draw calls. An Amber file will be generated
  // from the draw call if the draw call is set to be captured via settings.
  void HandleDrawCall(uint32_t first_index, uint32_t index_count,
                      uint32_t first_vertex, uint32_t vertex_count,
                      uint32_t first_instance, uint32_t instance_count);

 private:
  // Creates the part of the Amber file where the descriptor sets and
  // descriptors are declared.
  // |device_data| Pointer to device data.
  // |buffer_declaration_str| String stream where the descriptor set definitions
  // will be collected.
  // |pipeline_str| String stream where the bind commands will be collected.
  void CreateDescriptorSetDeclarations(
      DeviceData* device_data, std::ostringstream& buffer_declaration_str,
      std::ostringstream& pipeline_str);

  // Creates the part of the Amber file where the index buffers are declared.
  // Copies |index_count| amount of indices from the index buffer (starting from
  // the offset defined in the index buffer binding).
  // Parameters:
  // |device_data| Pointer to device data.
  // |index_count| Index count from the draw command.
  // |max_index_value| Highest index value in the index buffer. This is an
  // output value.
  // |declaration_string_stream| String stream where the buffer declarations
  // will be collected.
  // |pipeline_string_stream| String stream  where the pipeline definitions will
  // be collected.
  void CreateIndexBufferDeclarations(
      DeviceData* device_data, uint32_t index_count, uint32_t* max_index_value,
      std::ostringstream& declaration_string_stream,
      std::ostringstream& pipeline_string_stream) const;

  // Creates the part of the Amber file where the vertex buffers are declared
  // and adds the vertex buffers to the pipeline. Copies the Vulkan buffers used
  // as vertex buffers to a host visible memory and writes the contents to
  // files. The whole `VkBuffer` is copied. Asserts if vertex buffer(s) can't be
  // found, because Amber requires at least one vertex buffer.
  // Parameters:
  // |device_data| Pointer to device data.
  // |declaration_string_stream| String stream where the buffer declarations
  // will be collected.
  // |pipeline_string_stream| String stream  where the pipeline definitions will
  // be collected.
  void CreateVertexBufferDeclarations(
      DeviceData* device_data, std::ostringstream& buffer_declaration_str,
      std::ostringstream& pipeline_str);

  // Gets command pool used to create the current command buffer.
  VkCommandPool GetCommandPool(DeviceData* device_data) const;

  DrawCallState draw_call_state_ = {};
  // Pointer to the global data. Used in HandleDrawCall function and may be
  // accessed via |GetDeviceData| function.
  GlobalData* global_data_;
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_DRAW_CALL_TRACKER_H
