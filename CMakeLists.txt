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

option(
        GF_LAYERS_WARNINGS_AS_ERRORS
        "Enables warnings as errors for gf-layers targets, but only if gf-layers is the root project."
        ON)

option(
        GF_LAYERS_USE_LLVM_LIBCPP
        "Use LLVM's libc++ when using Clang, plus various other LLVM options, but only if gf-layers is the root project."
        OFF)


##
## Function: gf_layers_add_common(target)
##
## Adds common C++ compiler flags to |target|. Does nothing by default.
## Redefined below if we are the root project.
## This ensures we do not mess with compilation flags if we are a subproject.
##
function(gf_layers_add_common target)
endfunction()


# Only do the following if we are the root project being built.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Set some global compiler flags.
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # If using Clang then use LLVM's libc++ and various other options.
    if(GF_LAYERS_USE_LLVM_LIBCPP AND CMAKE_CXX_COMPILER_ID STREQUAL Clang AND NOT ANDROID)

        # TODO(paulthomson): We may want to build a custom version of Clang instead of messing with its options.

        # Use LLVM's libc++ instead of GCC's libstdc++.
        add_compile_options(-stdlib=libc++)
        # Enable link time optimization; object files will be LLVM bitcode files.
        add_compile_options(-flto)

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


    ##
    ## Function: gf_layers_add_common(target)
    ##
    ## Adds common C++ compiler flags (e.g. all warnings) to |target|.
    ##
    function(gf_layers_add_common target)

        if(MSVC)
            # Add all warnings.
            target_compile_options(${target} PRIVATE /Wall)

            # Warnings as errors.
            if(GF_LAYERS_WARNINGS_AS_ERRORS)
                target_compile_options(
                        ${target}
                        PRIVATE
                        # Treat angle bracket headers as external headers.
                        #/external:anglebrackets
                        # Disable warnings from external headers.
                        #/external:W0
                        # Disable warnings
                        /WX  # warnings as errors
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
                        /wd5045  # warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
                )
            endif()
        else()

            # Add all warnings.
            target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -Wno-unknown-pragmas)

            # -Weverything: Clang only.
            if(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
                target_compile_options(
                        ${target}
                        PRIVATE
                        -Wno-unknown-warning-option
                        -Wno-c++98-compat
                        -Wno-c++98-compat-pedantic
                        -Wno-padded
                        -Weverything)
            endif()

            # Warnings as errors.
            if(GF_LAYERS_WARNINGS_AS_ERRORS)
                target_compile_options(${target} PRIVATE -Werror)
            endif()

        endif()
    endfunction()
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
if(NOT TARGET gf_layers_vulkan_headers)
    add_library(gf_layers_vulkan_headers INTERFACE)
    if(NOT DEFINED GF_LAYERS_VULKAN_HEADERS_REPO_DIR)
        set(GF_LAYERS_VULKAN_HEADERS_REPO_DIR
                ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Vulkan-Headers)
    endif()
    if (IS_DIRECTORY ${GF_LAYERS_VULKAN_HEADERS_REPO_DIR})
        # We have the Vulkan-Headers directory, so use that.
        # We avoid using add_subdirectory() because we just need the headers.
        target_include_directories(
                gf_layers_vulkan_headers
                SYSTEM
                INTERFACE
                ${GF_LAYERS_VULKAN_HEADERS_REPO_DIR}/include)
    elseif(TARGET Vulkan::Headers)
        # The Vulkan::Headers target already exists (we must be a subdirectory
        # in a larger build), so we can use that.
        get_target_property(
                GF_LAYERS_TEMP_VULKAN_HEADERS
                Vulkan::Headers
                INTERFACE_INCLUDE_DIRECTORIES)

        target_include_directories(
                gf_layers_vulkan_headers
                SYSTEM
                INTERFACE
                ${GF_LAYERS_TEMP_VULKAN_HEADERS})
    else()
        message(
                FATAL_ERROR
                "Could not find Vulkan headers. "
                "Try fetching submodules or look at the "
                "gf_layers_vulkan_headers target in CMakeLists.txt.")
    endif()
endif()


##
## Target: gf_layers_layer_util (static library)
##
## Utility containers and functions.
##
add_subdirectory(src/gf_layers_layer_util)  # Provides gf_layers_layer_util_SOURCES.
add_library(gf_layers_layer_util STATIC ${gf_layers_layer_util_SOURCES})
gf_layers_add_common(gf_layers_layer_util)
target_include_directories(gf_layers_layer_util PUBLIC src/gf_layers_layer_util/include)
target_link_libraries(gf_layers_layer_util PUBLIC gf_layers_vulkan_headers)
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
## Function:
##
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
## Adds a shared library target (Vulkan layer) called |name|.
## Calls add_subdirectory("src/${name}/CMakeLists.txt"), which must provide a
## variable "name_SOURCES" containing the source files.
##
function(gf_layers_add_vulkan_layer name)
    add_subdirectory(src/${name})
    add_library(${name} SHARED ${${name}_SOURCES})
    target_include_directories(${name} PUBLIC src/${name}/include)
    gf_layers_add_common(${name})
    target_link_libraries(
            ${name}
            PRIVATE
            gf_layers_vulkan_headers
            gf_layers_layer_util)
    target_compile_features(${name} PUBLIC cxx_std_20)
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
    # TODO(paulthomson): There might be a better way doing this so that the .json file always ends up alongside the
    #  library without needing to specify "lib".
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
