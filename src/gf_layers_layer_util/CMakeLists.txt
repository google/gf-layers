# Copyright 2020 The gf-layers Project Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(gf_layers_layer_util_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gf_layers_layer_util/logging.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gf_layers_layer_util/util.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util.cc
    PARENT_SCOPE
)
