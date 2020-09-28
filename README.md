# gf-layers

A set of
[Vulkan layers](https://github.com/KhronosGroup/Vulkan-Loader/blob/master/loader/LoaderAndLayerInterface.md#layers)
that can be built for Linux, Mac, Windows, and Android.

This repository is a work-in-progress.

* WIP: VkLayer_GF_frame_counter: counts the frames per second.

This is not an officially supported Google product.

## Build


```sh
# Clone the repo.
git clone git@github.com:google/gf-layers.git
# Or:
git clone https://github.com/google/gf-layers.git

# Navigate to the root of the repo.
cd gf-layers

# Update and init submodules.
git submodule update --init

# Build using a recent version of CMake. Ensure `ninja` is on your PATH.
mkdir build
cd build

cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
cmake -DCMAKE_INSTALL_PREFIX=./install -P cmake_install.cmake --config Debug

# Layers are installed to `build/install/lib/`.
# You can skip the final "install" step and find the layers in `build/`.
```

## Test the layers

For example:

```sh
export VK_LAYER_PATH=/path/to/gf-layers/build
export VK_INSTANCE_LAYERS=VkLayer_GF_frame_counter
./vulkan_app
```

## Run checks and fixes

Only Bash on Linux is supported for now.
Ensure `ninja` is on your PATH and `python3` is at least Python 3.6.

```sh
# Launch the developer shell.
# 6GB+ (Clang 10, Android NDK, etc.) will be downloaded to `./temp/`.
# Scripts and Clang 10 will be added to your PATH.
./dev_shell.sh.template

# Run all checks.
check_all.sh

# (Re-)Generate files.
generate_files.py

# Fix C++ formatting via clang-format-10.
fix_format.sh
```

## [Coding conventions](docs/conventions.md)

## CLion

Use CLion to open the top-level CMakeLists.txt file.

Install and configure plugins:

* File Watchers (may already be installed).
  * The watcher task should already be under version control.

Add `dictionary.dic` as a custom dictionary (search for "Spelling" in Actions).
Do not add words via the "Quick Fixes" feature,
as the word will only be added to your personal dictionary.
Instead, manually add the word to `dictionary.dic`.
