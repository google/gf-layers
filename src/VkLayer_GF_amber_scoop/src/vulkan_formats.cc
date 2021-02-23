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

#include "VkLayer_GF_amber_scoop/vulkan_formats.h"

// <cstdio> and <cstdlib> required by LOG and ASSERT macros, but IWYU doesn't
// notice it, so we need to manually keep the includes.
#include <cstdio>   // IWYU pragma: keep
#include <cstdlib>  // IWYU pragma: keep

#include "gf_layers_layer_util/logging.h"

namespace gf_layers::amber_scoop_layer {

const char* VkFormatToAmberFormatName(VkFormat vk_format) {
  // Disable warning: "enumerator 'identifier' in switch of enum 'enumeration'
  // is not explicitly handled by a case label".
#pragma warhing(push)
#pragma warning(disable : 4061)
  switch (vk_format) {
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
      return "A1R5G5B5_UNORM_PACK16";
    case VK_FORMAT_A2B10G10R10_SINT_PACK32:
      return "A2B10G10R10_SINT_PACK32";
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
      return "A2B10G10R10_SNORM_PACK32";
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
      return "A2B10G10R10_SSCALED_PACK32";
    case VK_FORMAT_A2B10G10R10_UINT_PACK32:
      return "A2B10G10R10_UINT_PACK32";
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
      return "A2B10G10R10_UNORM_PACK32";
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
      return "A2B10G10R10_USCALED_PACK32";
    case VK_FORMAT_A2R10G10B10_SINT_PACK32:
      return "A2R10G10B10_SINT_PACK32";
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
      return "A2R10G10B10_SNORM_PACK32";
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
      return "A2R10G10B10_SSCALED_PACK32";
    case VK_FORMAT_A2R10G10B10_UINT_PACK32:
      return "A2R10G10B10_UINT_PACK32";
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
      return "A2R10G10B10_UNORM_PACK32";
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
      return "A2R10G10B10_USCALED_PACK32";
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
      return "A8B8G8R8_SINT_PACK32";
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
      return "A8B8G8R8_SNORM_PACK32";
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
      return "A8B8G8R8_SRGB_PACK32";
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
      return "A8B8G8R8_SSCALED_PACK32";
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
      return "A8B8G8R8_UINT_PACK32";
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
      return "A8B8G8R8_UNORM_PACK32";
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
      return "A8B8G8R8_USCALED_PACK32";
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
      return "B10G11R11_UFLOAT_PACK32";
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
      return "B4G4R4A4_UNORM_PACK16";
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
      return "B5G5R5A1_UNORM_PACK16";
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
      return "B5G6R5_UNORM_PACK16";
    case VK_FORMAT_B8G8R8A8_SINT:
      return "B8G8R8A8_SINT";
    case VK_FORMAT_B8G8R8A8_SNORM:
      return "B8G8R8A8_SNORM";
    case VK_FORMAT_B8G8R8A8_SRGB:
      return "B8G8R8A8_SRGB";
    case VK_FORMAT_B8G8R8A8_SSCALED:
      return "B8G8R8A8_SSCALED";
    case VK_FORMAT_B8G8R8A8_UINT:
      return "B8G8R8A8_UINT";
    case VK_FORMAT_B8G8R8A8_UNORM:
      return "B8G8R8A8_UNORM";
    case VK_FORMAT_B8G8R8A8_USCALED:
      return "B8G8R8A8_USCALED";
    case VK_FORMAT_B8G8R8_SINT:
      return "B8G8R8_SINT";
    case VK_FORMAT_B8G8R8_SNORM:
      return "B8G8R8_SNORM";
    case VK_FORMAT_B8G8R8_SRGB:
      return "B8G8R8_SRGB";
    case VK_FORMAT_B8G8R8_SSCALED:
      return "B8G8R8_SSCALED";
    case VK_FORMAT_B8G8R8_UINT:
      return "B8G8R8_UINT";
    case VK_FORMAT_B8G8R8_UNORM:
      return "B8G8R8_UNORM";
    case VK_FORMAT_B8G8R8_USCALED:
      return "B8G8R8_USCALED";
    case VK_FORMAT_D16_UNORM:
      return "D16_UNORM";
    case VK_FORMAT_D16_UNORM_S8_UINT:
      return "D16_UNORM_S8_UINT";
    case VK_FORMAT_D24_UNORM_S8_UINT:
      return "D24_UNORM_S8_UINT";
    case VK_FORMAT_D32_SFLOAT:
      return "D32_SFLOAT";
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
      return "D32_SFLOAT_S8_UINT";
    case VK_FORMAT_R16G16B16A16_SFLOAT:
      return "R16G16B16A16_SFLOAT";
    case VK_FORMAT_R16G16B16A16_SINT:
      return "R16G16B16A16_SINT";
    case VK_FORMAT_R16G16B16A16_SNORM:
      return "R16G16B16A16_SNORM";
    case VK_FORMAT_R16G16B16A16_SSCALED:
      return "R16G16B16A16_SSCALED";
    case VK_FORMAT_R16G16B16A16_UINT:
      return "R16G16B16A16_UINT";
    case VK_FORMAT_R16G16B16A16_UNORM:
      return "R16G16B16A16_UNORM";
    case VK_FORMAT_R16G16B16A16_USCALED:
      return "R16G16B16A16_USCALED";
    case VK_FORMAT_R16G16B16_SFLOAT:
      return "R16G16B16_SFLOAT";
    case VK_FORMAT_R16G16B16_SINT:
      return "R16G16B16_SINT";
    case VK_FORMAT_R16G16B16_SNORM:
      return "R16G16B16_SNORM";
    case VK_FORMAT_R16G16B16_SSCALED:
      return "R16G16B16_SSCALED";
    case VK_FORMAT_R16G16B16_UINT:
      return "R16G16B16_UINT";
    case VK_FORMAT_R16G16B16_UNORM:
      return "R16G16B16_UNORM";
    case VK_FORMAT_R16G16B16_USCALED:
      return "R16G16B16_USCALED";
    case VK_FORMAT_R16G16_SFLOAT:
      return "R16G16_SFLOAT";
    case VK_FORMAT_R16G16_SINT:
      return "R16G16_SINT";
    case VK_FORMAT_R16G16_SNORM:
      return "R16G16_SNORM";
    case VK_FORMAT_R16G16_SSCALED:
      return "R16G16_SSCALED";
    case VK_FORMAT_R16G16_UINT:
      return "R16G16_UINT";
    case VK_FORMAT_R16G16_UNORM:
      return "R16G16_UNORM";
    case VK_FORMAT_R16G16_USCALED:
      return "R16G16_USCALED";
    case VK_FORMAT_R16_SFLOAT:
      return "R16_SFLOAT";
    case VK_FORMAT_R16_SINT:
      return "R16_SINT";
    case VK_FORMAT_R16_SNORM:
      return "R16_SNORM";
    case VK_FORMAT_R16_SSCALED:
      return "R16_SSCALED";
    case VK_FORMAT_R16_UINT:
      return "R16_UINT";
    case VK_FORMAT_R16_UNORM:
      return "R16_UNORM";
    case VK_FORMAT_R16_USCALED:
      return "R16_USCALED";
    case VK_FORMAT_R32G32B32A32_SFLOAT:
      return "R32G32B32A32_SFLOAT";
    case VK_FORMAT_R32G32B32A32_SINT:
      return "R32G32B32A32_SINT";
    case VK_FORMAT_R32G32B32A32_UINT:
      return "R32G32B32A32_UINT";
    case VK_FORMAT_R32G32B32_SFLOAT:
      return "R32G32B32_SFLOAT";
    case VK_FORMAT_R32G32B32_SINT:
      return "R32G32B32_SINT";
    case VK_FORMAT_R32G32B32_UINT:
      return "R32G32B32_UINT";
    case VK_FORMAT_R32G32_SFLOAT:
      return "R32G32_SFLOAT";
    case VK_FORMAT_R32G32_SINT:
      return "R32G32_SINT";
    case VK_FORMAT_R32G32_UINT:
      return "R32G32_UINT";
    case VK_FORMAT_R32_SFLOAT:
      return "R32_SFLOAT";
    case VK_FORMAT_R32_SINT:
      return "R32_SINT";
    case VK_FORMAT_R32_UINT:
      return "R32_UINT";
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
      return "R4G4B4A4_UNORM_PACK16";
    case VK_FORMAT_R4G4_UNORM_PACK8:
      return "R4G4_UNORM_PACK8";
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
      return "R5G5B5A1_UNORM_PACK16";
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
      return "R5G6B5_UNORM_PACK16";
    case VK_FORMAT_R64G64B64A64_SFLOAT:
      return "R64G64B64A64_SFLOAT";
    case VK_FORMAT_R64G64B64A64_SINT:
      return "R64G64B64A64_SINT";
    case VK_FORMAT_R64G64B64A64_UINT:
      return "R64G64B64A64_UINT";
    case VK_FORMAT_R64G64B64_SFLOAT:
      return "R64G64B64_SFLOAT";
    case VK_FORMAT_R64G64B64_SINT:
      return "R64G64B64_SINT";
    case VK_FORMAT_R64G64B64_UINT:
      return "R64G64B64_UINT";
    case VK_FORMAT_R64G64_SFLOAT:
      return "R64G64_SFLOAT";
    case VK_FORMAT_R64G64_SINT:
      return "R64G64_SINT";
    case VK_FORMAT_R64G64_UINT:
      return "R64G64_UINT";
    case VK_FORMAT_R64_SFLOAT:
      return "R64_SFLOAT";
    case VK_FORMAT_R64_SINT:
      return "R64_SINT";
    case VK_FORMAT_R64_UINT:
      return "R64_UINT";
    case VK_FORMAT_R8G8B8A8_SINT:
      return "R8G8B8A8_SINT";
    case VK_FORMAT_R8G8B8A8_SNORM:
      return "R8G8B8A8_SNORM";
    case VK_FORMAT_R8G8B8A8_SRGB:
      return "R8G8B8A8_SRGB";
    case VK_FORMAT_R8G8B8A8_SSCALED:
      return "R8G8B8A8_SSCALED";
    case VK_FORMAT_R8G8B8A8_UINT:
      return "R8G8B8A8_UINT";
    case VK_FORMAT_R8G8B8A8_UNORM:
      return "R8G8B8A8_UNORM";
    case VK_FORMAT_R8G8B8A8_USCALED:
      return "R8G8B8A8_USCALED";
    case VK_FORMAT_R8G8B8_SINT:
      return "R8G8B8_SINT";
    case VK_FORMAT_R8G8B8_SNORM:
      return "R8G8B8_SNORM";
    case VK_FORMAT_R8G8B8_SRGB:
      return "R8G8B8_SRGB";
    case VK_FORMAT_R8G8B8_SSCALED:
      return "R8G8B8_SSCALED";
    case VK_FORMAT_R8G8B8_UINT:
      return "R8G8B8_UINT";
    case VK_FORMAT_R8G8B8_UNORM:
      return "R8G8B8_UNORM";
    case VK_FORMAT_R8G8B8_USCALED:
      return "R8G8B8_USCALED";
    case VK_FORMAT_R8G8_SINT:
      return "R8G8_SINT";
    case VK_FORMAT_R8G8_SNORM:
      return "R8G8_SNORM";
    case VK_FORMAT_R8G8_SRGB:
      return "R8G8_SRGB";
    case VK_FORMAT_R8G8_SSCALED:
      return "R8G8_SSCALED";
    case VK_FORMAT_R8G8_UINT:
      return "R8G8_UINT";
    case VK_FORMAT_R8G8_UNORM:
      return "R8G8_UNORM";
    case VK_FORMAT_R8G8_USCALED:
      return "R8G8_USCALED";
    case VK_FORMAT_R8_SINT:
      return "R8_SINT";
    case VK_FORMAT_R8_SNORM:
      return "R8_SNORM";
    case VK_FORMAT_R8_SRGB:
      return "R8_SRGB";
    case VK_FORMAT_R8_SSCALED:
      return "R8_SSCALED";
    case VK_FORMAT_R8_UINT:
      return "R8_UINT";
    case VK_FORMAT_R8_UNORM:
      return "R8_UNORM";
    case VK_FORMAT_R8_USCALED:
      return "R8_USCALED";
    case VK_FORMAT_S8_UINT:
      return "S8_UINT";
    case VK_FORMAT_X8_D24_UNORM_PACK32:
      return "X8_D24_UNORM_PACK32";
    default:
      RUNTIME_ASSERT_MSG(false, "Format not supported by amber.");
  }
#pragma warning(pop)
}

}  // namespace gf_layers::amber_scoop_layer
