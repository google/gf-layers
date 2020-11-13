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
#include <vector>

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
  Cmd();
  virtual ~Cmd();

  // Disabled copy/move constructors and copy/move assign operators
  Cmd(const Cmd&) = delete;
  Cmd(Cmd&&) = delete;
  Cmd& operator=(const Cmd&) = delete;
  Cmd& operator=(Cmd&&) = delete;

  virtual bool IsDrawCall() { return false; }

  // Overriding function should either update the given draw call tracker (most
  // of the commands) or generate an Amber file (draw commands).
  virtual void ProcessSubmittedCommand(DrawCallTracker*) = 0;
};

class CmdBeginRenderPass : public Cmd {
 public:
  CmdBeginRenderPass(VkRenderPassBeginInfo const* renderpass_begin,
                     VkSubpassContents contents)
      : render_pass_begin_(DeepCopy(renderpass_begin)), contents_(contents) {}

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

class CmdBindIndexBuffer : public Cmd {
 public:
  CmdBindIndexBuffer(VkBuffer buffer, VkDeviceSize offset,
                     VkIndexType index_type)
      : buffer_(buffer), offset_(offset), index_type_(index_type) {}

  // Sets the index buffer binding.
  void ProcessSubmittedCommand(
      DrawCallTracker* draw_call_state_tracker) override;

 private:
  VkBuffer buffer_;
  VkDeviceSize offset_;
  VkIndexType index_type_;
};

class CmdBindPipeline : public Cmd {
 public:
  CmdBindPipeline(VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline)
      : pipeline_bind_point_(pipeline_bind_point), pipeline_(pipeline) {}

  // Updates the bound pipeline.
  void ProcessSubmittedCommand(
      DrawCallTracker* draw_call_state_tracker) override;

 private:
  VkPipelineBindPoint pipeline_bind_point_;
  VkPipeline pipeline_;
};

class CmdBindVertexBuffers : public Cmd {
 public:
  CmdBindVertexBuffers(uint32_t first_binding, uint32_t binding_count,
                       const VkBuffer* buffers, const VkDeviceSize* offsets)
      : first_binding_(first_binding),
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        buffers_(buffers, buffers + binding_count),
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        offsets_(offsets, offsets + binding_count) {}

  // Sets vertex buffer bindings and their offsets.
  void ProcessSubmittedCommand(
      DrawCallTracker* draw_call_state_tracker) override;

 private:
  uint32_t first_binding_;
  std::vector<VkBuffer> buffers_;
  std::vector<VkDeviceSize> offsets_;
};

class CmdDraw : public Cmd {
 public:
  CmdDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
          uint32_t first_instance)
      : vertex_count_(vertex_count),
        instance_count_(instance_count),
        first_vertex_(first_vertex),
        first_instance_(first_instance) {}

  bool IsDrawCall() override { return true; }

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
      : index_count_(index_count),
        instance_count_(instance_count),
        first_index_(first_index),
        vertex_offset_(vertex_offset),
        first_instance_(first_instance) {}

  bool IsDrawCall() override { return true; }

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

class CmdPipelineBarrier : public Cmd {
 public:
  CmdPipelineBarrier(VkPipelineStageFlags src_stage_mask,
                     VkPipelineStageFlags dst_stage_mask,
                     VkDependencyFlags dependency_flags,
                     uint32_t memory_barrier_count,
                     const VkMemoryBarrier* memory_barriers,
                     uint32_t buffer_memory_barrier_count,
                     const VkBufferMemoryBarrier* buffer_memory_barriers,
                     uint32_t image_memory_barrier_count,
                     const VkImageMemoryBarrier* image_memory_barriers)
      : src_stage_mask_(src_stage_mask),
        dst_stage_mask_(dst_stage_mask),
        dependency_flags_(dependency_flags) {
    // Copy the barriers to vectors for easier access.
    if (memory_barrier_count > 0) {
      memory_barriers_.assign(
          memory_barriers,
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
          memory_barriers + memory_barrier_count);
    }
    if (buffer_memory_barrier_count > 0) {
      buffer_memory_barriers_.assign(
          buffer_memory_barriers,
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
          buffer_memory_barriers + buffer_memory_barrier_count);
    }
    if (image_memory_barrier_count > 0) {
      image_memory_barriers_.assign(
          image_memory_barriers,
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
          image_memory_barriers + image_memory_barrier_count);
    }
  }

  // Sets the index buffer binding.
  void ProcessSubmittedCommand(
      DrawCallTracker* draw_call_state_tracker) override;

  // Getters for the member fields.

  [[nodiscard]] VkPipelineStageFlags GetSrcStageMask() const {
    return src_stage_mask_;
  }
  [[nodiscard]] VkPipelineStageFlags GetDstStageMask() const {
    return dst_stage_mask_;
  }
  [[nodiscard]] VkDependencyFlags GetDependencyFlags() const {
    return dependency_flags_;
  }
  [[nodiscard]] const std::vector<VkMemoryBarrier>& GetMemoryBarriers() const {
    return memory_barriers_;
  }
  [[nodiscard]] const std::vector<VkBufferMemoryBarrier>&
  GetBufferMemoryBarriers() const {
    return buffer_memory_barriers_;
  }
  [[nodiscard]] const std::vector<VkImageMemoryBarrier>&
  GetImageMemoryBarriers() const {
    return image_memory_barriers_;
  }

 private:
  VkPipelineStageFlags src_stage_mask_;
  VkPipelineStageFlags dst_stage_mask_;
  VkDependencyFlags dependency_flags_;
  std::vector<VkMemoryBarrier> memory_barriers_;
  std::vector<VkBufferMemoryBarrier> buffer_memory_barriers_;
  std::vector<VkImageMemoryBarrier> image_memory_barriers_;
};

#pragma clang diagnostic pop

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_VULKAN_COMMANDS_H
