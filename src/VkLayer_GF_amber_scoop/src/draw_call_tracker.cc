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

#include <atomic>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

#include "VkLayer_GF_amber_scoop/amber_scoop_layer.h"
#include "gf_layers_layer_util/logging.h"

namespace gf_layers::amber_scoop_layer {

void DrawCallTracker::HandleDrawCall(uint32_t first_index, uint32_t index_count,
                                     uint32_t first_vertex,
                                     uint32_t vertex_count,
                                     uint32_t first_instance,
                                     uint32_t instance_count) {
  // Silence unused parameter warnings.
  // TODO(ilkkasaa) remove these when they are used.
  (void)first_vertex;
  (void)vertex_count;

  // Graphics pipeline must be bound.
  DEBUG_ASSERT(draw_call_state_.graphics_pipeline);
  DEBUG_ASSERT(draw_call_state_.current_render_pass);

  const uint64_t current_draw_call = global_data_->current_draw_call;
  // Return if current draw call should not be captured.
  if (current_draw_call < global_data_->settings.start_draw_call ||
      current_draw_call > global_data_->settings.last_draw_call) {
    global_data_->current_draw_call++;
    return;
  }
  global_data_->current_draw_call++;

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
  // TODO(ilkkasaa): add vertex shader spirv here
  amber_file << "TODO" << std::endl;
  amber_file << "END" << std::endl << std::endl;

  amber_file << "SHADER fragment fragment_shader SPIRV-ASM" << std::endl;
  // TODO(ilkkasaa): add fragment shader spirv here
  amber_file << "TODO" << std::endl;
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
