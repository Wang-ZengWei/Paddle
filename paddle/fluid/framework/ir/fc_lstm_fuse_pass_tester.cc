// Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.
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

#include "paddle/fluid/framework/ir/fc_lstm_fuse_pass_tester.h"

namespace paddle::framework::ir::fc_lstm_test {

TEST(FcLstmFusePass, basic) {
  std::unique_ptr<ir::Graph> graph = PrepareGraph();
  auto pass = PassRegistry::Instance().Get("fc_lstm_fuse_pass");
  pass->Set("use_gpu", new bool(false));
  graph->Set("__param_scope__", CreateParamScope());
  int num_nodes_before = static_cast<int>(graph->Nodes().size());
  int num_lstm_nodes_before = GetNumOpNodes(graph, "lstm");
  VLOG(3) << DebugString(graph);

  graph.reset(pass->Apply(graph.release()));
  int num_nodes_after = static_cast<int>(graph->Nodes().size());
  int num_fusion_lstm_nodes_after = GetNumOpNodes(graph, "fusion_lstm");
  VLOG(3) << DebugString(graph);

  PADDLE_ENFORCE_EQ(
      num_nodes_before,
      num_nodes_after - 6,
      phi::errors::PreconditionNotMet("The number of nodes before and after "
                                      "the fuse does not meet expectations"));
  PADDLE_ENFORCE_EQ(
      num_fusion_lstm_nodes_after,
      2,
      phi::errors::PreconditionNotMet("The number of lstm nodes before the "
                                      "fuse does not meet expectations"));
  PADDLE_ENFORCE_EQ(
      num_lstm_nodes_before,
      num_fusion_lstm_nodes_after,
      phi::errors::PreconditionNotMet("The number of fusion_gru nodes does "
                                      "not meet expectations after fuse"));
}
}  // namespace paddle::framework::ir::fc_lstm_test

USE_PASS(fc_lstm_fuse_pass);
