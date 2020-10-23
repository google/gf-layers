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

#include "VkLayer_GF_amber_scoop/draw_call_tracker.h"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <atomic>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>

#include "VkLayer_GF_amber_scoop/amber_scoop_layer.h"
#include "VkLayer_GF_amber_scoop/buffer_copy.h"
#include "VkLayer_GF_amber_scoop/command_buffer_data.h"
#include "VkLayer_GF_amber_scoop/create_info_wrapper.h"
#include "VkLayer_GF_amber_scoop/graphics_pipeline_data.h"
#include "absl/types/span.h"
#include "gf_layers_layer_util/logging.h"
#include "gf_layers_layer_util/spirv.h"
#include "gf_layers_layer_util/util.h"

#pragma warning(push, 1)  // MSVC: reduces warning level to W1.

#include "spirv-tools/libspirv.h"
#include "spirv-tools/libspirv.hpp"

#pragma warning(pop)

namespace gf_layers::amber_scoop_layer {

namespace {

std::string DisassembleShaderModule(
    const VkShaderModuleCreateInfo& create_info) {
  // Get SPIR-V shader module version.

  // Get a span containing the shader code. |create_info.codeSize| gives the
  // size in bytes, so we convert it to words.
  absl::Span<const uint32_t> code =
      absl::MakeConstSpan(create_info.pCode, create_info.codeSize / 4);

  uint32_t version_word = code[1];

  uint8_t major_version = GetSpirvVersionMajorPart(version_word);
  uint8_t minor_version = GetSpirvVersionMinorPart(version_word);

  if (major_version != 1) {
    LOG("Unknown SPIR-V major version %u", major_version);
    RUNTIME_ASSERT(false);
  }

  spv_target_env target_env;  // NOLINT(cppcoreguidelines-init-variables)
  switch (minor_version) {
    case 0:
      target_env = SPV_ENV_UNIVERSAL_1_0;
      break;
    case 1:
      target_env = SPV_ENV_UNIVERSAL_1_1;
      break;
    case 2:
      target_env = SPV_ENV_UNIVERSAL_1_2;
      break;
    case 3:
      target_env = SPV_ENV_UNIVERSAL_1_3;
      break;
    case 4:
      target_env = SPV_ENV_UNIVERSAL_1_4;
      break;
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    case 5:
      target_env = SPV_ENV_UNIVERSAL_1_5;
      break;
    default:
      LOG("Unknown SPIR-V minor version %u", minor_version);
      RUNTIME_ASSERT(false);
  }

  spvtools::SpirvTools tools(target_env);
  if (!tools.IsValid()) {
    LOG("Failed to instantiate SpirvTools object.");
    RUNTIME_ASSERT(false);
  }

  std::vector<uint32_t> binary(code.begin(), code.end());

  std::string disassembly;
  tools.Disassemble(binary, &disassembly, SPV_BINARY_TO_TEXT_OPTION_INDENT);

  return disassembly;
}
}  // namespace

void DrawCallTracker::HandleDrawCall(uint32_t first_index, uint32_t index_count,
                                     uint32_t first_vertex,
                                     uint32_t vertex_count,
                                     uint32_t first_instance,
                                     uint32_t instance_count) {
  // Silence unused parameter warnings.
  // TODO(ilkkasaa): remove these when they are used.
  (void)first_vertex;
  (void)vertex_count;

  // Graphics pipeline must be bound.
  DEBUG_ASSERT(draw_call_state_.graphics_pipeline);
  DEBUG_ASSERT(draw_call_state_.current_render_pass);

  const uint64_t current_draw_call = global_data_->current_draw_call++;

  // Return if current draw call should not be captured.
  if (current_draw_call < global_data_->settings.start_draw_call ||
      current_draw_call > global_data_->settings.last_draw_call) {
    return;
  }

  DeviceData* device_data =
      global_data_->device_map.Get(DeviceKey(draw_call_state_.queue))->get();

  const VkPipelineShaderStageCreateInfo* vertex_shader = nullptr;
  const VkPipelineShaderStageCreateInfo* fragment_shader = nullptr;
  const GraphicsPipelineData* graphics_pipeline_data =
      device_data->graphics_pipelines.Get(draw_call_state_.graphics_pipeline)
          ->get();

  for (const VkPipelineShaderStageCreateInfo& stage_create_info :
       absl::MakeConstSpan(
           graphics_pipeline_data->GetCreateInfo().pStages,
           graphics_pipeline_data->GetCreateInfo().stageCount)) {
    if (stage_create_info.stage == VK_SHADER_STAGE_VERTEX_BIT) {
      vertex_shader = &stage_create_info;
    } else if (stage_create_info.stage == VK_SHADER_STAGE_FRAGMENT_BIT) {
      fragment_shader = &stage_create_info;
    } else {
      LOG("Shader stage not handled.");
      RUNTIME_ASSERT(false);
    }
  }
  // Both vertex and fragment shaders are required.
  if (vertex_shader == nullptr || fragment_shader == nullptr) {
    LOG("Missing vertex or fragment shader.");
    RUNTIME_ASSERT(false);
  }

  // Initialize string streams for different parts of the amber file.
  std::ostringstream buffer_declaration_str;
  std::ostringstream pipeline_str_stream;
  // TODO(ilkkasaa): add other string streams

  // Pipeline
  pipeline_str_stream << "PIPELINE graphics pipeline" << std::endl;
  pipeline_str_stream << "  ATTACH vertex_shader" << std::endl;
  pipeline_str_stream << "  ATTACH fragment_shader" << std::endl;

  CreateIndexBufferDeclarations(device_data, index_count,
                                buffer_declaration_str, pipeline_str_stream);

  // End pipeline
  pipeline_str_stream << "END" << std::endl << std::endl;

  std::string amber_file_name = global_data_->settings.output_file_prefix +
                                "_" + std::to_string(current_draw_call) +
                                ".amber";

  // Start generating the Amber file.
  std::ofstream amber_file;
  amber_file.open(amber_file_name, std::ios::trunc | std::ios::out);

  // Add shader modules.
  amber_file << "#!amber" << std::endl << std::endl;
  amber_file << "SHADER vertex vertex_shader SPIRV-ASM" << std::endl;
  amber_file << DisassembleShaderModule(
                    graphics_pipeline_data
                        ->GetShaderModuleData(vertex_shader->module)
                        ->GetCreateInfo())
             << std::endl;
  amber_file << "END" << std::endl << std::endl;
  amber_file << "SHADER fragment fragment_shader SPIRV-ASM" << std::endl;
  amber_file << DisassembleShaderModule(
                    graphics_pipeline_data
                        ->GetShaderModuleData(fragment_shader->module)
                        ->GetCreateInfo())
             << std::endl;
  amber_file << "END" << std::endl << std::endl;

  // Append string streams to the Amber file.
  amber_file << buffer_declaration_str.str();
  amber_file << pipeline_str_stream.str();

  // TODO(ilkkasaa): get primitive topology from VkGraphicsPipelineCreateInfo
  const std::string topology = "TODO";

  // Add run commands.
  if (index_count > 0) {
    amber_file << "RUN pipeline DRAW_ARRAY AS " << topology
               << " INDEXED START_IDX " << first_index << " COUNT "
               << index_count;
  } else {
    amber_file << "RUN pipeline DRAW_ARRAY AS " << topology;
  }
  if (instance_count > 0) {
    amber_file << " START_INSTANCE " << first_instance << " INSTANCE_COUNT "
               << instance_count;
  }
  amber_file << std::endl;

  amber_file.close();
}

void DrawCallTracker::CreateIndexBufferDeclarations(
    DeviceData* device_data, uint32_t index_count,
    std::ostringstream& declaration_string_stream,
    std::ostringstream& pipeline_string_stream) {
  auto* index_buffer = draw_call_state_.bound_index_buffer.buffer;
  auto* buffer_create_info = device_data->buffers.Get(index_buffer);

  // Get the command pool used to create th current command buffer. Command pool
  // is used to create our own command buffer for copying the buffer contents.
  CommandBufferData* command_buffer_data =
      device_data->command_buffers_data.Get(draw_call_state_.command_buffer);
  auto* command_pool = command_buffer_data->GetAllocateInfo()->commandPool;

  // Create a list of pipeline barriers that should be used to synchronize
  // the access to index buffer.
  std::vector<const CmdPipelineBarrier*> pipeline_barriers;

  // List of stage flags we are interested in.
  static const VkPipelineStageFlags stages_flag_bits =
      // NOLINTNEXTLINE(hicpp-signed-bitwise)
      VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
      VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

  std::copy_if(draw_call_state_.pipeline_barriers.begin(),
               draw_call_state_.pipeline_barriers.end(),
               std::back_inserter(pipeline_barriers),
               [](const CmdPipelineBarrier* barrier) {
                 return ((barrier->GetDstStageMask() & stages_flag_bits) != 0);
               });

  const VkDeviceSize& index_buffer_size =
      buffer_create_info->GetCreateInfo().size;

  auto index_buffer_copy =
      BufferCopy(device_data, index_buffer, index_buffer_size,
                 draw_call_state_.queue, command_pool, pipeline_barriers);

  pipeline_string_stream << "  INDEX_DATA index_buffer" << std::endl;

  // Amber supports only 32-bit indices. 16-bit indices will be used as
  // 32-bit.
  declaration_string_stream << "BUFFER index_buffer DATA_TYPE uint32 ";
  declaration_string_stream << "DATA " << std::endl << "  ";

  if (draw_call_state_.bound_index_buffer.index_type == VK_INDEX_TYPE_UINT16) {
    // 16-bit indices
    absl::Span<const uint16_t> index_data = absl::MakeConstSpan<>(
        reinterpret_cast<const uint16_t*>(index_buffer_copy.GetCopiedData()),
        index_buffer_size);

    for (VkDeviceSize idx = draw_call_state_.bound_index_buffer.offset;
         idx < index_count; idx++) {
      declaration_string_stream << index_data[idx] << " ";
    }
  } else if (draw_call_state_.bound_index_buffer.index_type ==
             VK_INDEX_TYPE_UINT32) {
    // 32-bit indices
    absl::Span<const uint32_t> index_data = absl::MakeConstSpan<>(
        reinterpret_cast<const uint32_t*>(index_buffer_copy.GetCopiedData()),
        index_buffer_size);

    for (VkDeviceSize idx = draw_call_state_.bound_index_buffer.offset;
         idx < index_count; idx++) {
      declaration_string_stream << index_data[idx] << " ";
    }
  } else {
    LOG("Index type not supported.");
    RUNTIME_ASSERT(false);
  }
  declaration_string_stream << std::endl << "END" << std::endl << std::endl;
}

}  // namespace gf_layers::amber_scoop_layer
