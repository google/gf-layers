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

#include "gf_layers_layer_util/spirv.h"

namespace gf_layers {

static constexpr uint32_t kMajorVersionShift = 16;
static constexpr uint32_t kMinorVersionShift = 8;
static constexpr uint32_t kOnePartMask = 0xff;

uint32_t GetSpirvVersionWord(uint8_t major_part, uint8_t minor_part) {
  return (static_cast<uint32_t>(major_part) << kMajorVersionShift) |
         (static_cast<uint32_t>(minor_part) << kMinorVersionShift);
}

uint8_t GetSpirvVersionMajorPart(uint32_t version_word) {
  return static_cast<uint8_t>((version_word >> kMajorVersionShift) &
                              kOnePartMask);
}

uint8_t GetSpirvVersionMinorPart(uint32_t version_word) {
  return static_cast<uint8_t>((version_word >> kMinorVersionShift) &
                              kOnePartMask);
}

}  // namespace gf_layers
