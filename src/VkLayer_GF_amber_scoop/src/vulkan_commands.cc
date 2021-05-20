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

#include "VkLayer_GF_amber_scoop/vulkan_commands.h"

// <algorithm> is used by GCC's libstdc++, but not LLVM's libc++.
#include <algorithm>  // IWYU pragma: keep
#include <unordered_map>

// <cstdio> and <cstdlib> required by LOG and ASSERT macros, but IWYU doesn't
// notice it, so we need to manually keep the includes.
#include <cstdio>   // IWYU pragma: keep
#include <cstdlib>  // IWYU pragma: keep

#include "VkLayer_GF_amber_scoop/draw_call_tracker.h"
#include "gf_layers_layer_util/logging.h"

namespace gf_layers::amber_scoop_layer {

Cmd::Cmd() = default;

Cmd::~Cmd() = default;

void CmdBeginRenderPass::ProcessSubmittedCommand(
    DrawCallTracker* draw_call_state_tracker) {
  draw_call_state_tracker->GetDrawCallState()->current_render_pass =
      &render_pass_begin_;
  // TODO(ilkkasaa): currently only one subpass is supported.
  draw_call_state_tracker->GetDrawCallState()->current_subpass = 0;
}

void CmdBindDescriptorSets::ProcessSubmittedCommand(
    DrawCallTracker* draw_call_state_tracker) {
  if (pipeline_bind_point_ != VK_PIPELINE_BIND_POINT_GRAPHICS) {
    DEBUG_LOG("Only graphics pipeline bind point is currently supported.");
    return;
  }

  // Index for reading data from the dynamic offset array. Value is updated in
  // the BindGraphicsDescriptorSet function.
  uint32_t dynamic_offset_idx = 0;
  // Loop through all descriptor sets to be updated. Update the draw call
  // tracker's state with the descriptor sets and their dynamic offsets.
  uint32_t descriptor_set_idx = 0;
  for (const VkDescriptorSet& descriptor_set : descriptor_sets_) {
    draw_call_state_tracker->BindGraphicsDescriptorSet(
        first_set_ + descriptor_set_idx, descriptor_set, dynamic_offsets_,
        &dynamic_offset_idx);
    descriptor_set_idx++;
  }

  DEBUG_ASSERT_MSG(dynamic_offset_idx == dynamic_offsets_.size(),
                   "dynamicOffsetCount is not equal to the number of dynamic "
                   "descriptors in the sets being bound.");
}

void CmdBindIndexBuffer::ProcessSubmittedCommand(
    DrawCallTracker* draw_call_state_tracker) {
  auto& bound_index_buffer =
      draw_call_state_tracker->GetDrawCallState()->bound_index_buffer;

  bound_index_buffer.buffer = buffer_;
  bound_index_buffer.offset = offset_;
  bound_index_buffer.index_type = index_type_;
}

void CmdBindPipeline::ProcessSubmittedCommand(
    DrawCallTracker* draw_call_state_tracker) {
  // Currently we are interested in graphics pipelines only.
  if (pipeline_bind_point_ == VK_PIPELINE_BIND_POINT_GRAPHICS) {
    draw_call_state_tracker->BindGraphicsPipeline(pipeline_);
  }
}

void CmdBindVertexBuffers::ProcessSubmittedCommand(
    DrawCallTracker* draw_call_state_tracker) {
  // Set vertex buffer bindings and their offsets.

  auto& bound_vertex_buffers =
      draw_call_state_tracker->GetDrawCallState()->bound_vertex_buffers;

  for (uint32_t binding_idx = 0; binding_idx < buffers_.size(); binding_idx++) {
    bound_vertex_buffers[binding_idx + first_binding_] = {
        buffers_[binding_idx], offsets_[binding_idx]};
  }
}

void CmdDraw::ProcessSubmittedCommand(
    DrawCallTracker* draw_call_state_tracker) {
  draw_call_state_tracker->HandleDrawCall(0, 0, first_vertex_, vertex_count_,
                                          first_instance_, instance_count_);
}

void CmdDrawIndexed::ProcessSubmittedCommand(
    DrawCallTracker* draw_call_state_tracker) {
  draw_call_state_tracker->HandleDrawCall(first_index_, index_count_, 0, 0,
                                          first_instance_, instance_count_);
}

void CmdPipelineBarrier::ProcessSubmittedCommand(
    DrawCallTracker* draw_call_state_tracker) {
  draw_call_state_tracker->GetDrawCallState()->pipeline_barriers.push_back(
      this);
}

}  // namespace gf_layers::amber_scoop_layer
