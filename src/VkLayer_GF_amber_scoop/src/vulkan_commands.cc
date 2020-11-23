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

#include <algorithm>  // IWYU pragma: keep
// <algorithm> is used by GCC's libstdc++, but not LLVM's libc++.
#include <unordered_map>

#include "VkLayer_GF_amber_scoop/draw_call_tracker.h"

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
    draw_call_state_tracker->GetDrawCallState()->graphics_pipeline = pipeline_;
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
