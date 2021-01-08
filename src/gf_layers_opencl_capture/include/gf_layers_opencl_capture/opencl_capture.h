// Copyright 2021 The gf-layers Project Authors
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

#ifndef GF_LAYERS_OPENCL_CAPTURE_OPENCL_CAPTURE_H
#define GF_LAYERS_OPENCL_CAPTURE_OPENCL_CAPTURE_H

#include <CL/opencl.h>

#include "gf_layers_opencl_capture/opencl_gen.h"

namespace gf_layers::opencl_capture {

struct GlobalData {
  GlobalData();

  OpenCLPointers opencl_pointers;
};

}  // namespace gf_layers::opencl_capture

#endif  // GF_LAYERS_OPENCL_CAPTURE_OPENCL_CAPTURE_H
