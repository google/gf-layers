#!/usr/bin/env python3

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


import argparse
import os
import re
import sys

from pathlib import Path
from typing import List, Any

doc = """
Generates the layer manifest JSON files and linker scripts from the C++ source
code in each VK_LAYER_* directory.

The C++ files must include commented layer properties like this:

    "VK_LAYER_GF_frame_counter",    // layerName
    VK_MAKE_VERSION(1U, 1U, 130U),  // specVersion NOLINT(hicpp-signed-bitwise)
    1,                              // implementationVersion
    "Frame counter layer.",         // description

And exports like this:

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
VK_LAYER_GF_frame_counterNegotiateLoaderLayerInterfaceVersion(

"""


def not_none(x):
    assert x is not None
    return x


def log(s: str):
    print(s, file=sys.stderr, flush=True)


class LayerProperties:
    def __init__(self):
        self.layer_name = None
        self.spec_version = None
        self.implementation_version = None
        self.description = None


def generate_manifests(layer_dir: Path, layer_properties: LayerProperties, check_only: bool) -> bool:
    result = True

    src_files = [f for f in layer_dir.glob("src/*.cc")]
    for src_file in src_files:
        with src_file.open("r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                if "// layerName" in line:
                    layer_properties.layer_name = re.search(r'"([^"]*)",', line).group(1)
                elif "// specVersion" in line:
                    match = re.search(r'VK_MAKE_VERSION\((\d+)U, (\d+)U, (\d+)U\)', line)
                    layer_properties.spec_version = f"{match.group(1)}.{match.group(2)}.{match.group(3)}"
                elif "// implementationVersion" in line:
                    layer_properties.implementation_version = re.search(r' (\d+),', line).group(1)
                elif "// description" in line:
                    layer_properties.description = re.search(r'"([^"]*)"', line).group(1)

    # Write out the manifest file for each platform.
    for platform in ["unix", "darwin", "windows"]:
        if platform == "unix":
            library_path = f"./lib{not_none(layer_properties.layer_name)}.so"
        elif platform == "darwin":
            library_path = f"./lib{not_none(layer_properties.layer_name)}.dylib"
        elif platform == "windows":
            # JSON output is similar to: ".\\aaa.dll"
            # I.e. the backslash must be escaped in JSON.
            # We don't need to double-escape below because we use a raw string literal.
            library_path = fr".\\{not_none(layer_properties.layer_name)}.dll"
        else:
            assert False, "Unsupported platform"

        manifest_file = layer_dir / f"{not_none(layer_properties.layer_name)}_{platform}.json"
        # Braces have to be escaped.
        manifest_contents = f'''{{
  "file_format_version" : "1.1.2",
  "layers": [
    {{
      "name": "{not_none(layer_properties.layer_name)}",
      "type": "GLOBAL",
      "library_path": "{library_path}",
      "api_version" : "{not_none(layer_properties.spec_version)}",
      "implementation_version" : "{not_none(layer_properties.implementation_version)}",
      "description" : "{not_none(layer_properties.description)}",
      "functions": {{
        "vkGetDeviceProcAddr": "{not_none(layer_properties.layer_name)}GetDeviceProcAddr",
        "vkGetInstanceProcAddr": "{not_none(layer_properties.layer_name)}GetInstanceProcAddr",
        "vkNegotiateLoaderLayerInterfaceVersion": "{not_none(layer_properties.layer_name)}NegotiateLoaderLayerInterfaceVersion"
      }}
    }}
  ],
  "enable_environment": {{
    "{not_none(layer_properties.layer_name)}_V{not_none(layer_properties.implementation_version)}_ENABLE": "1"
  }},
  "disable_environment": {{
    "{not_none(layer_properties.layer_name)}_V{not_none(layer_properties.implementation_version)}_DISABLE": "1"
  }}
}}
'''
        # End of manifest_contents.
        log(f"Processing: {str(manifest_file)}")
        if check_only:
            if ((not manifest_file.is_file()) or
                    manifest_file.read_text(encoding="utf-8", errors="ignore") != manifest_contents):
                log("Error: manifest file needs to be re-generated")
                result = False
        else:
            manifest_file.write_text(manifest_contents, encoding="utf-8", errors="ignore")

    return result


ANDROID_EXPORTS = [
    "vkEnumerateInstanceLayerProperties",
    "vkEnumerateDeviceLayerProperties",
    "vkEnumerateInstanceExtensionProperties",
    "vkEnumerateDeviceExtensionProperties",
]


def generate_linker_scripts(layer_dir: Path, layer_name: str, check_only: bool) -> bool:
    result = True

    # Collect the exported functions by scanning all source files.
    non_android_exported_functions: List[str] = []
    src_files = [f for f in layer_dir.glob("src/*.cc")]
    for src_file in src_files:
        contents = src_file.read_text(encoding="utf-8", errors="ignore")
        function_names = re.findall(
            r"VK_LAYER_EXPORT(?:\s|\n)+VKAPI_ATTR(?:\s|\n)+\w+(?:\s|\n)+VKAPI_CALL(?:\s|\n)+(\w+)\(",
            contents,
        )
        for function_name in function_names:
            if function_name not in ANDROID_EXPORTS:
                non_android_exported_functions.append(function_name)

    # Write out the linker scripts.

    # .def (used by MSVC)
    def_path = layer_dir / f"{layer_name}.def"
    log(f"Processing: {str(def_path)}")
    def_contents = f"; Linker script for MSVC.\n"
    def_contents += f"; Generated file; do not edit.\n"
    def_contents += f"LIBRARY {layer_name}\n"
    def_contents += "EXPORTS\n"
    for function in non_android_exported_functions:
        def_contents += f"    {function}\n"
    if check_only:
        if not def_path.is_file() or def_path.read_text(encoding="utf-8", errors="ignore") != def_contents:
            log("Error: file needs to be re-generated")
            result = False
    else:
        def_path.write_text(def_contents, encoding="utf-8", errors="ignore")

    # .exports (used for Apple targets)
    exports_path = layer_dir / f"{layer_name}.exports"
    log(f"Processing: {str(exports_path)}")
    exports_contents = "# Linker script for Apple.\n"
    exports_contents += f"# Generated file; do not edit.\n"
    for function in non_android_exported_functions:
        exports_contents += f"_{function}\n"
    if check_only:
        if not exports_path.is_file() or exports_path.read_text(encoding="utf-8", errors="ignore") != exports_contents:
            log("Error: file needs to be re-generated")
            result = False
    else:
        exports_path.write_text(exports_contents, encoding="utf-8", errors="ignore")

    # .lds (used for Linux targets)
    lds_path = layer_dir / f"{layer_name}.lds"
    log(f"Processing: {str(lds_path)}")
    lds_contents = "# Linker script for Linux.\n"
    lds_contents += f"# Generated file; do not edit.\n"
    lds_contents += "{\n"
    lds_contents += "global:\n"
    for function in non_android_exported_functions:
        lds_contents += f"    {function};\n"
    lds_contents += "local:\n"
    lds_contents += "    *;\n"
    lds_contents += "};\n"
    if check_only:
        if not lds_path.is_file() or lds_path.read_text(encoding="utf-8", errors="ignore") != lds_contents:
            log("Error: file needs to be re-generated")
            result = False
    else:
        lds_path.write_text(lds_contents, encoding="utf-8", errors="ignore")

    # _android.lds (used for Android targets)
    lds_android_path = layer_dir / f"{layer_name}_android.lds"
    log(f"Processing: {str(lds_android_path)}")
    lds_android_contents = "# Linker script for Android.\n"
    lds_android_contents += f"# Generated file; do not edit.\n"
    lds_android_contents += "{\n"
    lds_android_contents += "global:\n"
    for function in non_android_exported_functions:
        lds_android_contents += f"    {function};\n"
    # Just for Android:
    lds_android_contents += "    # Introspection functions must be exported on Android.\n"
    for function in ANDROID_EXPORTS:
        lds_android_contents += f"    {function};\n"
    lds_android_contents += "local:\n"
    lds_android_contents += "    *;\n"
    lds_android_contents += "};\n"
    if check_only:
        if (not lds_android_path.is_file()
                or lds_android_path.read_text(encoding="utf-8", errors="ignore") != lds_android_contents):
            log("Error: file needs to be re-generated")
            result = False
    else:
        lds_android_path.write_text(lds_android_contents, encoding="utf-8", errors="ignore")

    return result


def main(args) -> None:

    raw_help_formatter: Any = argparse.RawDescriptionHelpFormatter

    parser = argparse.ArgumentParser(
        description=doc,
        formatter_class=raw_help_formatter,
    )

    parser.add_argument(
        "--check_only",
        help="Don't change any files; just check that they would not change. If the files would have changed then "
             "exits with error status 2.",
        action="store_true",
    )

    parsed_args = parser.parse_args(args[1:])

    check_only: bool = parsed_args.check_only

    os.chdir(os.environ["GF_LAYERS_REPO_ROOT"])

    layer_dirs = [d for d in Path().glob("src/VK_LAYER_*/") if d.is_dir()]

    result = True

    # For each layer:
    for layer_dir in layer_dirs:
        layer_properties = LayerProperties()
        result &= generate_manifests(layer_dir, layer_properties, check_only)
        result &= generate_linker_scripts(layer_dir, layer_name=layer_properties.layer_name, check_only=check_only)

    if not result:
        raise AssertionError("Checks failed. See above.")


if __name__ == "__main__":
    main(sys.argv)
