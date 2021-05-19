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

#include "VkLayer_GF_amber_scoop/vk_deep_copy.h"

#include <cstring>

namespace gf_layers::amber_scoop_layer {

VkBufferCreateInfo DeepCopy(const VkBufferCreateInfo& create_info) {
  VkBufferCreateInfo result = create_info;
  result.pQueueFamilyIndices = CopyArray(create_info.pQueueFamilyIndices,
                                         create_info.queueFamilyIndexCount);
  return result;
}

void DeepDelete(const VkBufferCreateInfo& create_info) {
  if (create_info.queueFamilyIndexCount > 0) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete[] create_info.pQueueFamilyIndices;
  }
}

VkDescriptorSetLayoutBinding DeepCopy(
    const VkDescriptorSetLayoutBinding& descriptor_set_layout_binding) {
  VkDescriptorSetLayoutBinding result = descriptor_set_layout_binding;

  if (descriptor_set_layout_binding.pImmutableSamplers == nullptr) {
    return result;
  }

  // Copy immutable sampler handles.
  result.pImmutableSamplers =
      CopyArray(descriptor_set_layout_binding.pImmutableSamplers,
                descriptor_set_layout_binding.descriptorCount);

  return result;
}

void DeepDelete(const VkDescriptorSetLayoutBinding& binding) {
  // Delete immutable sampler handles.
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] binding.pImmutableSamplers;
}

VkDescriptorSetLayoutCreateInfo DeepCopy(
    const VkDescriptorSetLayoutCreateInfo& create_info) {
  VkDescriptorSetLayoutCreateInfo result = create_info;
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  VkDescriptorSetLayoutBinding* bindings;
  bindings = create_info.bindingCount > 0
                 ? new VkDescriptorSetLayoutBinding[create_info.bindingCount]
                 : nullptr;

  for (uint32_t i = 0; i < create_info.bindingCount; i++) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    bindings[i] = DeepCopy(create_info.pBindings[i]);
  }
  result.pBindings = bindings;
  return result;
}

void DeepDelete(const VkDescriptorSetLayoutCreateInfo& create_info) {
  for (uint32_t i = 0; i < create_info.bindingCount; i++) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    DeepDelete(create_info.pBindings[i]);
  }
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] create_info.pBindings;
}

VkGraphicsPipelineCreateInfo DeepCopy(
    const VkGraphicsPipelineCreateInfo& create_info) {
  VkGraphicsPipelineCreateInfo result = create_info;
  // Copy pStages
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* new_stages =
        new VkPipelineShaderStageCreateInfo[create_info.stageCount];

    for (uint32_t i = 0; i < create_info.stageCount; i++) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      new_stages[i] = DeepCopy(create_info.pStages[i]);
    }
    result.pStages = new_stages;
  }

  // Copy pVertexInputState
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* vertex_input_state = new VkPipelineVertexInputStateCreateInfo();
    *vertex_input_state = *create_info.pVertexInputState;
    // Copy pVertexBindingDescriptions
    {
      vertex_input_state->pVertexBindingDescriptions =
          CopyArray(create_info.pVertexInputState->pVertexBindingDescriptions,
                    vertex_input_state->vertexBindingDescriptionCount);
    }
    // Copy pVertexAttributeDescriptions
    {
      vertex_input_state->pVertexAttributeDescriptions = CopyArray(
          create_info.pVertexInputState->pVertexAttributeDescriptions,
          create_info.pVertexInputState->vertexAttributeDescriptionCount);
    }
    result.pVertexInputState = vertex_input_state;
  }

  // Copy pInputAssemblyState
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* input_assembly_create_info =
        new VkPipelineInputAssemblyStateCreateInfo();
    *input_assembly_create_info = *create_info.pInputAssemblyState;
    result.pInputAssemblyState = input_assembly_create_info;
  }

  // Copy pRasterizationState
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* rasterization_state_create_info =
        new VkPipelineRasterizationStateCreateInfo();
    *rasterization_state_create_info = *create_info.pRasterizationState;
    result.pRasterizationState = rasterization_state_create_info;
  }

  // Copy pDepthStencilState
  if (create_info.pDepthStencilState != nullptr) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* depth_stencil_state_create_info =
        new VkPipelineDepthStencilStateCreateInfo();
    *depth_stencil_state_create_info = *create_info.pDepthStencilState;
    result.pDepthStencilState = depth_stencil_state_create_info;
  }

  // TODO(ilkkasaa): handle deep copying of other fields.
  return result;
}

