/*******************************************************************************
 * Copyright 2020-2022 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

#ifndef BACKEND_GRAPH_COMPILER_CORE_SRC_OPS_TEMPLATES_CONV_BWD_HPP
#define BACKEND_GRAPH_COMPILER_CORE_SRC_OPS_TEMPLATES_CONV_BWD_HPP

#include <memory>
#include <tuple>
#include <vector>
#include <ops/body_generator.hpp>
namespace sc {

namespace ops {

struct conv_bwd_config {
  int K_block;
  int C_block;
  int tile_p;
  int tile_q;
  int loop_sched;
};

class gen_conv_bwd : public body_generator_t<conv_bwd_config> {
public:
  sc_dims stride_;
  sc_dims padding_;
  struct op_params_t {
    static constexpr int in_fwd_output = 0;
    static constexpr int in_weight = 1;
    static constexpr int out_del_input = 0;
  };
  using parent = body_generator_t<conv_bwd_config>;
  using parent::generate;

  std::tuple<int, int> get_output_shape() {
    return std::tuple<int, int> {get_output_dims()[2], get_output_dims()[3]};
  }

  gen_conv_bwd(sc_op *owner, const sc_dims &stride, const sc_dims &padding,
    std::vector<logical_tensor_t> &&ins, std::vector<logical_tensor_t> &&outs);

  float get_gflop() const override;

  const sc_dims &get_input_dims() const {
    return in_tensors_[0].get_plain_dims();
  }
  const sc_dims &get_weight_dims() const {
    return in_tensors_[1].get_plain_dims();
  }
  const sc_dims &get_output_dims() const {
    return out_tensors_[0].get_plain_dims();
  }
  sc_data_type_t get_dtype() const { return in_tensors_[0].dtype_; }

  bool generate(context_ptr ctx, const conv_bwd_config &config,
    fusion_manager *fusion, const std::vector<expr> &inputs,
    const std::vector<expr> &outputs,
    std::vector<for_loop> &loops) const override;
  config_ptr get_default_config(context_ptr ctx) const override;

  void schedule_loops(context_ptr ctx, const conv_bwd_config &config, stmt body,
    std::vector<for_loop> &fors) const override;
};
} // namespace ops

} // namespace sc

#endif