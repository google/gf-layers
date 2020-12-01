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

#include "VkLayer_GF_amber_scoop/buffer_to_file.h"

#include <absl/types/span.h>

#include <cstdint>

#include "gf_layers_layer_util/logging.h"

namespace gf_layers::amber_scoop_layer {

BufferToFile::BufferToFile(const std::string& file_path) {
  // |file_stream_| is closed automatically in it's destructor.
  file_stream_.open(file_path,
                    std::ios::out | std::ios::binary | std::ios::trunc);
  // open() doesn't throw exception so we must check that the file is open.
  RUNTIME_ASSERT(file_stream_.is_open());
}

void BufferToFile::WriteComponents(const absl::Span<const char>& data,
                                   VkDeviceSize read_offset,
                                   const VulkanFormat& format) {
  // Compute the number of bytes to be written.
  static const uint8_t BYTE_WIDTH = 8;
  const auto bytes_to_write =
      static_cast<VkDeviceSize>(format.GetTotalWidthBits() / BYTE_WIDTH);

  // Make sure we are not reading outside the data span.
  if (read_offset + bytes_to_write > data.size()) {
    LOG("Trying to read outside the data span.");
    RUNTIME_ASSERT(false);
  }

  file_stream_.write(&data[read_offset],
                     static_cast<std::streamsize>(bytes_to_write));

  // Vec3 formats need to be aligned to be size of vec4. Padding bytes will be
  // filled with zeroes.
  uint8_t padding_bytes = format.GetPaddingBytes();
  if (padding_bytes != 0) {
    static const uint64_t zero = 0UL;
    file_stream_.write(reinterpret_cast<const char*>(&zero), padding_bytes);
  }
}

}  // namespace gf_layers::amber_scoop_layer
