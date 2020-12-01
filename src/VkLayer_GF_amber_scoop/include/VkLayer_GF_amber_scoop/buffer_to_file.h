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

#ifndef VKLAYER_GF_AMBER_SCOOP_BUFFER_TO_FILE_H_
#define VKLAYER_GF_AMBER_SCOOP_BUFFER_TO_FILE_H_

#include <absl/types/span.h>
#include <vulkan/vulkan.h>

#include <fstream>
#include <string>

#include "VkLayer_GF_amber_scoop/vulkan_formats.h"

namespace gf_layers::amber_scoop_layer {

// This class is used to dump contents of a buffer to a file. Data is copied one
// element at a time via |WriteComponents| function.
class BufferToFile {
 public:
  // Opens the file. Existing file with the same name will be overwritten.
  // Asserts if the file can't be opened.
  explicit BufferToFile(const std::string& file_path);

  // Writes all components of a data element. Data will be read from the given
  // |data| span starting from byte offset given in |read_offset|. |format|
  // defines the amount of bytes to be copied. If the format is 'vec3' type, the
  // output data will use alignment to be size of 'vec4'.
  void WriteComponents(const absl::Span<const char>& data,
                       VkDeviceSize read_offset, const VulkanFormat& format);

  ~BufferToFile() = default;

  // Disabled copy/move constructors and copy/move assign operators.
  BufferToFile(const BufferToFile&) = delete;
  BufferToFile(BufferToFile&&) = delete;
  BufferToFile& operator=(const BufferToFile&) = delete;
  BufferToFile& operator=(BufferToFile&&) = delete;

 private:
  std::ofstream file_stream_;
};

}  // namespace gf_layers::amber_scoop_layer

#endif  // VKLAYER_GF_AMBER_SCOOP_BUFFER_TO_FILE_H_
