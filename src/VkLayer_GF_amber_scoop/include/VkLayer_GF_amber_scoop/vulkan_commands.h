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

#ifndef VKLAYER_GF_AMBER_SCOOP_VULKAN_COMMANDS_H
#define VKLAYER_GF_AMBER_SCOOP_VULKAN_COMMANDS_H

#include <vulkan/vulkan.h>

#include <cstdint>

#include "VkLayer_GF_amber_scoop/vk_deep_copy.h"

namespace gf_layers::amber_scoop_layer {

class DrawCallTracker;

// These classes contain unused private fields that will be used later. The
// warning is disable to allow the classes to store all the fields required in
// the future.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"

// Cmd is a base for all vkCmdXX commands (Vulkan command buffer commands) we
// are interested in. The interesting commands include all the commands that may
// affect the draw command(s) that will be captured.
class Cmd {
 public:
  // Types of interesting commands. Names should be the same as the Vulkan's
  // equivalent command names: "vkCmdXX" without the "vkCmd" prefix.
  enum Kind {
    kBeginRenderPass,
    kBindPipeline,
    kDraw,
    kDrawIndexed,
  };

  explicit Cmd(Kind kind) : kind_(kind) {}

  virtual ~Cmd();

  // Disabled copy/move constructors and copy/move assign operators
  Cmd(const Cmd&) = delete;
  Cmd(Cmd&&) = delete;
  Cmd& operator=(const Cmd&) = delete;
  Cmd& operator=(Cmd&&) = delete;

  // Returns the command type.
  [[nodiscard]] Kind GetKind() const { return kind_; }

  // Overriding function should either update the given draw call tracker (most
  // of the commands) or generate an Amber file (draw commands).
  virtual void ProcessSubmittedCommand(DrawCallTracker*) = 0;

 private:
  Kind kind_;
  // clang-format on
};

class CmdBeginRenderPass : public Cmd {
 public:
  CmdBeginRenderPass(VkRenderPassBeginInfo const* renderpass_begin,
                     VkSubpassContents contents)
      : Cmd(kBeginRenderPass),
        render_pass_begin_(DeepCopy(renderpass_begin)),
        contents_(contents) {}

  ~CmdBeginRenderPass() override { DeepDelete(&render_pass_begin_); }

  // Disabled copy/move constructors and copy/move assign operators
  CmdBeginRenderPass(const CmdBeginRenderPass&) = delete;
  CmdBeginRenderPass(CmdBeginRenderPass&&) = delete;
  CmdBeginRenderPass& operator=(const CmdBeginRenderPass&) = delete;
  CmdBeginRenderPass& operator=(CmdBeginRenderPass&&) = delete;

  // Sets the current render pass.
  void ProcessSubmittedCommand(DrawCallTracker* draw_call_state) override;

 private:
  VkRenderPassBeginInfo render_pass_begin_;
  VkSubpassContents contents_;
};

class CmdBindPipeline : public Cmd {
 public:
  CmdBindPipeline(VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline)
      : Cmd(kBindPipeline),
        pipeline_bind_point_(pipeline_bind_point),
        pipeline_(pipeline) {}

  // Updates the bound pipeline.
  void ProcessSubmittedCommand(
      DrawCallTracker* draw_call_state_tracker) override;

 private:
  VkPipelineBindPoint pipeline_bind_point_;
  VkPipeline pipeline_;
};

class CmdDraw : public Cmd {
 public:
  CmdDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
          uint32_t first_instance)
      : Cmd(kDraw),
        vertex_count_(vertex_count),
        instance_count_(instance_count),
        first_vertex_(first_vertex),
        first_instance_(first_instance) {}

  // Generates an Amber file if the draw call is marked to be captured.
  void ProcessSubmittedCommand(
      DrawCallTracker* draw_call_state_tracker) override;

 private:
  uint32_t vertex_count_;
  uint32_t instance_count_;
  uint32_t first_vertex_;
  uint32_t first_instance_;
};

class CmdDrawIndexed : public Cmd {
 public:
  CmdDrawIndexed(uint32_t index_count, uint32_t instance_count,
                 uint32_t first_index, int32_t vertex_offset,
                 uint32_t first_instance)
      : Cmd(kDrawIndexed),
        index_count_(index_count),
        instance_count_(instance_count),
        first_index_(first_index),
        vertex_offset_(vertex_offset),
        first_instance_(first_instance) {}

  // Generates an Amber file if the draw call is marked to be captured.
  void ProcessSubmittedCommand(
      DrawCallTracker* draw_call_state_tracker) override;

 private:
  uint32_t index_count_;
  uint32_t instance_count_;
  uint32_t first_index_;
  int32_t vertex_offset_;
  uint32_t first_instance_;
};

#pragma clang diagnostic pop

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_VULKAN_COMMANDS_H
