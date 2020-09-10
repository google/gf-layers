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

cmake_minimum_required(VERSION 3.13)

project(gf-layers
    VERSION 1.0
    DESCRIPTION "Vulkan layers"
    LANGUAGES CXX)

# Note: this is not an option; it is used to set the default value for some options below.
# It is set to ON if gf-layers is the root project being built, and OFF otherwise.
set(GF_LAYERS_PROJECT_IS_ROOT OFF)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(GF_LAYERS_PROJECT_IS_ROOT ON)
endif()


# Options (i.e. cache variables) defined via "option" (if ON/OFF) or "set" (if not ON/OFF):
set(
    GF_LAYERS_VULKAN_HEADERS_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Vulkan-Headers/include
    CACHE
    PATH
    "Path to an include directory containing vulkan/vulkan.h, vulkan/vk_layer.h, etc.")

set(
    GF_LAYERS_SPIRV_HEADERS_REPO_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SPIRV-Headers
    CACHE
    PATH
    "Path to a https://github.com/KhronosGroup/SPIRV-Headers repo.")

set(
    GF_LAYERS_SPIRV_TOOLS_REPO_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SPIRV-Tools
    CACHE
    PATH
    "Path to a https://github.com/KhronosGroup/SPIRV-Tools repo.")

set(
    GF_LAYERS_PROTOBUF_REPO_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/protobuf
    CACHE
    PATH
    "Path to a https://github.com/protocolbuffers/protobuf repo.")

option(
    GF_LAYERS_USE_LLVM_LIBCPP
    "Use LLVM's libc++ when using Clang, plus various other LLVM options, but only if gf-layers is the root project."
    OFF)

option(
    GF_LAYERS_WARNINGS_AS_ERRORS
    "Enable warnings as errors for gf-layers targets."
    ${GF_LAYERS_PROJECT_IS_ROOT})

option(
    GF_LAYERS_WARNINGS_EXTRA
    "Enable extra warnings via compile flags for gf-layers targets."
    ${GF_LAYERS_PROJECT_IS_ROOT})

option(
    GF_LAYERS_WARNING_SUPPRESSIONS
    "Enable suppression of specific warnings for gf-layers targets."
    ${GF_LAYERS_PROJECT_IS_ROOT})


if(GF_LAYERS_PROJECT_IS_ROOT)

    # Set some global compiler flags.
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    # GF_LAYERS_USE_LLVM_LIBCPP=ON means we will statically link (if using non-Android Clang):
    #  - libc++
    #  - compiler-rt
    #  - libc++abi
    #  - libunwind
    # This is very useful for getting libraries that only depend on libc.
    if(
        GF_LAYERS_USE_LLVM_LIBCPP AND
        CMAKE_CXX_COMPILER_ID STREQUAL Clang AND
        NOT ANDROID)

        # TODO(paulthomson): We may want to build a custom version of Clang instead of messing with its options.

        # Use LLVM's libc++ instead of GCC's libstdc++.
        add_compile_options(-stdlib=libc++)

        # Link options:
        # Use LLVM's libc++ instead of GCC's libstdc++.
        add_link_options(-stdlib=libc++)
        # Use LLVM's lld as the linker.
        add_link_options(-fuse-ld=lld)
        # Statically link the C++ library (which is not GCC's libstdc++ because of the -stdlib flag above).
        add_link_options(-static-libstdc++)
        # Use LLVM's compiler-rt instead of libgcc, which will be statically linked.
        add_link_options(--rtlib=compiler-rt)

        # Include the path to LLVM's libc++abi.a
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} --print-file-name=libc++abi.a
            OUTPUT_VARIABLE GF_LAYERS_TEMP_CPP_ABI_PATH
        )
        string(REGEX REPLACE "\n" "" GF_LAYERS_TEMP_CPP_ABI_PATH ${GF_LAYERS_TEMP_CPP_ABI_PATH})

        add_link_options(${GF_LAYERS_TEMP_CPP_ABI_PATH})

        # Include the path to LLVM's libunwind.a
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} --print-file-name=libunwind.a
            OUTPUT_VARIABLE GF_LAYERS_TEMP_UNWIND_PATH
        )
        string(REGEX REPLACE "\n" "" GF_LAYERS_TEMP_UNWIND_PATH ${GF_LAYERS_TEMP_UNWIND_PATH})

        add_link_options(${GF_LAYERS_TEMP_UNWIND_PATH})
    endif()
endif()

