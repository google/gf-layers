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

#include <atomic>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "VkLayer_GF_amber_scoop/amber_scoop_layer.h"
#include "VkLayer_GF_amber_scoop/create_info_wrapper.h"
#include "VkLayer_GF_amber_scoop/graphics_pipeline_data.h"
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
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  uint32_t version_word = create_info.pCode[1];
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

  // Convert code size to words.
  const auto code_size_in_words =
      static_cast<uint32_t>(create_info.codeSize) / 4;

  std::vector<uint32_t> binary(
      create_info.pCode,
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      create_info.pCode + code_size_in_words);

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

  for (uint32_t stageIndex = 0;
       stageIndex < graphics_pipeline_data->GetCreateInfo().stageCount;
       stageIndex++) {
    const VkPipelineShaderStageCreateInfo& stage_create_info =
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        graphics_pipeline_data->GetCreateInfo().pStages[stageIndex];
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
  // TODO(ilkkasaa): add other string streams

  buffer_declaration_str << "BUFFER ...";

  std::string amber_file_name = global_data_->settings.output_file_prefix +
                                "_" + std::to_string(current_draw_call) +
                                ".amber";

  std::ofstream amber_file;
  amber_file.open(amber_file_name, std::ios::trunc | std::ios::out);

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

  // Pipeline
  amber_file << "PIPELINE graphics pipeline" << std::endl;
  amber_file << "  ATTACH vertex_shader";
  amber_file << std::endl;
  amber_file << "  ATTACH fragment_shader";
  // TODO(ilkkasaa): add other pipeline contents here
  amber_file << "END" << std::endl << std::endl;  // PIPELINE

  // TODO(ilkkasaa): get primitive topology from VkGraphicsPipelineCreateInfo
  const std::string topology = "TODO";

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
}

}  // namespace gf_layers::amber_scoop_layer
