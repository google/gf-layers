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
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "VkLayer_GF_amber_scoop/amber_scoop_layer.h"
#include "VkLayer_GF_amber_scoop/buffer_copy.h"
#include "VkLayer_GF_amber_scoop/buffer_to_file.h"
#include "VkLayer_GF_amber_scoop/command_buffer_data.h"
#include "VkLayer_GF_amber_scoop/create_info_wrapper.h"
#include "VkLayer_GF_amber_scoop/graphics_pipeline_data.h"
#include "VkLayer_GF_amber_scoop/vulkan_formats.h"
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
  std::ostringstream pipeline_str;
  // TODO(ilkkasaa): add other string streams

  // Pipeline
  pipeline_str << "PIPELINE graphics pipeline" << std::endl;
  pipeline_str << "  ATTACH vertex_shader" << std::endl;
  pipeline_str << "  ATTACH fragment_shader" << std::endl;

  // Highest index value (if indices are used). Updated in
  // |CreateIndexBufferDeclarations| function.
  uint32_t max_index_value = 0;

  CreateIndexBufferDeclarations(device_data, index_count, &max_index_value,
                                buffer_declaration_str, pipeline_str);

  CreateVertexBufferDeclarations(device_data, vertex_count, first_instance,
                                 instance_count, max_index_value,
                                 buffer_declaration_str, pipeline_str);

  // Add frame buffer that can be exported to PNG.
  buffer_declaration_str << "BUFFER framebuffer FORMAT B8G8R8A8_UNORM"
                         << std::endl
                         << std::endl;
  pipeline_str << "  BIND BUFFER framebuffer AS color LOCATION 0" << std::endl;

  // End pipeline
  pipeline_str << "END" << std::endl << std::endl;

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
  amber_file << pipeline_str.str();

  // TODO(ilkkasaa): get primitive topology from VkGraphicsPipelineCreateInfo
  const std::string topology = "TRIANGLE_LIST";

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
    DeviceData* device_data, uint32_t index_count, uint32_t* max_index_value,
    std::ostringstream& declaration_string_stream,
    std::ostringstream& pipeline_string_stream) const {
  VkBuffer index_buffer = draw_call_state_.bound_index_buffer.buffer;
  BufferData* buffer_create_info = device_data->buffers.Get(index_buffer);

  // Copy the buffer.
  const VkDeviceSize& index_buffer_size =
      buffer_create_info->GetCreateInfo().size;
  auto index_buffer_copy =
      BufferCopy(device_data, index_buffer, index_buffer_size,
                 draw_call_state_.queue, GetCommandPool(device_data));

  // Create index buffer declaration string.

  // Amber supports only 32-bit indices. 16-bit indices will be used as
  // 32-bit.
  declaration_string_stream << "BUFFER index_buffer DATA_TYPE uint32 ";
  declaration_string_stream << "DATA " << std::endl << "  ";

  // Index data starts from the buffer beginning + offset.
  const char* first_index_address =
      &index_buffer_copy
           .GetCopiedData()[draw_call_state_.bound_index_buffer.offset];

  if (draw_call_state_.bound_index_buffer.index_type == VK_INDEX_TYPE_UINT16) {
    // 16-bit indices
    // Create a new span with type of |uint16_t| so the indices can be read
    // easily.
    absl::Span<const uint16_t> index_data = absl::MakeConstSpan(
        reinterpret_cast<const uint16_t*>(first_index_address), index_count);
    // Append values to the index buffer string.
    for (uint16_t index : index_data) {
      *max_index_value =
          std::max(*max_index_value, static_cast<uint32_t>(index));
      declaration_string_stream << index << " ";
    }
  } else if (draw_call_state_.bound_index_buffer.index_type ==
             VK_INDEX_TYPE_UINT32) {
    // 32-bit indices
    // Create a new span with type of |uint32_t| so the indices can be read
    // easily.
    absl::Span<const uint32_t> index_data = absl::MakeConstSpan(
        reinterpret_cast<const uint32_t*>(first_index_address), index_count);
    // Append values to the index buffer string.
    for (uint32_t index : index_data) {
      *max_index_value = std::max(*max_index_value, index);
      declaration_string_stream << index << " ";
    }
  } else {
    LOG("Index type not supported.");
    RUNTIME_ASSERT(false);
  }
  declaration_string_stream << std::endl << "END" << std::endl << std::endl;

  // Use indices in the pipeline.
  pipeline_string_stream << "  INDEX_DATA index_buffer" << std::endl;
}

