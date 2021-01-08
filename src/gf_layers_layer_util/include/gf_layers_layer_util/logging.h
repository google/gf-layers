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

#ifndef GF_LAYERS_LAYER_UTIL_LOGGING_H
#define GF_LAYERS_LAYER_UTIL_LOGGING_H

namespace gf_layers {

void Log(const char* format_string, ...);

}  // namespace gf_layers

#if defined(NDEBUG)
#define GF_LAYERS_DEBUG (false)
#else
#define GF_LAYERS_DEBUG (true)
#endif

#define GF_LAYERS_STRINGIFY(X) GF_LAYERS_STRINGIFY_HELPER(X)
#define GF_LAYERS_STRINGIFY_HELPER(X) #X

// Logs to stderr, or logcat on Android.
// The first argument must be a string literal because we add adjacent string
// literals.
// Pretends to call fprintf directly, which ensures static checks of the format
// string on most compilers, without any compiler-specific annotations.
#define LOG(...)                                                        \
  do {                                                                  \
    gf_layers::Log(__FILE__                                             \
                   ":" GF_LAYERS_STRINGIFY(__LINE__) ": " __VA_ARGS__); \
    if (false) {                                                        \
      std::fprintf(stderr, __VA_ARGS__);                                \
    }                                                                   \
  } while (false)

#define RUNTIME_ASSERT(x)           \
  do {                              \
    if (!(x)) {                     \
      LOG("Assertion failed: " #x); \
      abort();                      \
    }                               \
  } while (false)

#define RUNTIME_ASSERT_MSG(x, ...)  \
  do {                              \
    if (!(x)) {                     \
      LOG("Assertion failed: " #x); \
      LOG("Message: " __VA_ARGS__); \
      abort();                      \
    }                               \
  } while (false)

#define DEBUG_LOG(...)     \
  do {                     \
    if (GF_LAYERS_DEBUG) { \
      LOG(__VA_ARGS__);    \
    }                      \
  } while (false)

#define DEBUG_ASSERT(x)    \
  do {                     \
    if (GF_LAYERS_DEBUG) { \
      RUNTIME_ASSERT(x);   \
    }                      \
  } while (false)

#define DEBUG_ASSERT_MSG(x, ...)          \
  do {                                    \
    if (GF_LAYERS_DEBUG) {                \
      RUNTIME_ASSERT_MSG(x, __VA_ARGS__); \
    }                                     \
  } while (false)

#endif  // GF_LAYERS_LAYER_UTIL_LOGGING_H
