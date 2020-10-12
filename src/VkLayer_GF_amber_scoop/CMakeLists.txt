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

set(VkLayer_GF_amber_scoop_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/VkLayer_GF_amber_scoop/amber_scoop_layer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/VkLayer_GF_amber_scoop/command_buffer_data.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/VkLayer_GF_amber_scoop/create_info_wrapper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/VkLayer_GF_amber_scoop/draw_call_tracker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/VkLayer_GF_amber_scoop/graphics_pipeline_data.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/VkLayer_GF_amber_scoop/vk_deep_copy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/VkLayer_GF_amber_scoop/vulkan_commands.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/amber_scoop_layer.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/command_buffer_data.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/draw_call_tracker.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics_pipeline_data.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vk_deep_copy.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan_commands.cc
    PARENT_SCOPE
)

