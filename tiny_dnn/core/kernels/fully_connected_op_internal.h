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


  if (in_data[0].size() == 9216 && out_data[0].size() == 4096 && bias.size() == 4096 && W.size() == 37748736) {

    std::ofstream fout_indata;
    fout_indata.open("transfer_files/IN_DATA.dat", std::ios::out | std::ios::trunc);
    for( int i=0; i < in_data[0].size(); i++) {
      fout_indata << std::fixed << in_data[0][i] << std::endl;
    }
    fout_indata.close();

    // std::ofstream fout_outdata;
    // fout_outdata.open("transfer_files/OUT_DATA.dat", std::ios::out | std::ios::trunc);
    // for( int i=0; i < out_data[0].size(); i++) {
    //  fout_outdata << std::fixed << out_data[0][i] << std::endl;
    // }
    // fout_outdata.close();
    //
    // // Escreve pesos em arquivo binario
    // std::ofstream fout_filter;
    // fout_filter.open("transfer_files/FILTER_DATA.dat", std::ios::out | std::ios::trunc);
    // for( int i=0; i < W.size(); i++) {
    //   fout_filter << std::fixed << W[i] << std::endl;
    // }
    // fout_filter.close();
    //
    // // Escreve biases em arquivo binario
    // std::ofstream fout_bias;
    // fout_bias.open("transfer_files/BIAS_DATA.dat", std::ios::out | std::ios::trunc);
    // for( int i=0; i < bias.size(); i++) {
    //   fout_bias << std::fixed << bias[i] << std::endl;
    // }
    // fout_bias.close();

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
