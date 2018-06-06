/*
    Copyright (c) 2013, Taiga Nomi and the respective contributors
    All rights reserved.

    Use of this source code is governed by a BSD-style license that can be found
    in the LICENSE file.
*/
#pragma once

#include <fstream>
#include <limits>

namespace tiny_dnn {
namespace kernels {

inline void conv2d_op_internal(const tensor_t &in_data,
                               const vec_t &W,
                               const vec_t &bias,
                               tensor_t &out_data,
                               const core::conv_params &params,
                               const bool parallelize) {
  // Korol
  #ifdef PRINT_DEBUG
  printf("\n[conv2d_op_internal] Convolution Layer Operation (internal)\n");

  printf("Layer Input (padded): %d x %d x %d, size = %ld\nLayer Output = %d x %d x %d, size = %ld\nFilter %d x %d \nBias = %ld\nElement stride = %d\nLine stride = %d\n",
  params.in_padded.width_,
  params.in_padded.height_,
  params.in.depth_,
  in_data[0].size(),
  params.out.width_,
  params.out.height_,
  params.out.depth_,
  out_data[0].size(),
  params.weight.width_,
  params.weight.height_,
  bias.size(),
  params.w_stride,
  params.in_padded.width_ * params.h_stride);

  printf("FOR sample = r.begin() : r.end()\n\
         \tFOR o = 0 : %d \n\
         \t|\tFOR inc = 0 : %d \n\
         \t|\t|\tpin = in[in_padded.get_index(0, 0, inc)]\n\
         \t|\t|\tpw  = W[weight.get_index(0, 0, id * o + inc)]\n\
         \t|\t|\tFOR y = 0 : %d \n\
         \t|\t|\t|\tpin_line = pin\n\
         \t|\t|\t|\tFOR x = 0 : %d \n\
         \t|\t|\t|\t|\tsum = 0\n\
         \t|\t|\t|\t|\tpw_element = pin_line\n\
         \t|\t|\t|\t|\tpin_element = pw\n\
         \t|\t|\t|\t|\tFOR wx = 0 : %d \n\
         \t|\t|\t|\t|\t|\tFOR wy = 0 : %d \n\
         \t|\t|\t|\t|\t|\t|\tsum += pw_element[wx] * pin_element[wx]\n\
         \t|\t|\t|\t|\t|\tpw_element += kw \n\
         \t|\t|\t|\t|\t|\tpin_element += iw \n\
         \t|\t|\t|\t|\tpout[x] += sum \n\
         \t|\t|\t|\t|\tpin_line += elem_stride\n\
         \t|\t|\t|\tpout += ow \n\
         \t|\t|\t|\tpin += line_stride\n\
         \t|\tvectorize::add(bias[o], out_area, pa)\n\
         \n\n",
       params.out.depth_,
       params.in.depth_,
       params.out.height_,
       params.out.width_,
       params.weight.height_,
       params.weight.width_);
  #endif

  for_(parallelize, 0, in_data.size(),
       [&](const blocked_range &r) {
         size_t out_area           = params.out.area();
         serial_size_t iw          = params.in_padded.width_;
         serial_size_t id          = params.in.depth_;
         serial_size_t ow          = params.out.width_;
         serial_size_t oh          = params.out.height_;
         serial_size_t od          = params.out.depth_;
         serial_size_t kw          = params.weight.width_;
         serial_size_t kh          = params.weight.height_;
         serial_size_t elem_stride = params.w_stride;
         serial_size_t line_stride = iw * params.h_stride;
         for (size_t sample = r.begin(); sample < r.end(); sample++) {
           const vec_t &in = in_data[sample];
           vec_t &a        = out_data[sample];
           for (serial_size_t o = 0; o < od; o++) {
             float_t *pa = &a[params.out.get_index(0, 0, o)];//
             for (serial_size_t inc = 0; inc < id; inc++) {
               // if (!params.tbl.is_connected(o, inc)) continue;
               serial_size_t idx;
               idx                = params.weight.get_index(0, 0, id * o + inc);
               const float_t *pw  = &W[idx];
               idx                = params.in_padded.get_index(0, 0, inc);
               const float_t *pin = &in[idx];
               float_t *pout      = pa;
               for (serial_size_t y = 0; y < oh; y++) {
                 const float_t *pin_line = pin; //
                 for (serial_size_t x = 0; x < ow; x++) {
                   const float_t *pin_element = pin_line;
                   const float_t *pw_element  = pw;
                   float_t sum{0};
                   // should be optimized for small kernel(3x3,5x5)
                   for (serial_size_t wy = 0; wy < kh; wy++) {    // NOLINT
                     for (serial_size_t wx = 0; wx < kw; wx++) {  // NOLINT
                       sum += pw_element[wx] * pin_element[wx];
                     }
                     pw_element += kw;
                     pin_element += iw;
                   }
                   pout[x] += sum;
                   pin_line += elem_stride;
                 }
                 pout += ow;
                 pin += line_stride;
               }
             }
             if (params.has_bias) {
               vectorize::add(bias[o], out_area, pa);
             }
           }
         }
       },
       0);

   //  if (in_data[0].size() == 86400 && out_data[0].size() == 43264 && bias.size() == 256 && W.size() == 884736) {
   //
   //   std::ofstream fout_indata;
   //   fout_indata.open("transfer_files/IN_DATA.dat", std::ios::out | std::ios::trunc);
   //   for( int i=0; i < in_data[0].size(); i++) {
   //     fout_indata << std::fixed << in_data[0][i] << std::endl;
   //   }
   //   fout_indata.close();
   //
   //   std::ofstream fout_outdata;
   //   fout_outdata.open("transfer_files/OUT_DATA.dat", std::ios::out | std::ios::trunc);
   //   for( int i=0; i < out_data[0].size(); i++) {
   //    fout_outdata << std::fixed << out_data[0][i] << std::endl;
   //    }
   //   fout_outdata.close();
   //
   //   std::ofstream fout_filter;
   //   fout_filter.open("transfer_files/FILTER_DATA.dat", std::ios::out | std::ios::trunc);
   //   for( int i=0; i < W.size(); i++) {
   //     fout_filter << std::fixed << W[i] << std::endl;
   //   }
   //   fout_filter.close();
   //
   //   std::ofstream fout_bias;
   //   fout_bias.open("transfer_files/BIAS_DATA.dat", std::ios::out | std::ios::trunc);
   //   for( int i=0; i < bias.size(); i++) {
   //     fout_bias << std::fixed << bias[i] << std::endl;
   //   }
   //   fout_bias.close();
   //
   // }
   // // else {
   // //   printf("FAILED FILE WRITING:\nIN SIZE = %ld\tOUT SIZE = %ld\tBIAS SIZE = %ld\tW SIZE = %ld\n", in_data[0].size(), out_data[0].size(), bias.size(), W.size());
   // // }
}

/******************************************************************/

template <typename tensor_t, typename vec_t>
void conv2d_op_internal(const tensor_t &prev_out,
                        const vec_t &W,
                        tensor_t &dW,
                        tensor_t &db,
                        tensor_t &curr_delta,
                        tensor_t &prev_delta,
                        const core::conv_params &params,
                        const bool parallelize) {
  typedef typename vec_t::value_type float_t;

  for_i(parallelize, prev_out.size(), [&](int sample) {
    // propagate delta to previous layer
    for (serial_size_t inc = 0; inc < params.in.depth_; inc++) {
      for (serial_size_t outc = 0; outc < params.out.depth_; outc++) {
        if (!params.tbl.is_connected(outc, inc)) continue;

        serial_size_t idx = 0;
        idx               = params.in.depth_ * outc + inc;
        idx               = params.weight.get_index(0, 0, idx);
        const float_t *pw = &W[idx];

        idx                       = params.out.get_index(0, 0, outc);
        const float_t *pdelta_src = &curr_delta[sample][idx];

        idx = params.in_padded.get_index(0, 0, inc);
        // float_t* pdelta_dst = &(*prev_delta)[sample][idx];
        float_t *pdelta_dst = &prev_delta[sample][idx];

        for (serial_size_t y = 0; y < params.out.height_; y++) {
          for (serial_size_t x = 0; x < params.out.width_; x++) {
            const float_t *ppw = pw;

            idx                       = y * params.out.width_ + x;
            const float_t ppdelta_src = pdelta_src[idx];

            float_t *ppdelta_dst =
              pdelta_dst + y * params.h_stride * params.in_padded.width_ +
              x * params.w_stride;

            for (serial_size_t wy = 0; wy < params.weight.height_;
                 wy++) {  // NOLINT
              for (serial_size_t wx = 0; wx < params.weight.width_;
                   wx++) {  // NOLINT
                idx = wy * params.in_padded.width_ + wx;
                ppdelta_dst[idx] += *ppw++ * ppdelta_src;
              }
            }
          }
        }
      }
    }

    // accumulate dw
    for (serial_size_t inc = 0; inc < params.in.depth_; inc++) {
      for (serial_size_t outc = 0; outc < params.out.depth_; outc++) {
        if (!params.tbl.is_connected(outc, inc)) continue;

        for (serial_size_t wy = 0; wy < params.weight.height_; wy++) {
          for (serial_size_t wx = 0; wx < params.weight.width_; wx++) {
            float_t dst{0};

            serial_size_t idx    = 0;
            idx                  = params.in_padded.get_index(wx, wy, inc);
            const float_t *prevo = &prev_out[sample][idx];

            idx                  = params.out.get_index(0, 0, outc);
            const float_t *delta = &curr_delta[sample][idx];

            if (params.w_stride > 1) {
              for (serial_size_t y = 0; y < params.out.height_; y++) {
                serial_size_t prevo_idx =
                  y * params.in_padded.width_ * params.h_stride;
                serial_size_t delta_idx = y * params.out.width_;

                for (serial_size_t x = 0; x < params.out.width_; x++) {
                  dst += prevo[prevo_idx + x * params.w_stride] *
                         delta[delta_idx + x];
                }
              }
            } else {
              for (serial_size_t y = 0; y < params.out.height_; y++) {
                dst += vectorize::dot(
                  prevo + y * params.in_padded.width_ * params.h_stride,
                  delta + y * params.out.width_, params.out.width_);
              }
            }

            idx = params.in.depth_ * outc + inc;
            dW[sample][params.weight.get_index(wx, wy, idx)] += dst;
          }
        }
      }
    }

    // accumulate db
    if (params.has_bias) {
      for (serial_size_t outc = 0; outc < params.out.depth_; outc++) {
        serial_size_t idx     = params.out.get_index(0, 0, outc);
        const float_t *delta  = &curr_delta[sample][idx];
        const float_t *deltaa = delta + params.out.width_ * params.out.height_;
        db[sample][outc] += std::accumulate(delta, deltaa, float_t{0});
      }
    }
  });
}

}  // namespace kernels
}  // namespace tiny_dnn
