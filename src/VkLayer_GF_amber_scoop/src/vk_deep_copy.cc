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

VkGraphicsPipelineCreateInfo DeepCopy(
    const VkGraphicsPipelineCreateInfo& createInfo) {
  VkGraphicsPipelineCreateInfo result = createInfo;
  // Copy pStages
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* newStages =
        new VkPipelineShaderStageCreateInfo[createInfo.stageCount];

    for (uint32_t i = 0; i < createInfo.stageCount; i++) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      newStages[i] = DeepCopy(createInfo.pStages[i]);
    }
    result.pStages = newStages;
  }

  // Copy pVertexInputState
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* vertexInputState = new VkPipelineVertexInputStateCreateInfo();
    *vertexInputState = *createInfo.pVertexInputState;
    // Copy pVertexBindingDescriptions
    {
      vertexInputState->pVertexBindingDescriptions =
          CopyArray(createInfo.pVertexInputState->pVertexBindingDescriptions,
                    vertexInputState->vertexBindingDescriptionCount);
    }
    // Copy pVertexAttributeDescriptions
    {
      vertexInputState->pVertexAttributeDescriptions = CopyArray(
          createInfo.pVertexInputState->pVertexAttributeDescriptions,
          createInfo.pVertexInputState->vertexAttributeDescriptionCount);
    }
    result.pVertexInputState = vertexInputState;
  }

  // Copy pInputAssemblyState
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* inputAssemblyStateCreateInfo =
        new VkPipelineInputAssemblyStateCreateInfo();
    *inputAssemblyStateCreateInfo = *createInfo.pInputAssemblyState;
    result.pInputAssemblyState = inputAssemblyStateCreateInfo;
  }

  // Copy pRasterizationState
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* rasterizationState = new VkPipelineRasterizationStateCreateInfo();
    *rasterizationState = *createInfo.pRasterizationState;
    result.pRasterizationState = rasterizationState;
  }

  // Copy pDepthStencilState
  if (createInfo.pDepthStencilState != nullptr) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* depthStencilState = new VkPipelineDepthStencilStateCreateInfo();
    *depthStencilState = *createInfo.pDepthStencilState;
    result.pDepthStencilState = depthStencilState;
  }

  // TODO(ilkkasaa): handle deep copying of other fields.
  return result;
}

void DeepDelete(const VkGraphicsPipelineCreateInfo& create_info) {
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
