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

#ifndef VK_LAYER_AMBER_SCOOP_VULKAN_COMMANDS_H
#define VK_LAYER_AMBER_SCOOP_VULKAN_COMMANDS_H

#include <vulkan/vulkan.h>

#include <cstdint>

#include "vk_layer_amber_scoop/vk_deep_copy.h"

namespace gf_layers::amber_scoop_layer {

class CmdBeginRenderPass;
class CmdBindPipeline;
class CmdDraw;
class CmdDrawIndexed;

class Cmd {
 public:
  enum Kind {
    kBeginRenderPass,
    kBindPipeline,
    kDraw,
    kDrawIndexed,
  };

  explicit Cmd(Kind kind) : kind_(kind) {}

  virtual ~Cmd();

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

  Kind kind_;
  // clang-format on

  // Disabled copy/move constructors and copy/move assign operators
  Cmd(const Cmd&) = delete;
  Cmd(Cmd&&) = delete;
  Cmd& operator=(const Cmd&) = delete;
  Cmd& operator=(Cmd&&) = delete;
};

class CmdBeginRenderPass : public Cmd {
 public:
  CmdBeginRenderPass(VkRenderPassBeginInfo const* pRenderPassBegin,
                     VkSubpassContents contents)
      : Cmd(kBeginRenderPass),
        render_pass_begin_(DeepCopy(pRenderPassBegin)),
        contents_(contents) {}

  CmdBeginRenderPass* AsBeginRenderPass() override;
  [[nodiscard]] const CmdBeginRenderPass* AsBeginRenderPass() const override;

  ~CmdBeginRenderPass() override { DeepDelete(&render_pass_begin_); }

  VkRenderPassBeginInfo render_pass_begin_;
  VkSubpassContents contents_;

  // Disabled copy/move constructors and copy/move assign operators
  CmdBeginRenderPass(const CmdBeginRenderPass&) = delete;
  CmdBeginRenderPass(CmdBeginRenderPass&&) = delete;
  CmdBeginRenderPass& operator=(const CmdBeginRenderPass&) = delete;
  CmdBeginRenderPass& operator=(CmdBeginRenderPass&&) = delete;
};

class CmdBindPipeline : public Cmd {
 public:
  CmdBindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
      : Cmd(kBindPipeline),
        pipeline_bind_point_(pipelineBindPoint),
        pipeline_(pipeline) {}

  CmdBindPipeline* AsBindPipeline() override;
  [[nodiscard]] const CmdBindPipeline* AsBindPipeline() const override;

  VkPipelineBindPoint pipeline_bind_point_;
  VkPipeline pipeline_;
};

class CmdDraw : public Cmd {
 public:
  CmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
          uint32_t firstInstance)
      : Cmd(kDraw),
        vertex_count_(vertexCount),
        instance_count_(instanceCount),
        first_vertex_(firstVertex),
        first_instance_(firstInstance) {}

  CmdDraw* AsDraw() override;
  [[nodiscard]] const CmdDraw* AsDraw() const override;

  uint32_t vertex_count_;
  uint32_t instance_count_;
  uint32_t first_vertex_;
  uint32_t first_instance_;
};

class CmdDrawIndexed : public Cmd {
 public:
  CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                 uint32_t firstIndex, int32_t vertexOffset,
                 uint32_t firstInstance)
      : Cmd(kDrawIndexed),
        index_count_(indexCount),
        instance_count_(instanceCount),
        first_index_(firstIndex),
        vertex_offset_(vertexOffset),
        first_instance_(firstInstance) {}

  CmdDrawIndexed* AsDrawIndexed() override;
  [[nodiscard]] const CmdDrawIndexed* AsDrawIndexed() const override;

  uint32_t index_count_;
  uint32_t instance_count_;
  uint32_t first_index_;
  int32_t vertex_offset_;
  uint32_t first_instance_;
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VK_LAYER_AMBER_SCOOP_VULKAN_COMMANDS_H
