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

#ifndef VKLAYER_GF_AMBER_SCOOP_VULKAN_FORMATS_H
#define VKLAYER_GF_AMBER_SCOOP_VULKAN_FORMATS_H

#include <vulkan/vulkan.h>

#include <cstdint>
#include <string>
#include <vector>

namespace gf_layers::amber_scoop_layer {
// This class has two main functions. The first one is to convert Vulkan's
// |VkFormat| enums to format name strings used in Amber script. The conversion
// is done in the constructor but can also be done without creating an instance
// of the class by calling static function |VkFormatToAmberFormatName|. The
// second function is to parse the properties of the color format from it's
// name. This is also done in the constructor.
class VulkanFormat {
 public:
  enum ColorComponentName { kX, kD, kR, kG, kB, kA };

  enum ColorComponentType {
    kUInt,
    kUNorm,
    kUFloat,
    kUScaled,
    kSInt,
    kSNorm,
    kSScaled,
    kSFloat,
    kSRGB,
    kS8,
  };

  struct ColorComponent {
    ColorComponentName color_component_name;
    // Component type of the color component. This information must be stored
    // per component, because some formats consist of components of different
    // types, f.ex. "D32_SFLOAT_S8_UINT".
    ColorComponentType format_mode;
    uint16_t width;
  };

  explicit VulkanFormat(VkFormat vk_format);

  // Returns true if the format is packed.
  [[nodiscard]] bool IsPackedFormat() const { return pack_size_ > 0; }

  // Returns number of padding bytes required by this format. Padding bytes are
  // required for vec3 formats.
  [[nodiscard]] uint8_t GetPaddingBytes() const;

  // Returns a name of the given |vk_format| as string. The names are as they
  // are
  // used in Amber i.e. don't contain the "VK_FORMAT_" prefix. Returns only
  // format names that are supported by Amber. Asserts if an unsupported
  // |vk_format| is given.
  static const char* VkFormatToAmberFormatName(VkFormat vk_format);

  // Getters for private fields.

  [[nodiscard]] const std::string& GetName() const { return name_; }
  [[nodiscard]] const VkFormat& GetVkFormat() const { return vk_format_; }
  [[nodiscard]] uint8_t GetPackSize() const { return pack_size_; }
  [[nodiscard]] const std::vector<ColorComponent>& GetColorComponents() const {
    return color_components_;
  }
  [[nodiscard]] uint16_t GetTotalWidthBits() const { return total_width_bits_; }

 private:
  // Parses the color components from the name.
  void ParseName();

  // Parses one "chunk" of data from the name.
  void ParseChunk(const std::string& chunk_data,
                  ColorComponentType* current_type);

  // Amber's format name, f.ex. R16G16_UNORM.
  std::string name_;
  // VkFormat of this format.
  VkFormat vk_format_ = VK_FORMAT_UNDEFINED;
  // Pack size for packed formats, f.ex. '16' for
  // VK_FORMAT_A1R5G5B5_UNORM_PACK16. 0 for non packed formats.
  uint8_t pack_size_ = 0;
  // Pre-computed total width (sum of color components' widths). Doesn't include
  // possible padding bits for vec3 formats.
  uint16_t total_width_bits_ = 0;
  // List of color components that this format contains.
  std::vector<ColorComponent> color_components_;
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_VULKAN_FORMATS_H
