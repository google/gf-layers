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

#include <cstdlib>
#include <string>
// <memory> is used by GCC's libstdc++, but not LLVM's libc++.
#include <memory>  // IWYU pragma: keep

#pragma GCC diagnostic push  // Clang, GCC.
#pragma warning(push, 1)     // MSVC: reduces warning level to W1.

#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma warning( \
    disable : 4365)  // signed/unsigned mismatch in abs/strings/ascii.h

#include "absl/strings/str_split.h"

#pragma GCC diagnostic pop
#pragma warning(pop)

#include "gf_layers_layer_util/logging.h"

namespace gf_layers::amber_scoop_layer {

const char* VulkanFormat::VkFormatToAmberFormatName(VkFormat vk_format) {
  if (vk_format == VK_FORMAT_A1R5G5B5_UNORM_PACK16) {
    return "A1R5G5B5_UNORM_PACK16";
  }
  if (vk_format == VK_FORMAT_A2B10G10R10_SINT_PACK32) {
    return "A2B10G10R10_SINT_PACK32";
  }
  if (vk_format == VK_FORMAT_A2B10G10R10_SNORM_PACK32) {
    return "A2B10G10R10_SNORM_PACK32";
  }
  if (vk_format == VK_FORMAT_A2B10G10R10_SSCALED_PACK32) {
    return "A2B10G10R10_SSCALED_PACK32";
  }
  if (vk_format == VK_FORMAT_A2B10G10R10_UINT_PACK32) {
    return "A2B10G10R10_UINT_PACK32";
  }
  if (vk_format == VK_FORMAT_A2B10G10R10_UNORM_PACK32) {
    return "A2B10G10R10_UNORM_PACK32";
  }
  if (vk_format == VK_FORMAT_A2B10G10R10_USCALED_PACK32) {
    return "A2B10G10R10_USCALED_PACK32";
  }
  if (vk_format == VK_FORMAT_A2R10G10B10_SINT_PACK32) {
    return "A2R10G10B10_SINT_PACK32";
  }
  if (vk_format == VK_FORMAT_A2R10G10B10_SNORM_PACK32) {
    return "A2R10G10B10_SNORM_PACK32";
  }
  if (vk_format == VK_FORMAT_A2R10G10B10_SSCALED_PACK32) {
    return "A2R10G10B10_SSCALED_PACK32";
  }
  if (vk_format == VK_FORMAT_A2R10G10B10_UINT_PACK32) {
    return "A2R10G10B10_UINT_PACK32";
  }
  if (vk_format == VK_FORMAT_A2R10G10B10_UNORM_PACK32) {
    return "A2R10G10B10_UNORM_PACK32";
  }
  if (vk_format == VK_FORMAT_A2R10G10B10_USCALED_PACK32) {
    return "A2R10G10B10_USCALED_PACK32";
  }
  if (vk_format == VK_FORMAT_A8B8G8R8_SINT_PACK32) {
    return "A8B8G8R8_SINT_PACK32";
  }
  if (vk_format == VK_FORMAT_A8B8G8R8_SNORM_PACK32) {
    return "A8B8G8R8_SNORM_PACK32";
  }
  if (vk_format == VK_FORMAT_A8B8G8R8_SRGB_PACK32) {
    return "A8B8G8R8_SRGB_PACK32";
  }
  if (vk_format == VK_FORMAT_A8B8G8R8_SSCALED_PACK32) {
    return "A8B8G8R8_SSCALED_PACK32";
  }
  if (vk_format == VK_FORMAT_A8B8G8R8_UINT_PACK32) {
    return "A8B8G8R8_UINT_PACK32";
  }
  if (vk_format == VK_FORMAT_A8B8G8R8_UNORM_PACK32) {
    return "A8B8G8R8_UNORM_PACK32";
  }
  if (vk_format == VK_FORMAT_A8B8G8R8_USCALED_PACK32) {
    return "A8B8G8R8_USCALED_PACK32";
  }
  if (vk_format == VK_FORMAT_B10G11R11_UFLOAT_PACK32) {
    return "B10G11R11_UFLOAT_PACK32";
  }
  if (vk_format == VK_FORMAT_B4G4R4A4_UNORM_PACK16) {
    return "B4G4R4A4_UNORM_PACK16";
  }
  if (vk_format == VK_FORMAT_B5G5R5A1_UNORM_PACK16) {
    return "B5G5R5A1_UNORM_PACK16";
  }
  if (vk_format == VK_FORMAT_B5G6R5_UNORM_PACK16) {
    return "B5G6R5_UNORM_PACK16";
  }
  if (vk_format == VK_FORMAT_B8G8R8A8_SINT) {
    return "B8G8R8A8_SINT";
  }
  if (vk_format == VK_FORMAT_B8G8R8A8_SNORM) {
    return "B8G8R8A8_SNORM";
  }
  if (vk_format == VK_FORMAT_B8G8R8A8_SRGB) {
    return "B8G8R8A8_SRGB";
  }
  if (vk_format == VK_FORMAT_B8G8R8A8_SSCALED) {
    return "B8G8R8A8_SSCALED";
  }
  if (vk_format == VK_FORMAT_B8G8R8A8_UINT) {
    return "B8G8R8A8_UINT";
  }
  if (vk_format == VK_FORMAT_B8G8R8A8_UNORM) {
    return "B8G8R8A8_UNORM";
  }
  if (vk_format == VK_FORMAT_B8G8R8A8_USCALED) {
    return "B8G8R8A8_USCALED";
  }
  if (vk_format == VK_FORMAT_B8G8R8_SINT) {
    return "B8G8R8_SINT";
  }
  if (vk_format == VK_FORMAT_B8G8R8_SNORM) {
    return "B8G8R8_SNORM";
  }
  if (vk_format == VK_FORMAT_B8G8R8_SRGB) {
    return "B8G8R8_SRGB";
  }
  if (vk_format == VK_FORMAT_B8G8R8_SSCALED) {
    return "B8G8R8_SSCALED";
  }
  if (vk_format == VK_FORMAT_B8G8R8_UINT) {
    return "B8G8R8_UINT";
  }
  if (vk_format == VK_FORMAT_B8G8R8_UNORM) {
    return "B8G8R8_UNORM";
  }
  if (vk_format == VK_FORMAT_B8G8R8_USCALED) {
    return "B8G8R8_USCALED";
  }
  if (vk_format == VK_FORMAT_D16_UNORM) {
    return "D16_UNORM";
  }
  if (vk_format == VK_FORMAT_D16_UNORM_S8_UINT) {
    return "D16_UNORM_S8_UINT";
  }
  if (vk_format == VK_FORMAT_D24_UNORM_S8_UINT) {
    return "D24_UNORM_S8_UINT";
  }
  if (vk_format == VK_FORMAT_D32_SFLOAT) {
    return "D32_SFLOAT";
  }
  if (vk_format == VK_FORMAT_D32_SFLOAT_S8_UINT) {
    return "D32_SFLOAT_S8_UINT";
  }
  if (vk_format == VK_FORMAT_R16G16B16A16_SFLOAT) {
    return "R16G16B16A16_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R16G16B16A16_SINT) {
    return "R16G16B16A16_SINT";
  }
  if (vk_format == VK_FORMAT_R16G16B16A16_SNORM) {
    return "R16G16B16A16_SNORM";
  }
  if (vk_format == VK_FORMAT_R16G16B16A16_SSCALED) {
    return "R16G16B16A16_SSCALED";
  }
  if (vk_format == VK_FORMAT_R16G16B16A16_UINT) {
    return "R16G16B16A16_UINT";
  }
  if (vk_format == VK_FORMAT_R16G16B16A16_UNORM) {
    return "R16G16B16A16_UNORM";
  }
  if (vk_format == VK_FORMAT_R16G16B16A16_USCALED) {
    return "R16G16B16A16_USCALED";
  }
  if (vk_format == VK_FORMAT_R16G16B16_SFLOAT) {
    return "R16G16B16_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R16G16B16_SINT) {
    return "R16G16B16_SINT";
  }
  if (vk_format == VK_FORMAT_R16G16B16_SNORM) {
    return "R16G16B16_SNORM";
  }
  if (vk_format == VK_FORMAT_R16G16B16_SSCALED) {
    return "R16G16B16_SSCALED";
  }
  if (vk_format == VK_FORMAT_R16G16B16_UINT) {
    return "R16G16B16_UINT";
  }
  if (vk_format == VK_FORMAT_R16G16B16_UNORM) {
    return "R16G16B16_UNORM";
  }
  if (vk_format == VK_FORMAT_R16G16B16_USCALED) {
    return "R16G16B16_USCALED";
  }
  if (vk_format == VK_FORMAT_R16G16_SFLOAT) {
    return "R16G16_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R16G16_SINT) {
    return "R16G16_SINT";
  }
  if (vk_format == VK_FORMAT_R16G16_SNORM) {
    return "R16G16_SNORM";
  }
  if (vk_format == VK_FORMAT_R16G16_SSCALED) {
    return "R16G16_SSCALED";
  }
  if (vk_format == VK_FORMAT_R16G16_UINT) {
    return "R16G16_UINT";
  }
  if (vk_format == VK_FORMAT_R16G16_UNORM) {
    return "R16G16_UNORM";
  }
  if (vk_format == VK_FORMAT_R16G16_USCALED) {
    return "R16G16_USCALED";
  }
  if (vk_format == VK_FORMAT_R16_SFLOAT) {
    return "R16_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R16_SINT) {
    return "R16_SINT";
  }
  if (vk_format == VK_FORMAT_R16_SNORM) {
    return "R16_SNORM";
  }
  if (vk_format == VK_FORMAT_R16_SSCALED) {
    return "R16_SSCALED";
  }
  if (vk_format == VK_FORMAT_R16_UINT) {
    return "R16_UINT";
  }
  if (vk_format == VK_FORMAT_R16_UNORM) {
    return "R16_UNORM";
  }
  if (vk_format == VK_FORMAT_R16_USCALED) {
    return "R16_USCALED";
  }
  if (vk_format == VK_FORMAT_R32G32B32A32_SFLOAT) {
    return "R32G32B32A32_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R32G32B32A32_SINT) {
    return "R32G32B32A32_SINT";
  }
  if (vk_format == VK_FORMAT_R32G32B32A32_UINT) {
    return "R32G32B32A32_UINT";
  }
  if (vk_format == VK_FORMAT_R32G32B32_SFLOAT) {
    return "R32G32B32_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R32G32B32_SINT) {
    return "R32G32B32_SINT";
  }
  if (vk_format == VK_FORMAT_R32G32B32_UINT) {
    return "R32G32B32_UINT";
  }
  if (vk_format == VK_FORMAT_R32G32_SFLOAT) {
    return "R32G32_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R32G32_SINT) {
    return "R32G32_SINT";
  }
  if (vk_format == VK_FORMAT_R32G32_UINT) {
    return "R32G32_UINT";
  }
  if (vk_format == VK_FORMAT_R32_SFLOAT) {
    return "R32_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R32_SINT) {
    return "R32_SINT";
  }
  if (vk_format == VK_FORMAT_R32_UINT) {
    return "R32_UINT";
  }
  if (vk_format == VK_FORMAT_R4G4B4A4_UNORM_PACK16) {
    return "R4G4B4A4_UNORM_PACK16";
  }
  if (vk_format == VK_FORMAT_R4G4_UNORM_PACK8) {
    return "R4G4_UNORM_PACK8";
  }
  if (vk_format == VK_FORMAT_R5G5B5A1_UNORM_PACK16) {
    return "R5G5B5A1_UNORM_PACK16";
  }
  if (vk_format == VK_FORMAT_R5G6B5_UNORM_PACK16) {
    return "R5G6B5_UNORM_PACK16";
  }
  if (vk_format == VK_FORMAT_R64G64B64A64_SFLOAT) {
    return "R64G64B64A64_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R64G64B64A64_SINT) {
    return "R64G64B64A64_SINT";
  }
  if (vk_format == VK_FORMAT_R64G64B64A64_UINT) {
    return "R64G64B64A64_UINT";
  }
  if (vk_format == VK_FORMAT_R64G64B64_SFLOAT) {
    return "R64G64B64_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R64G64B64_SINT) {
    return "R64G64B64_SINT";
  }
  if (vk_format == VK_FORMAT_R64G64B64_UINT) {
    return "R64G64B64_UINT";
  }
  if (vk_format == VK_FORMAT_R64G64_SFLOAT) {
    return "R64G64_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R64G64_SINT) {
    return "R64G64_SINT";
  }
  if (vk_format == VK_FORMAT_R64G64_UINT) {
    return "R64G64_UINT";
  }
  if (vk_format == VK_FORMAT_R64_SFLOAT) {
    return "R64_SFLOAT";
  }
  if (vk_format == VK_FORMAT_R64_SINT) {
    return "R64_SINT";
  }
  if (vk_format == VK_FORMAT_R64_UINT) {
    return "R64_UINT";
  }
  if (vk_format == VK_FORMAT_R8G8B8A8_SINT) {
    return "R8G8B8A8_SINT";
  }
  if (vk_format == VK_FORMAT_R8G8B8A8_SNORM) {
    return "R8G8B8A8_SNORM";
  }
  if (vk_format == VK_FORMAT_R8G8B8A8_SRGB) {
    return "R8G8B8A8_SRGB";
  }
  if (vk_format == VK_FORMAT_R8G8B8A8_SSCALED) {
    return "R8G8B8A8_SSCALED";
  }
  if (vk_format == VK_FORMAT_R8G8B8A8_UINT) {
    return "R8G8B8A8_UINT";
  }
  if (vk_format == VK_FORMAT_R8G8B8A8_UNORM) {
    return "R8G8B8A8_UNORM";
  }
  if (vk_format == VK_FORMAT_R8G8B8A8_USCALED) {
    return "R8G8B8A8_USCALED";
  }
  if (vk_format == VK_FORMAT_R8G8B8_SINT) {
    return "R8G8B8_SINT";
  }
  if (vk_format == VK_FORMAT_R8G8B8_SNORM) {
    return "R8G8B8_SNORM";
  }
  if (vk_format == VK_FORMAT_R8G8B8_SRGB) {
    return "R8G8B8_SRGB";
  }
  if (vk_format == VK_FORMAT_R8G8B8_SSCALED) {
    return "R8G8B8_SSCALED";
  }
  if (vk_format == VK_FORMAT_R8G8B8_UINT) {
    return "R8G8B8_UINT";
  }
  if (vk_format == VK_FORMAT_R8G8B8_UNORM) {
    return "R8G8B8_UNORM";
  }
  if (vk_format == VK_FORMAT_R8G8B8_USCALED) {
    return "R8G8B8_USCALED";
  }
  if (vk_format == VK_FORMAT_R8G8_SINT) {
    return "R8G8_SINT";
  }
  if (vk_format == VK_FORMAT_R8G8_SNORM) {
    return "R8G8_SNORM";
  }
  if (vk_format == VK_FORMAT_R8G8_SRGB) {
    return "R8G8_SRGB";
  }
  if (vk_format == VK_FORMAT_R8G8_SSCALED) {
    return "R8G8_SSCALED";
  }
  if (vk_format == VK_FORMAT_R8G8_UINT) {
    return "R8G8_UINT";
  }
  if (vk_format == VK_FORMAT_R8G8_UNORM) {
    return "R8G8_UNORM";
  }
  if (vk_format == VK_FORMAT_R8G8_USCALED) {
    return "R8G8_USCALED";
  }
  if (vk_format == VK_FORMAT_R8_SINT) {
    return "R8_SINT";
  }
  if (vk_format == VK_FORMAT_R8_SNORM) {
    return "R8_SNORM";
  }
  if (vk_format == VK_FORMAT_R8_SRGB) {
    return "R8_SRGB";
  }
  if (vk_format == VK_FORMAT_R8_SSCALED) {
    return "R8_SSCALED";
  }
  if (vk_format == VK_FORMAT_R8_UINT) {
    return "R8_UINT";
  }
  if (vk_format == VK_FORMAT_R8_UNORM) {
    return "R8_UNORM";
  }
  if (vk_format == VK_FORMAT_R8_USCALED) {
    return "R8_USCALED";
  }
  if (vk_format == VK_FORMAT_S8_UINT) {
    return "S8_UINT";
  }
  if (vk_format == VK_FORMAT_X8_D24_UNORM_PACK32) {
    return "X8_D24_UNORM_PACK32";
  }

  LOG("Format not supported by amber.");
  RUNTIME_ASSERT(false);
}

VulkanFormat::VulkanFormat(VkFormat vk_format) : vk_format_(vk_format) {
  name_ = VkFormatToAmberFormatName(vk_format_);
  // Parse information from the name string.
  ParseName();

  // Set total width in bytes.
  for (ColorComponent color_component : color_components_) {
    total_width_bits_ += color_component.width;
  }
}

uint8_t VulkanFormat::GetPaddingBytes() const {
  // Formats with 3 components (vec3 formats) have the same alignment as vec4
  // formats. Packed formats don't have padding.
  if (!IsPackedFormat() && color_components_.size() == 3) {
    static const uint8_t BYTE_WIDTH = 8;
    // Padding width (in bytes) is the size of one color component. All
    // components in a vec3 format have the same width. Therefore we are using
    // the width of the color component at position 0.
    return static_cast<uint8_t>(color_components_.at(0).width / BYTE_WIDTH);
  }
  return 0;
}

void VulkanFormat::ParseName() {
  // Split the name by '_' into chunks.
  std::vector<std::string> chunks = absl::StrSplit(name_, '_');

  // Parse chunks backwards order, because we need to know the type before
  // parsing the component data. The type is stored in |current_type| variable.
  ColorComponentType current_type = kSInt;
  for (auto iterator = chunks.rbegin(); iterator != chunks.rend(); iterator++) {
    ParseChunk(*iterator, &current_type);
  }
}

void VulkanFormat::ParseChunk(const std::string& chunk_data,
                              ColorComponentType* current_type) {
  // Get "pack size" from packed formats.
  if (chunk_data[0] == 'P') {
    if (chunk_data == "PACK8") {
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
      pack_size_ = 8;
    } else if (chunk_data == "PACK16") {
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
      pack_size_ = 16;
    } else if (chunk_data == "PACK32") {
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
      pack_size_ = 32;
    } else {
      LOG("Unknown format.");
      RUNTIME_ASSERT(false);
    }
    return;
  }

  // Handle unsigned types.
  if (chunk_data[0] == 'U') {
    if (chunk_data == "UINT") {
      *current_type = kUInt;
    } else if (chunk_data == "UNORM") {
      *current_type = kUNorm;
    } else if (chunk_data == "UFLOAT") {
      *current_type = kUFloat;
    } else if (chunk_data == "USCALED") {
      *current_type = kUScaled;
    } else {
      LOG("Unknown format.");
      RUNTIME_ASSERT(false);
    }
    return;
  }

  // Handle signed formats.
  if (chunk_data[0] == 'S') {
    if (chunk_data == "SINT") {
      *current_type = kSInt;
    } else if (chunk_data == "SNORM") {
      *current_type = kSNorm;
    } else if (chunk_data == "SSCALED") {
      *current_type = kSScaled;
    } else if (chunk_data == "SFLOAT") {
      *current_type = kSFloat;
    } else if (chunk_data == "SRGB") {
      *current_type = kSRGB;
    } else if (chunk_data == "S8") {
      *current_type = kS8;
    } else {
      LOG("Unknown format.");
      RUNTIME_ASSERT(false);
    }
    return;
  }

  // Parse the color components chunk, f.ex. "A1R5G5B5".
  // Start from the end of the chunk string.
  for (auto position = static_cast<int32_t>(chunk_data.size() - 1);
       position >= 0; position--) {
    ColorComponent color_component = {};
    // Move the position backwards until a character is found, or the position
    // goes negative in case of invalid string (character in position '0' should
    // be always a valid character).
    for (; position >= 0; position--) {
      auto pos = static_cast<size_t>(position);
      if (chunk_data[pos] == 'X') {
        color_component.color_component_name = kX;
        break;
      }
      if (chunk_data[pos] == 'D') {
        color_component.color_component_name = kD;
        break;
      }
      if (chunk_data[pos] == 'R') {
        color_component.color_component_name = kR;
        break;
      }
      if (chunk_data[pos] == 'G') {
        color_component.color_component_name = kG;
        break;
      }
      if (chunk_data[pos] == 'B') {
        color_component.color_component_name = kB;
        break;
      }
      if (chunk_data[pos] == 'A') {
        color_component.color_component_name = kA;
        break;
      }
    }
    // Position should be zero or positive value. If not, the format string is
    // invalid or not supported.
    if (position < 0) {
      LOG("Format not supported.");
      RUNTIME_ASSERT(false);
    }

    // Parse the color component width. The width number starts from the next
    // character of the current position (color component type letter).
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const char* width_str_start = chunk_data.c_str() + position + 1;
    const int BASE_10 = 10;
    uint64_t width = std::strtoull(width_str_start, nullptr, BASE_10);

    if (width == 0) {
      LOG("Unable to parse color component width.");
      RUNTIME_ASSERT(false);
    }
    color_component.width = static_cast<uint16_t>(width);
    // We are parsing the widths in backwards order, so the color component
    // needs to be insert in the beginning of the list to keep the order.
    color_components_.insert(color_components_.begin(), color_component);
  }
}

}  // namespace gf_layers::amber_scoop_layer
