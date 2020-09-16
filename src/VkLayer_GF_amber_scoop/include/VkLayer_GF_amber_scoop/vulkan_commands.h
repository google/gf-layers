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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow-field-in-constructor"

namespace gf_layers::amber_scoop_layer {

struct CmdBeginRenderPass;
struct CmdBindPipeline;
struct CmdDraw;
struct CmdDrawIndexed;

// Cmd is a base for all vkCmdXXXX commands (Vulkan command buffer commands) we
// are interested in. The interesting commands include all the commands that may
// affect somehow the draw command(s) that will be captured.
struct Cmd {
  // Types of the interesting commands. Names should be the same as the Vulkan's
  // equivalent command names: "vkCmdXXX" without the "vkCmd" prefix.
  enum Kind {
    kBeginRenderPass,
    kBindPipeline,
    kDraw,
    kDrawIndexed,
  };

  explicit Cmd(Kind kind) : kind(kind) {}

  virtual ~Cmd();

  // Disabled copy/move constructors and copy/move assign operators
  Cmd(const Cmd&) = delete;
  Cmd(Cmd&&) = delete;
  Cmd& operator=(const Cmd&) = delete;
  Cmd& operator=(Cmd&&) = delete;

  // A bunch of methods for casting this type to a given type. Returns this if
  // the cast can be done, nullptr otherwise.
  // clang-format off
#define DeclareCastMethod(target)                                              \
  virtual Cmd##target *As##target() { return nullptr; }                        \
  virtual const Cmd##target *As##target() const { return nullptr; }
  DeclareCastMethod(BeginRenderPass)
  DeclareCastMethod(BindPipeline)
  DeclareCastMethod(Draw)
  DeclareCastMethod(DrawIndexed)
#undef DeclareCastMethod

  Kind kind;
  // clang-format on
};

struct CmdBeginRenderPass : public Cmd {
  CmdBeginRenderPass(VkRenderPassBeginInfo const* renderpass_begin,
                     VkSubpassContents contents)
      : Cmd(kBeginRenderPass),
        render_pass_begin(DeepCopy(renderpass_begin)),
        contents(contents) {}

  ~CmdBeginRenderPass() override { DeepDelete(&render_pass_begin); }

  // Disabled copy/move constructors and copy/move assign operators
  CmdBeginRenderPass(const CmdBeginRenderPass&) = delete;
  CmdBeginRenderPass(CmdBeginRenderPass&&) = delete;
  CmdBeginRenderPass& operator=(const CmdBeginRenderPass&) = delete;
  CmdBeginRenderPass& operator=(CmdBeginRenderPass&&) = delete;

  // Functions for getting a reference to this instance with correct type.
  CmdBeginRenderPass* AsBeginRenderPass() override;
  [[nodiscard]] const CmdBeginRenderPass* AsBeginRenderPass() const override;

  VkRenderPassBeginInfo render_pass_begin;
  VkSubpassContents contents;
};

struct CmdBindPipeline : public Cmd {
  CmdBindPipeline(VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline)
      : Cmd(kBindPipeline),
        pipeline_bind_point(pipeline_bind_point),
        pipeline(pipeline) {}

  // Functions for getting a reference to this instance with correct type.
  CmdBindPipeline* AsBindPipeline() override;
  [[nodiscard]] const CmdBindPipeline* AsBindPipeline() const override;

  VkPipelineBindPoint pipeline_bind_point;
  VkPipeline pipeline;
};

struct CmdDraw : public Cmd {
  CmdDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
          uint32_t first_instance)
      : Cmd(kDraw),
        vertex_count(vertex_count),
        instance_count(instance_count),
        first_vertex(first_vertex),
        first_instance(first_instance) {}

  // Functions for getting a reference to this instance with correct type.
  CmdDraw* AsDraw() override;
  [[nodiscard]] const CmdDraw* AsDraw() const override;

  uint32_t vertex_count;
  uint32_t instance_count;
  uint32_t first_vertex;
  uint32_t first_instance;
};

struct CmdDrawIndexed : public Cmd {
  CmdDrawIndexed(uint32_t index_count, uint32_t instance_count,
                 uint32_t first_index, int32_t vertex_offset,
                 uint32_t first_instance)
      : Cmd(kDrawIndexed),
        index_count(index_count),
        instance_count(instance_count),
        first_index(first_index),
        vertex_offset(vertex_offset),
        first_instance(first_instance) {}

  // Functions for getting a reference to this instance with correct type.
  CmdDrawIndexed* AsDrawIndexed() override;
  [[nodiscard]] const CmdDrawIndexed* AsDrawIndexed() const override;

  uint32_t index_count;
  uint32_t instance_count;
  uint32_t first_index;
  int32_t vertex_offset;
  uint32_t first_instance;
};

}  // namespace gf_layers::amber_scoop_layer

#pragma clang diagnostic pop

#endif  // VKLAYER_GF_AMBER_SCOOP_VULKAN_COMMANDS_H
