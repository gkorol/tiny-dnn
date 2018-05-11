/*
    Copyright (c) 2013, Taiga Nomi and the respective contributors
    All rights reserved.

    Use of this source code is governed by a BSD-style license that can be found
    in the LICENSE file.
*/
#pragma once

namespace tiny_dnn {
namespace kernels {

inline void maxpool_op_internal(
  const tensor_t &in_data,
  tensor_t &out_data,
  std::vector<std::vector<serial_size_t>> &max_idx,
  const std::vector<std::vector<serial_size_t>> &out2in,
  const bool layer_parallelize) {
  // Korol
  #ifdef PRINT_DEBUG
  printf("[maxpool_op_internal] Max Pool Layer Operation (internal)\n");

  printf("\tFOR i = 0 : %ld (out2in.size)\n\
         \t|\tin_index = out2in[i] \n\
         \t|\tmax_value = numeric_limits::lowest \n\
         \t|\tidx = 0 \n\
         \t|\tFOR j = 0 : %ld (all in_index) \n\
         \t|\t|\tif in[j] > max_value {\n\
         \t|\t|\t|\tmax_value = in[j]\n\
         \t|\t|\t|\tidx = j }\n\
         \t|\tmax[i] = idx\n\
         \t|\tout[i] = max_value\n\
         \t\n\n",
       out2in.size(),
       out2in[0].size() );
  #endif
  for_i(layer_parallelize, in_data.size(), [&](int sample) {
    const vec_t &in                 = in_data[sample];
    vec_t &out                      = out_data[sample];
    std::vector<serial_size_t> &max = max_idx[sample];

    for (serial_size_t i = 0; i < out2in.size(); i++) {
      const auto &in_index = out2in[i];
      float_t max_value    = std::numeric_limits<float_t>::lowest();
      serial_size_t idx    = 0;
      for (auto j : in_index) {
        if (in[j] > max_value) {
          max_value = in[j];
          idx       = j;
        }
      }
      max[i] = idx;
      out[i] = max_value;
    }
  });
}

inline void maxpool_grad_op_internal(
  tensor_t &prev_delta,
  const tensor_t &curr_delta,
  std::vector<std::vector<serial_size_t>> &max_idx,
  const std::vector<serial_size_t> &in2out,
  const bool layer_parallelize) {
  for_i(layer_parallelize, prev_delta.size(), [&](int sample) {
    vec_t &prev                           = prev_delta[sample];
    const vec_t &curr                     = curr_delta[sample];
    const std::vector<serial_size_t> &max = max_idx[sample];

    for (serial_size_t i = 0; i < in2out.size(); i++) {
      serial_size_t outi = in2out[i];
      prev[i] =
        (max[outi] == static_cast<serial_size_t>(i)) ? curr[outi] : float_t{0};
    }
  });
}

}  // namespace kernels
}  // namespace tiny_dnn
