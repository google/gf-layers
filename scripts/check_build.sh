#!/usr/bin/env bash

# Copyright 2020 The gf-layers Project Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e
set -u
set -x

cd "${GF_LAYERS_REPO_ROOT}"
cd temp/

for CONFIG in Debug Release; do
  mkdir -p "build-${CONFIG}/"
  pushd "build-${CONFIG}/"

    cmake \
      -G Ninja \
      ../.. \
      "-DCMAKE_BUILD_TYPE=${CONFIG}" \
      "-DGF_LAYERS_USE_LLVM_LIBCPP=1"

    cmake --build . --config "${CONFIG}"

    check_compile_commands.sh compile_commands.json

  popd
done

# When building for Android, we override the SPIRV_FUZZ_PROTOC_COMMAND.
# This is used to generate some source files during the build.
# It is the protobuf::protoc CMake target by default,
# but we cannot run a protoc binary built for Android, so we must override it with
# a suitable host binary.
PROTOC_PATH="${GF_LAYERS_REPO_ROOT}/temp/build-Debug/third_party/protobuf/cmake/protoc"

# Android builds.
for CONFIG in Debug Release; do
  mkdir -p "build-android-${CONFIG}/"
  pushd "build-android-${CONFIG}/"

    cmake \
      -G Ninja \
      ../.. \
      "-DCMAKE_BUILD_TYPE=${CONFIG}" \
      "-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake" \
      "-DANDROID_ABI=arm64-v8a" \
      "-DANDROID_NATIVE_API_LEVEL=24" \
      "-DSPIRV_FUZZ_PROTOC_COMMAND=${PROTOC_PATH}"

    cmake --build . --config "${CONFIG}"

    check_compile_commands.sh compile_commands.json

  popd
done