# Layer manifest files (VkLayer_GF_foo_plat.json) have the |plat| suffix as
# windows, darwin, or linux. Here, we decide which suffix value to use. We will
# later copy the corresponding manifest file alongside the layer library.
if(WIN32)
    set(GF_LAYERS_MANIFEST_SYSTEM_SUFFIX "windows")
elseif(APPLE)
    set(GF_LAYERS_MANIFEST_SYSTEM_SUFFIX "darwin")
else()
    set(GF_LAYERS_MANIFEST_SYSTEM_SUFFIX "unix")
endif()


##
## Target: gf_layers_vulkan_headers
##
## Provides the Vulkan headers, which will usually be found in a Git submodule
## at: third_party/Vulkan-Headers.
##
## We avoid using find_package(Vulkan) because:
## - its goal is to find a Vulkan library, while we just need the headers.
## - we want the latest headers plus vulkan/vk_layer.h, which is not present in
##   the Android NDK.
##
add_library(gf_layers_vulkan_headers INTERFACE)
if(NOT IS_DIRECTORY ${GF_LAYERS_VULKAN_HEADERS_DIR})
    message(
        FATAL_ERROR
        "Could not find Vulkan-Headers at ${GF_LAYERS_VULKAN_HEADERS_DIR}. "
        "Try fetching submodules or set GF_LAYERS_VULKAN_HEADERS_DIR.")
endif()
# We avoid using add_subdirectory() because we just need the headers.
target_include_directories(
    gf_layers_vulkan_headers
    SYSTEM
    INTERFACE
    ${GF_LAYERS_VULKAN_HEADERS_DIR})


##
## Project: protobuf
##
## Provides targets: protobuf::protoc, protobuf::libprotobuf, and others.
##
if(NOT TARGET protobuf::protoc OR NOT TARGET protobuf::libprotobuf)
    if(NOT IS_DIRECTORY ${GF_LAYERS_PROTOBUF_REPO_DIR})
        message(
            FATAL_ERROR
            "Could not find protobuf at ${GF_LAYERS_PROTOBUF_REPO_DIR}. "
            "Try fetching submodules or set GF_LAYERS_PROTOBUF_REPO_DIR.")
    endif()
    set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build tests")
    set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libraries")
    set(protobuf_MSVC_STATIC_RUNTIME OFF CACHE BOOL "Link static runtime libraries")
    set(protobuf_WITH_ZLIB OFF CACHE BOOL "Build with zlib support")
    add_subdirectory(${GF_LAYERS_PROTOBUF_REPO_DIR}/cmake EXCLUDE_FROM_ALL)
endif()

##
## Project: SPIRV-Tools
##
## Provides targets: SPIRV-Tools-opt, SPIRV-Tools-fuzz, and others.
##
if(NOT TARGET SPIRV-Tools-opt)
    if(NOT IS_DIRECTORY ${GF_LAYERS_SPIRV_TOOLS_REPO_DIR})
        message(
            FATAL_ERROR
            "Could not find SPIRV-Tools at ${GF_LAYERS_SPIRV_TOOLS_REPO_DIR}. "
            "Try fetching submodules or set GF_LAYERS_SPIRV_TOOLS_REPO_DIR.")
    endif()
    # Requires SPIRV-Headers.
    if(NOT IS_DIRECTORY ${GF_LAYERS_SPIRV_HEADERS_REPO_DIR})
        message(
            FATAL_ERROR
            "Could not find SPIRV-Headers at ${GF_LAYERS_SPIRV_HEADERS_REPO_DIR}. "
            "Try fetching submodules or set GF_LAYERS_SPIRV_HEADERS_REPO_DIR.")
    endif()

    # The presence of SPIRV-Headers_SOURCE_DIR would normally imply that
    # add_subdirectory(/path/to/SPIRV-Tools/) has been executed.
    # However, SPIRV-Tools does not actually use any targets from SPIRV-Headers;
    # it only uses SPIRV-Headers_SOURCE_DIR. Thus, we can avoid using
    # add_subdirectory(...) by setting this variable.
    set(SPIRV-Headers_SOURCE_DIR ${GF_LAYERS_SPIRV_HEADERS_REPO_DIR})

    # Enable the SPIRV-Tools-fuzz target, which is disabled by default.
    set(SPIRV_BUILD_FUZZER ON CACHE BOOL "Build spirv-fuzz")

    add_subdirectory(${GF_LAYERS_SPIRV_TOOLS_REPO_DIR} EXCLUDE_FROM_ALL)
