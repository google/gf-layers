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

#include "gf_layers_layer_util/settings.h"

#include <cstdlib>
#include <cstring>
#include <sstream>

#include "gf_layers_layer_util/logging.h"

#if defined(__ANDROID__)
#include <sys/system_properties.h>

#include <array>
#endif

namespace gf_layers {

namespace {

#if defined(__ANDROID__)
bool GetAndroidProperty(const char* android_property_name, std::string* value) {
  std::array<char, PROP_VALUE_MAX> buff{};
  if (__system_property_get(android_property_name, buff.data()) <= 0) {
    return false;
  }
  // Also fail if the value is zero length.
  if (std::strlen(buff.data()) == 0) {
    return false;
  }
  *value = buff.data();
  return true;
}
#endif

bool GetEnvVar(const char* env_var, std::string* value) {
  const char* result = std::getenv(env_var);
  if (result == nullptr) {
    return false;
  }
  // Also fail if the value is zero length.
  if (std::strlen(result) == 0) {
    return false;
  }
  *value = result;
  return true;
}
}  // namespace

bool GetSettingString(const char* env_var, const char* android_prop,
                      std::string* value) {
  if (GetEnvVar(env_var, value)) {
    return true;
  }

#if defined(__ANDROID__)
  if (GetAndroidProperty(android_prop, value)) {
    return true;
  }
#endif

  LOG("Could not find setting: %s / %s", env_var, android_prop);

  return false;
}

bool GetSettingUint64(const char* env_var, const char* android_prop,
                      std::uint64_t* value) {
  std::string temp;
  if (!GetSettingString(env_var, android_prop, &temp)) {
    return false;
  }

  std::istringstream ss{temp};
  std::uint64_t temp_uint{};
  ss >> temp_uint;

  if (ss.fail()) {
    LOG("Failed to parse setting %s / %s with value %s", env_var, android_prop,
        temp.c_str());
    return false;
  }

  *value = temp_uint;
  return true;
}

}  // namespace gf_layers
