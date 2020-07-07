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

#ifndef GF_LAYERS_LAYER_UTIL_SETTINGS_H
#define GF_LAYERS_LAYER_UTIL_SETTINGS_H

#include <cstdint>
#include <string>

namespace gf_layers {

bool get_setting_string(const char* env_var, const char* android_prop,
                        std::string* value);

bool get_setting_uint64(const char* env_var, const char* android_prop,
                        std::uint64_t* value);

}  // namespace gf_layers

#endif  // GF_LAYERS_LAYER_UTIL_SETTINGS_H
