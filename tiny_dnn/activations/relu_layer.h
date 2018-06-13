/*
    Copyright (c) 2013, Taiga Nomi and the respective contributors
    All rights reserved.

    Use of this source code is governed by a BSD-style license that can be found
    in the LICENSE file.
*/
#pragma once
#include "tiny_dnn/activations/activation_layer.h"
#include "tiny_dnn/layers/layer.h"

namespace tiny_dnn {

class relu_layer : public activation_layer {
 public:
  using activation_layer::activation_layer;

  std::string layer_type() const override { return "relu-activation"; }

  void forward_activation(const vec_t &x, vec_t &y) override {
    // Korol
    #ifdef PRINT_DEBUG
     printf("[relu_layer/forward_propagation] ReLU Layer Operation\n");
     printf("\tFOR j = 0 : %ld\n\
     \t|\ty[j] = std::max(float_t(0), x[j])\n\
     \t\n\n",
          x.size());
    #endif
    
    for (serial_size_t j = 0; j < x.size(); j++) {
      y[j] = std::max(float_t(0), x[j]);
    }

    // if (x.size() == 290400) {
    //   std::ofstream fout_outdata;
    //   fout_outdata.open("transfer_files/OUT_DATA_.dat", std::ios::out | std::ios::trunc);
    //   for( int i=0; i < x.size(); i++) {
    //    fout_outdata << std::fixed << y[i] << std::endl;
    //   }
    //   fout_outdata.close();
    // }
  }

  void backward_activation(const vec_t &x,
                           const vec_t &y,
                           vec_t &dx,
                           const vec_t &dy) override {
    for (serial_size_t j = 0; j < x.size(); j++) {
      // dx = dy * (gradient of relu)
      dx[j] = dy[j] * (y[j] > float_t(0) ? float_t(1) : float_t(0));
    }
  }

  std::pair<float_t, float_t> scale() const override {
    return std::make_pair(float_t(0.1), float_t(0.9));
  }

  friend struct serialization_buddy;
};

}  // namespace tiny_dnn