endif()
# The SPIRV-Tools-fuzz target is disabled by default; check that it is indeed
# enabled.
if(NOT TARGET SPIRV-Tools-fuzz)
    message(
        FATAL_ERROR
        "The SPIRV-Tools-fuzz target is not enabled; make sure you have enabled the required option in SPIRV-Tools.")
endif()


##
## Target: gf_layers_spirv_fuzz
##
## Provides the SPIRV-Tools-fuzz target with extra headers.
##
add_library(gf_layers_spirv_fuzz INTERFACE)

# Targets that link gf_layers_spirv_fuzz should get SPIRV-Tools-fuzz.
# The following command: target_link_libraries(gf_layers_spirv_fuzz PUBLIC SPIRV-Tools-fuzz)
# is not supported by CMake for INTERFACE libraries, but we can achieve it manually:
set_property(
    TARGET gf_layers_spirv_fuzz
    APPEND
    PROPERTY INTERFACE_LINK_LIBRARIES
    SPIRV-Tools-fuzz
)
# We need to add the SPIRV-Tools root (source and binary directories) as
# include paths, because we need to use internal headers from SPIRV-Tools.
# CMake adds variables <PROJECT-NAME>_SOURCE_DIR and <PROJECT-NAME>_BINARY_DIR
# when it sees the project(<PROJECT-NAME> ...) command, so we can use these.
if(NOT spirv-tools_SOURCE_DIR OR NOT spirv-tools_BINARY_DIR)
    message(
        FATAL_ERROR
        "Could not find CMake variables spirv-tools_SOURCE_DIR and/or spirv-tools_BINARY_DIR. "
        "We require access to internal SPIRV-Tools headers, so the full source is required "
        "(as opposed to an installed version of SPIRV-Tools).")
endif()
target_include_directories(
    gf_layers_spirv_fuzz
    INTERFACE
    ${spirv-tools_SOURCE_DIR}
    ${spirv-tools_BINARY_DIR})


##
## The targets that follow are all gf-layers targets.
## We now conditionally set some compile options related to warnings.
## Most of these will only be set if gf-layers is the root project being built.
## Using "add_*" functions is generally discouraged, but is suitable for
## "global" compile options. These will propagate to subdirectories via
## add_subdirectory(...) and will not be PUBLIC compile options (i.e. they
## will NOT propagate to targets above this project that link with these
## targets.
##

if(GF_LAYERS_WARNINGS_AS_ERRORS)
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

if(GF_LAYERS_WARNINGS_EXTRA)
    if(MSVC)
        add_compile_options(/Wall)
    else()
        add_compile_options(-Wall -Wextra -pedantic)
        if(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
            add_compile_options(-Weverything)
        endif()
    endif()
endif()

if(GF_LAYERS_WARNING_SUPPRESSIONS)
    if(MSVC)
        add_compile_options(
            # Treat angle bracket headers as external headers.
            #/external:anglebrackets

            # Disable warnings from external headers.
            #/external:W0

            # Disable specific warnings:
            /wd4068  # warning C4068: unknown pragma
            /wd4820  # warning C4820: 'xxx': 'x' bytes padding added after data member 'xxx'
            /wd4191  # warning C4191: 'reinterpret_cast': unsafe conversion from 'x' to 'x'
            /wd4625  # copy constructor was implicitly defined as deleted
            /wd5026  # move constructor was implicitly defined as deleted
            /wd4626  # assignment operator was implicitly defined as deleted
            /wd5027  # move assignment operator was implicitly defined as deleted
            /wd4514  # warning C4514: 'x': unreferenced inline function has been removed
            /wd4711  # warning C4711: function 'x' selected for automatic inline expansion (informational)
            /wd4710  # warning C4710: 'x': function not inlined (informational)
            /wd4996  # warning C4996: 'getenv': This function or variable may be unsafe.
        )
    else()
        add_compile_options(
            -Wno-unknown-pragmas
            -Wno-unknown-warning-option
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-padded
        )
    endif()
endif()


##
## Target: gf_layers_layer_util (static library)
##
## Utility containers and functions.
##
add_subdirectory(src/gf_layers_layer_util EXCLUDE_FROM_ALL)  # Provides gf_layers_layer_util_SOURCES.
add_library(gf_layers_layer_util STATIC ${gf_layers_layer_util_SOURCES})
target_include_directories(gf_layers_layer_util PUBLIC src/gf_layers_layer_util/include)
target_link_libraries(gf_layers_layer_util PUBLIC gf_layers_vulkan_headers)
target_compile_features(gf_layers_layer_util PUBLIC cxx_std_17)
# We do not want Vulkan function prototypes. Our util library must not call
# Vulkan functions directly.
target_compile_definitions(gf_layers_layer_util PRIVATE VK_NO_PROTOTYPES)
# Must enable position independent code so we can link this into shared libraries.
set_target_properties(
        gf_layers_layer_util
        PROPERTIES
        POSITION_INDEPENDENT_CODE ON)
# On Android, we need to link with "liblog.so".
if(ANDROID)
    find_library(GF_LAYERS_TEMP_ANDROID_LOG_LIB_PATH log)
    target_link_libraries(gf_layers_layer_util PUBLIC ${GF_LAYERS_TEMP_ANDROID_LOG_LIB_PATH})
endif()


##
## Function: gf_layers_hide_exports(target, source_dir)
##
## Hides all functions in |target| except for those we explicitly want to
## export. Uses various linker scripts that must be present in |source_dir|.
##
function(gf_layers_hide_exports target source_dir)
    if(MSVC)
        target_link_options(${target} PRIVATE "/DEF:${source_dir}\\${target}.def")
        set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS "${source_dir}/${target}.def")
    elseif(APPLE)
        target_link_options(${target} PRIVATE -exported_symbols_list "${source_dir}/${target}.exports")
        set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS "${source_dir}/${target}.exports")
    else()
        if(ANDROID)
            target_link_options(${target} PRIVATE "LINKER:--version-script=${source_dir}/${target}_android.lds")
            set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS "${source_dir}/${target}_android.lds")
        else()
            target_link_options(${target} PRIVATE "LINKER:--version-script=${source_dir}/${target}.lds")
            set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS "${source_dir}/${target}.lds")
        endif()


        target_link_options(${target} PRIVATE "LINKER:-Bsymbolic")
        target_link_options(${target} PRIVATE "LINKER:--no-undefined")

    endif()
