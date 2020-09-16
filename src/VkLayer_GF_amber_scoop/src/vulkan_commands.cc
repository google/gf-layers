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

#include "VkLayer_GF_amber_scoop/vulkan_commands.h"

namespace gf_layers::amber_scoop_layer {

// At least one of the virtual functions from each of "Cmd*"-class must be
// defined in this file (instead of the header file) to avoid vtable to be
// emitted in every translation unit (-Wweak-vtables error).

Cmd::~Cmd() = default;

CmdBeginRenderPass* CmdBeginRenderPass::AsBeginRenderPass() { return this; }
const CmdBeginRenderPass* CmdBeginRenderPass::AsBeginRenderPass() const {
  return this;
}

CmdBindPipeline* CmdBindPipeline::AsBindPipeline() { return this; }
const CmdBindPipeline* CmdBindPipeline::AsBindPipeline() const { return this; }

CmdDraw* CmdDraw::AsDraw() { return this; }
const CmdDraw* CmdDraw::AsDraw() const { return this; }

CmdDrawIndexed* CmdDrawIndexed::AsDrawIndexed() { return this; }
const CmdDrawIndexed* CmdDrawIndexed::AsDrawIndexed() const { return this; }

}  // namespace gf_layers::amber_scoop_layer
