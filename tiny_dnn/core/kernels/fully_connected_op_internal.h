/*
    Copyright (c) 2013, Taiga Nomi and the respective contributors
    All rights reserved.

    Use of this source code is governed by a BSD-style license that can be found
    in the LICENSE file.
*/
#pragma once

#include "tiny_dnn/core/params/fully_params.h"

namespace tiny_dnn {
namespace kernels {

inline void fully_connected_op_internal(const tensor_t &in_data,
                                        const vec_t &W,
                                        const vec_t &bias,
                                        tensor_t &out_data,
                                        const fully_params &params,
                                        const bool layer_parallelize) {
  // Korol
  #ifdef PRINT_DEBUG
  printf("[fully_connected_op_internal] Fully Connected Layer Operation (internal)\n");
  printf("Layer Input Size = %d\nLayer Output Size = %d\nWeights Size = %ld\nBias size = %ld\n",
  params.in_size_,
  params.out_size_,
  W.size(),
  bias.size());

  printf("\tFOR i = 0 : %d\n\
         \t|\tout[i] = 0\n\
         \t|\tFOR c = 0 : %d\n\
         \t|\t|\tout[i] += W[c * params.out_size_ + i] * in[c]\n\
         \t|\tout[i] += bias[i];\n\
         \t\n\n",
       params.out_size_,
       params.in_size_);
  #endif
  for_i(layer_parallelize, in_data.size(), [&](int sample) {
    const vec_t &in = in_data[sample];
    vec_t &out      = out_data[sample];

    for (serial_size_t i = 0; i < params.out_size_; i++) {
      out[i] = float_t{0};
      for (serial_size_t c = 0; c < params.in_size_; c++) {
        out[i] += W[c * params.out_size_ + i] * in[c];
        // if(i==0) {
        //   printf("%f = %f * %f\n", out[i],W[c * params.out_size_ + i],in[c]);
        // }
      }

      if (params.has_bias_) {
        out[i] += bias[i];
      }
    }
  });

  if (params.out_size_ == 4096 && params.in_size_ == 9216 && W.size() == 37748736 && bias.size() == 4096) {
    // Verdadeiro so no primeiro layer FC

    // Escreve entrada em arquivo binario
    std::ofstream fout_indata;
    fout_indata.open("IN_DATA_FC.dat", std::ios::out | std::ofstream::binary | std::ios::trunc);
    const vec_t &in = in_data[0];
    fout_indata.write(reinterpret_cast<const char *>(&in[0]), params.in_size_*sizeof(float_t));
    fout_indata.close();

    // Escreve saida em arquivo binario
    std::ofstream fout_outdata;
    fout_outdata.open("OUT_DATA_FC.dat", std::ios::out | std::ofstream::binary | std::ios::trunc);
    const vec_t &a = out_data[0];
    fout_outdata.write(reinterpret_cast<const char *>(&a[0]), params.out_size_*sizeof(float_t));
    fout_outdata.close();

    // Escreve pesos em arquivo binario
    std::ofstream fout_filter;
    fout_filter.open("FILTER_FC.dat", std::ios::out | std::ofstream::binary | std::ios::trunc);
    fout_filter.write(reinterpret_cast<const char *>(&(W[0])), W.size()*sizeof(float_t));
    fout_filter.close();

    // Escreve biases em arquivo binario
    std::ofstream fout_bias;
    fout_bias.open("BIAS_FC.dat", std::ios::out | std::ofstream::binary | std::ios::trunc);
    fout_bias.write(reinterpret_cast<const char *>(&(bias[0])), bias.size()*sizeof(float_t));
    fout_bias.close();
  }
}

inline void fully_connected_op_internal(const tensor_t &prev_out,
                                        const vec_t &W,
                                        tensor_t &dW,
                                        tensor_t &db,
                                        tensor_t &curr_delta,
                                        tensor_t &prev_delta,
                                        const fully_params &params,
                                        const bool layer_parallelize) {
  for (serial_size_t sample = 0; sample < prev_out.size(); sample++) {
    for (serial_size_t c = 0; c < params.in_size_; c++) {
      // propagate delta to previous layer
      // prev_delta[c] += current_delta[r] * W_[c * out_size_ + r]
      prev_delta[sample][c] += vectorize::dot(
        &curr_delta[sample][0], &W[c * params.out_size_], params.out_size_);
    }

    for_(layer_parallelize, 0, size_t(params.out_size_),
         [&](const blocked_range &r) {
           // accumulate weight-step using delta
           // dW[c * out_size + i] += current_delta[i] * prev_out[c]
           for (serial_size_t c = 0; c < params.in_size_; c++) {
             vectorize::muladd(&curr_delta[sample][r.begin()],
                               prev_out[sample][c], r.end() - r.begin(),
                               &dW[sample][c * params.out_size_ + r.begin()]);
           }

           if (params.has_bias_) {
             // vec_t& db = *in_grad[2];
             for (size_t i = r.begin(); i < r.end(); i++) {
               db[sample][i] += curr_delta[sample][i];
             }
           }
         });
  }
}

}  // namespace kernels
}  // namespace tiny_dnn
