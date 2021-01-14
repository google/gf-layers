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

import xml.etree.ElementTree as ET

from pathlib import Path
from typing import List, Any, Optional

doc = """
Generates...
"""


def append(string_list: List[str], text: Optional[str]) -> None:
    if not text:
        return
    string_list.append(text.strip())


def append_space(string_list: List[str], text: Optional[str]) -> None:
    if not text:
        return
    string_list.append(text.strip())
    string_list.append(" ")


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

    xml_path = Path() / "third_party" / "OpenCL" / "cl.xml"

    tree = ET.parse(xml_path)
    registry = tree.getroot()
    commands = None
    for child in registry:  # type: ET.Element
        if child.tag == "commands":
            commands = child
            break

    assert commands

    typedef_lines: List[str] = []
    struct_lines: List[str] = []
    load_function_lines: List[str] = []
    prototype_lines: List[str] = []

    function_names: List[str] = []

    for command in commands:  # type: ET.Element
        assert command.tag == "command"
        proto = command[0]
        assert proto.tag == "proto"
        # <command>
        #     <proto><type>void</type>*                           <name>clHostMemAllocINTEL</name></proto>
        #     <param><type>cl_context</type>                      <name>context</name></param>
        #     <param>const <type>cl_mem_properties_intel</type>*  <name>properties</name></param>
        #     <param><type>size_t</type>                          <name>size</name></param>
        #     <param><type>cl_uint</type>                         <name>alignment</name></param>
        #     <param><type>cl_int</type>*                         <name>errcode_ret</name></param>
        # </command>

        # Struct line:
        #   PFN_clHostMemAllocINTEL clHostMemAllocINTEL = nullptr;
        # Load function line:
        # LoadFunction(clHostMemAllocINTEL);

        name = proto[1]
        assert name.tag == "name"

        struct_lines.append(f"  PFN_{name.text} {name.text} = nullptr;")
        load_function_lines.append(f"LoadFunction({name.text});")
        function_names.append(name.text)

    for command in commands:  # type: ET.Element
        assert command.tag == "command"
        proto = command[0]
        assert proto.tag == "proto"
        # <command>
        #     <proto><type>void</type>*                           <name>clHostMemAllocINTEL</name></proto>
        #     <param><type>cl_context</type>                      <name>context</name></param>
        #     <param>const <type>cl_mem_properties_intel</type>*  <name>properties</name></param>
        #     <param><type>size_t</type>                          <name>size</name></param>
        #     <param><type>cl_uint</type>                         <name>alignment</name></param>
        #     <param><type>cl_int</type>*                         <name>errcode_ret</name></param>
        # </command>

        # typedef void*(CL_API_CALL* PFN_clHostMemAllocINTEL)(
        #   cl_context /*context*/, ..., const cl_mem_properties_intel* /*properties*/, ...)

        # using PFN_clCreateEventFromEGLSyncKHR =
        #     std::add_pointer<CL_API_ENTRY cl_event CL_API_CALL(
        #         cl_context /*context*/, CLeglSyncKHR /*sync*/,
        #         CLeglDisplayKHR /*display*/, cl_int* /*errcode_ret*/)
        #                          CL_API_SUFFIX__VERSION_1_0>::type;

        typedef_parts: List[str] = list()

        append_space(typedef_parts, "using")

        assert len(proto) == 2
        return_type = proto[0]
        assert return_type.tag == "type"
        name = proto[1]
        assert name.tag == "name" and not name.tail

        append_space(typedef_parts, f"PFN_{name.text}")
        append_space(typedef_parts, "=")
        append_space(typedef_parts, "std::add_pointer<CL_API_ENTRY")

        append_space(typedef_parts, proto.text)
        append(typedef_parts, return_type.text)
        append(typedef_parts, return_type.tail)
        typedef_parts.append(" ")

        append_space(typedef_parts, "CL_API_CALL(")

        first = True

        for param in command[1:]:
            assert param.tag == "param"

            if not first:
                typedef_parts.append(", ")
            else:
                first = False

            assert len(param) in [1, 2]

            if len(param) == 1 and param[0].tag == "type":
                # Special case (no parameters): cl_int clUnloadCompiler(void);
                assert param[0].text == "void"
                break

            if len(param) == 1:
                # Special case that we can't properly handle yet. Just dump all the text.

                assert param[0].tag == "name"

                # E.g.
                # <param>void (CL_CALLBACK*
                #      <name>pfn_free_func</name>)(cl_command_queue queue, cl_uint num_svm_pointers,
                #      void* svm_pointers[], void* user_data)</param>

                text = " ".join(param.itertext())
                text = re.sub(" +", " ", text)
                append(typedef_parts, text)
                continue

            # E.g. const cl_mem_properties_intel* /*properties*/,

            # E.g. const
            append_space(typedef_parts, param.text)

            param_type = param[0]
            assert param_type.tag == "type"
            name = param[1]
            assert name.tag == "name"

            # E.g. cl_mem_properties_intel
            append(typedef_parts, param_type.text)

            # E.g. * or None
            append(typedef_parts, param_type.tail)

            # E.g. "[]"
            append(typedef_parts, name.tail)
            assert (not name.tail) or name.tail == "[]"

            typedef_parts.append(" ")

            append(typedef_parts, f"/*{name.text}")
            append(typedef_parts, name.tail)
            append(typedef_parts, "*/")

        append(typedef_parts, ")>::type;")

        typedef_lines.append("".join(typedef_parts))

    for command in commands:  # type: ET.Element
        assert command.tag == "command"
        proto = command[0]
        assert proto.tag == "proto"
        # <command>
        #     <proto><type>void</type>*                           <name>clHostMemAllocINTEL</name></proto>
        #     <param><type>cl_context</type>                      <name>context</name></param>
        #     <param>const <type>cl_mem_properties_intel</type>*  <name>properties</name></param>
        #     <param><type>size_t</type>                          <name>size</name></param>
        #     <param><type>cl_uint</type>                         <name>alignment</name></param>
        #     <param><type>cl_int</type>*                         <name>errcode_ret</name></param>
        # </command>

        # CL_API_ENTRY cl_int CL_API_CALL clEnqueueReadBuffer(
        #
        #     cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read,
        #     size_t offset, size_t size, void* ptr, cl_uint num_events_in_wait_list,
        #     const cl_event* event_wait_list,
        #     cl_event* event) CL_API_SUFFIX__VERSION_1_0 {
        #
        #   LOG("clEnqueueReadBuffer");
        #   return? gf_layers::opencl_capture::GetGlobalData()
        #       ->opencl_pointers.clEnqueueReleaseGLObjects();
        # }

        prototype_parts: List[str] = list()

        append_space(prototype_parts, "CL_API_ENTRY")
        append_space(prototype_parts, proto.text)
        assert len(proto) == 2
        return_type = proto[0]
        assert return_type.tag == "type"
        append(prototype_parts, return_type.text)
        append(prototype_parts, return_type.tail)
        function_name = proto[1]
        assert function_name.tag == "name" and not function_name.tail
        prototype_parts.append(" ")
        append_space(prototype_parts, "CL_API_CALL")
        append(prototype_parts, f"{function_name.text}(\n")

        first = True

        for param in command[1:]:
            assert param.tag == "param"

            if not first:
                prototype_parts.append(", ")
            else:
                first = False

            assert len(param) in [1, 2]

            if len(param) == 1 and param[0].tag == "type":
                # Special case (no parameters): cl_int clUnloadCompiler(void);
                assert param[0].text == "void"
                break

            if len(param) == 1:
                # Special case that we can't properly handle yet. Just dump all the text.

                assert param[0].tag == "name"

                # E.g.
                # <param>void (CL_CALLBACK*
                #      <name>pfn_free_func</name>)(cl_command_queue queue, cl_uint num_svm_pointers,
                #      void* svm_pointers[], void* user_data)</param>

                text = " ".join(param.itertext())
                text = re.sub(" +", " ", text)
                append(prototype_parts, text)
                continue

            # E.g. const cl_mem_properties_intel* properties,

            # E.g. const
            append_space(prototype_parts, param.text)

            param_type = param[0]
            assert param_type.tag == "type"
            name = param[1]
            assert name.tag == "name"

            # E.g. cl_mem_properties_intel
            append(prototype_parts, param_type.text)

            # E.g. * or None
            append(prototype_parts, param_type.tail)

            assert (not name.tail) or name.tail == "[]"

            prototype_parts.append(" ")

            append(prototype_parts, name.text)
            # E.g. []
            append(prototype_parts, name.tail)

        append(prototype_parts, ") {")

        prototype_parts.append(f"""\n  LOG("{function_name.text}");""")
        prototype_parts.append(f"\n  ")

        if return_type.text != "void" or return_type.tail.strip():
            # Return type is NOT "void" or it is void but there is something else after void (i.e. void*).
            # Thus, we need a return statement.
            prototype_parts.append("return ")

        prototype_parts.append(f"gf_layers::opencl_capture::GetGlobalData()->opencl_pointers.{function_name.text}(")

        # Function call parameters.

        first = True

        for param in command[1:]:
            assert param.tag == "param"

            if not first:
                prototype_parts.append(", ")
            else:
                first = False

            assert len(param) in [1, 2]

            if len(param) == 1 and param[0].tag == "type":
                # Special case (no parameters): cl_int clUnloadCompiler(void);
                assert param[0].text == "void"
                break

            name = param[0]
            if name.tag != "name":
                name = param[1]
            assert name.tag == "name"
            append(prototype_parts, name.text)

        prototype_parts.append(");\n}\n\n")

        prototype_lines.append("".join(prototype_parts))

    typedef_out_path = Path() / "temp" / "opencl_typedefs_gen.txt"
    struct_out_path = Path() / "temp" / "opencl_struct_fields_gen.txt"
    load_function_out_path = Path() / "temp" / "opencl_load_functions_gen.txt"
    prototypes_out_path = Path() / "temp" / "opencl_prototypes_gen.txt"

    typedef_out_path.write_text("\n".join(typedef_lines), encoding="utf-8", errors="ignore")
    struct_out_path.write_text("\n".join(struct_lines), encoding="utf-8", errors="ignore")
    load_function_out_path.write_text("\n".join(load_function_lines), encoding="utf-8", errors="ignore")
    prototypes_out_path.write_text("\n".join(prototype_lines), encoding="utf-8", errors="ignore")

    linker_script_linux_path = Path() / "src" / "gf_layers_opencl_capture" / "gf_layers_opencl_capture.lds"

    # noinspection PyListCreation
    linker_script_linux = []

    linker_script_linux.append("# Linker script for Linux.")
    linker_script_linux.append("# Generated file; do not edit.")
    linker_script_linux.append("{")
    linker_script_linux.append("global:")
    for name in function_names:
        linker_script_linux.append(f"    {name};")
    linker_script_linux.append("local:")
    linker_script_linux.append("    *;")
    linker_script_linux.append("};")

    linker_script_linux_path.write_text("\n".join(linker_script_linux), encoding="utf-8", errors="ignore")


if __name__ == "__main__":
    main(sys.argv)
