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

#include "gf_layers_layer_util/logging.h"

#include <cstdarg>

#if defined(__ANDROID__)
#include <android/log.h>
#else
#include <cstdio>
#endif

namespace gf_layers {

void Log(const char* format_string, ...) {
  va_list vargs;

  // NOLINTNEXTLINE
  va_start(vargs, format_string);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"

#if defined(__ANDROID__)
  // NOLINTNEXTLINE
  __android_log_vprint(ANDROID_LOG_DEBUG, "VkLayer_GF", format_string, vargs);
#else
  // NOLINTNEXTLINE
  std::vfprintf(stderr, format_string, vargs);
  std::fprintf(stderr, "\n");
#endif

#pragma clang diagnostic pop

  // NOLINTNEXTLINE
  va_end(vargs);
}

}  // namespace gf_layers