bool DrawCallTracker::CreateVertexBufferDeclarations(
    DeviceData* device_data, uint32_t vertex_count, uint32_t first_instance,
    uint32_t instance_count, uint32_t max_index_value,
    std::ostringstream& buffer_declaration_str,
    std::ostringstream& pipeline_str) {
  bool vertex_buffer_found = false;
  // Map of copied |VkBuffer|s to avoid copying same buffer multiple times.
  std::unordered_map<VkBuffer, std::unique_ptr<BufferCopy>> copied_buffers;

  const VkGraphicsPipelineCreateInfo& graphics_pipeline_create_info =
      device_data->graphics_pipelines.Get(draw_call_state_.graphics_pipeline)
          ->get()
          ->GetCreateInfo();

  const VkPipelineVertexInputStateCreateInfo& vertex_input_state =
      *graphics_pipeline_create_info.pVertexInputState;

  // Create a list of pipeline barriers that should be used to synchronize
  // the access to index buffer.
  std::vector<const CmdPipelineBarrier*> pipeline_barriers;

  // Make span for attribute and binding descriptions for safer access.
  absl::Span<const VkVertexInputAttributeDescription> attribute_descriptions =
      absl::MakeConstSpan<>(vertex_input_state.pVertexAttributeDescriptions,
                            vertex_input_state.vertexAttributeDescriptionCount);
  absl::Span<const VkVertexInputBindingDescription> binding_descriptions =
      absl::MakeConstSpan<>(vertex_input_state.pVertexBindingDescriptions,
                            vertex_input_state.vertexBindingDescriptionCount);

  // Go through all attribute descriptions to get the pipeline's vertex buffer
  // bindings. First copy the whole |VkBuffer| into a host visible memory.
  // Copied buffers are stored temporarily in |copied_buffers| map to prevent
  // the same buffer being copied multiple times if it's used in more than one
  // binding.
  for (const auto& attribute_description : attribute_descriptions) {
    // Find description for the binding used in this vertex attribute.
    const auto* binding_description = std::find_if(
        binding_descriptions.begin(), binding_descriptions.end(),
        [&](const VkVertexInputBindingDescription& _binding_description) {
          return _binding_description.binding == attribute_description.binding;
        });

    // Verify that a binding description exists.
    if (binding_description == nullptr) {
      LOG("Unable to find |VkVertexInputBindingDescription| for binding [%i] "
          "in |VkVertexInputAttributeDescription| at location [%i]",
          attribute_description.binding, attribute_description.location);
      RUNTIME_ASSERT(false);
    }

    // Get Vulkan handle and create info for the buffer.
    const VertexBufferBinding& vertex_buffer =
        draw_call_state_.bound_vertex_buffers.at(attribute_description.binding);
    const VkBufferCreateInfo& buffer_create_info =
        device_data->buffers.Get(vertex_buffer.buffer)->GetCreateInfo();

    // Usage bits should contain |VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|
    DEBUG_ASSERT(
        (buffer_create_info.usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) != 0);

    // Check if the buffer has been copied already, i.e. it is stored in the
    // |copied_buffers| map.
    if (copied_buffers[vertex_buffer.buffer].get() == nullptr) {
      std::unique_ptr<BufferCopy> vertex_buffer_copy =
          std::make_unique<BufferCopy>(
              device_data, vertex_buffer.buffer, buffer_create_info.size,
              draw_call_state_.queue, GetCommandPool(device_data));

      copied_buffers[vertex_buffer.buffer] = std::move(vertex_buffer_copy);
    }
    const BufferCopy* vertex_buffer_copy =
        copied_buffers[vertex_buffer.buffer].get();

    std::string buffer_name = "vert_";
    buffer_name.append(std::to_string(attribute_description.location));

    VulkanFormat format = VulkanFormat(attribute_description.format);

    std::string input_rate_str;
    const VertexBufferBinding& vertex_buffer_binding =
        draw_call_state_.bound_vertex_buffers.at(attribute_description.binding);

    // |element_count| is number of elements of type
    // |VkVertexInputAttributeDescription.format| to be read from the buffer.
    // The element count depends on the binding's |inputRate| and if this is an
    // instanced draw.
    uint32_t element_count = 0;
    if (binding_description->inputRate == VK_VERTEX_INPUT_RATE_VERTEX) {
      // If indices are used, we need to copy all the values up to the greatest
      // value of the indices (so the |element_count| is the greatest index
      // value +1). Otherwise we can use the number of vertices passed from the
      // draw command.
      element_count = vertex_count == 0 ? max_index_value + 1 : vertex_count;
      input_rate_str = "vertex";
    } else if (binding_description->inputRate ==
               VK_VERTEX_INPUT_RATE_INSTANCE) {
      // Copy all values starting from instance 0 even if the first instance is
      // greater than 0. This makes the draw call more similar to the original
      // draw call.
      element_count = first_instance + instance_count;
      input_rate_str = "instance";
    } else {
      LOG("Invalid vertex input rate.");
      RUNTIME_ASSERT(false);
    }

    std::string buffer_file_name = global_data_->settings.output_file_prefix;
    buffer_file_name.append("_").append(buffer_name).append(".bin");
    BufferToFile buffer_to_file(buffer_file_name);

    // Init the byte offset |offset| with sum of offsets defined in
    // |VkVertexInputAttributeDescription.offset| and
    // |vkCmdBindVertexBuffers.pOffsets|.
    VkDeviceSize offset =
        attribute_description.offset + vertex_buffer_binding.offset;

    const absl::Span<const char> copied_data =
        vertex_buffer_copy->GetCopiedData();

    // Write data elements to the file.
    for (uint32_t i = 0; i < element_count; i++) {
      buffer_to_file.WriteComponents(copied_data, offset, format);
      // Increase the offset by the value of stride.
      offset += binding_description->stride;
    }

    buffer_declaration_str << "BUFFER " << buffer_name << " DATA_TYPE "
                           << format.GetName() << " SIZE " << element_count
                           << " FILE BINARY " << buffer_file_name << std::endl;

    pipeline_str << "  VERTEX_DATA " << buffer_name << " LOCATION "
                 << attribute_description.location << " RATE " << input_rate_str
                 << std::endl;

    vertex_buffer_found = true;
  }
  return vertex_buffer_found;
}

VkCommandPool DrawCallTracker::GetCommandPool(DeviceData* device_data) const {
  CommandBufferData* command_buffer_data =
      device_data->command_buffers_data.Get(draw_call_state_.command_buffer);
  return command_buffer_data->GetAllocateInfo()->commandPool;
}

}  // namespace gf_layers::amber_scoop_layer