void DeepDelete(const VkGraphicsPipelineCreateInfo& create_info) {
  for (uint32_t i = 0; i < create_info.stageCount; i++) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    DeepDelete(create_info.pStages[i]);
  }
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] create_info.pStages;

  if (create_info.pVertexInputState != nullptr) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete[] create_info.pVertexInputState->pVertexBindingDescriptions;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete[] create_info.pVertexInputState->pVertexAttributeDescriptions;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete create_info.pVertexInputState;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete create_info.pInputAssemblyState;
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete create_info.pRasterizationState;
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete create_info.pDepthStencilState;
}

VkPipelineLayoutCreateInfo DeepCopy(
    const VkPipelineLayoutCreateInfo& create_info) {
  VkPipelineLayoutCreateInfo result = create_info;
  result.pSetLayouts =
      CopyArray(create_info.pSetLayouts, create_info.setLayoutCount);
  result.pPushConstantRanges = CopyArray(create_info.pPushConstantRanges,
                                         create_info.pushConstantRangeCount);
  return result;
}

void DeepDelete(const VkPipelineLayoutCreateInfo& create_info) {
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] create_info.pSetLayouts;
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] create_info.pPushConstantRanges;
}

VkPipelineShaderStageCreateInfo DeepCopy(
    const VkPipelineShaderStageCreateInfo& create_info) {
  VkPipelineShaderStageCreateInfo result = create_info;

  // Deep copy fields from specialization info struct.
  if (create_info.pSpecializationInfo != nullptr) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* specialization_info = new VkSpecializationInfo();
    *specialization_info = *create_info.pSpecializationInfo;
    specialization_info->pMapEntries =
        CopyArray(create_info.pSpecializationInfo->pMapEntries,
                  create_info.pSpecializationInfo->mapEntryCount);
    specialization_info->pData = CopyArray(
        static_cast<const char*>(create_info.pSpecializationInfo->pData),
        create_info.pSpecializationInfo->dataSize);
    result.pSpecializationInfo = specialization_info;
  }
  return result;
}

void DeepDelete(const VkPipelineShaderStageCreateInfo& create_info) {
  if (create_info.pSpecializationInfo != nullptr) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete[] create_info.pSpecializationInfo->pMapEntries;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete[] static_cast<const char*>(create_info.pSpecializationInfo->pData);
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete create_info.pSpecializationInfo;
  }
}

VkRenderPassBeginInfo DeepCopy(
    VkRenderPassBeginInfo const* p_render_pass_begin_info) {
  VkRenderPassBeginInfo result = *p_render_pass_begin_info;
  result.pClearValues = CopyArray(p_render_pass_begin_info->pClearValues,
                                  p_render_pass_begin_info->clearValueCount);
  return result;
}

void DeepDelete(VkRenderPassBeginInfo const* p_render_pass_begin_info) {
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] p_render_pass_begin_info->pClearValues;
}

VkShaderModuleCreateInfo DeepCopy(const VkShaderModuleCreateInfo& create_info) {
  VkShaderModuleCreateInfo result = create_info;
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  auto* new_code = new uint32_t[create_info.codeSize];
  memcpy(new_code, create_info.pCode, create_info.codeSize);
  result.pCode = new_code;
  return result;
}

void DeepDelete(const VkShaderModuleCreateInfo& create_info) {
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] create_info.pCode;
}

}  // namespace gf_layers::amber_scoop_layer