endfunction()


##
## Function: gf_layers_add_vulkan_layer(name)
##
## Adds a shared library target Vulkan layer called |name|.
## Calls add_subdirectory("src/${name}/CMakeLists.txt"), which must provide a
## variable "name_SOURCES" containing the source files.
##
function(gf_layers_add_vulkan_layer name)
    add_subdirectory(src/${name})
    add_library(${name} SHARED ${${name}_SOURCES})
    target_include_directories(${name} PUBLIC src/${name}/include)
    target_link_libraries(
            ${name}
            PRIVATE
            gf_layers_vulkan_headers
            gf_layers_layer_util)
    target_compile_features(${name} PUBLIC cxx_std_17)
    # We do not want Vulkan function prototypes, as layers must not call any
    # Vulkan functions directly.
    target_compile_definitions(${name} PRIVATE VK_NO_PROTOTYPES)
    # Use linker scripts to hide all functions except those we explicitly want to export.
    gf_layers_hide_exports(${name} ${CMAKE_SOURCE_DIR}/src/${name})
    # Copy the JSON manifest alongside the library.
    add_custom_command(
            TARGET ${name}
            POST_BUILD
            COMMAND
                ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/src/${name}/${name}_${GF_LAYERS_MANIFEST_SYSTEM_SUFFIX}.json
                $<TARGET_FILE_DIR:${name}>)
    # Support installing the layer target.
    install(TARGETS ${name} LIBRARY DESTINATION lib)
    # TODO(paulthomson): There might be a better way of doing this so that the
    #  .json file always ends up alongside the library without needing to
    #  specify "lib".
    install(
            FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/${name}/${name}_${GF_LAYERS_MANIFEST_SYSTEM_SUFFIX}.json
            DESTINATION lib)

endfunction()


##
## Target: VkLayer_GF_frame_counter
##
## A Vulkan layer for counting frames per second.
##
gf_layers_add_vulkan_layer(VkLayer_GF_frame_counter)

##
## Target: VkLayer_GF_amber_scoop
##
## A Vulkan layer for creating amber files from draw calls.
##
gf_layers_add_vulkan_layer(VkLayer_GF_amber_scoop)

##
## Target: VkLayer_GF_shader_fuzzer
##
## A Vulkan layer for fuzzing SPIR-V shaders using spirv-fuzz.
##
gf_layers_add_vulkan_layer(VkLayer_GF_shader_fuzzer)
target_link_libraries(VkLayer_GF_shader_fuzzer PRIVATE gf_layers_spirv_fuzz)

