#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "param_headers/1_weight.h"
#include "param_headers/1_in.h"
#include "param_headers/1_bias.h"

#include "param_headers/2_in.h"

// #include "param_headers/2_weight.h"
// #include "param_headers/2_bias.h"

// #include "param_headers/3_weight.h"
// #include "param_headers/3_bias.h"
//
// #include "param_headers/4_weight.h"
// #include "param_headers/4_bias.h"
//
// #include "param_headers/5_weight.h"
// #include "param_headers/5_bias.h"

/******* Layer Conv 1 ********/
#define FILTER_HEIGHT_1 11
#define FILTER_WIDTH_1  11
#define K_FILTERS_1     96
#define IN_HEIGHT_1     227
#define IN_WIDTH_1      227
#define IN_DEPTH_1      3
#define OUT_HEIGHT_1    55
#define OUT_WIDTH_1     55
#define OUT_DEPTH_1     96
#define STRIDE_1        4
/*****************************/

/******* Layer Conv 2 ********/
#define FILTER_HEIGHT_2 5
#define FILTER_WIDTH_2  5
#define K_FILTERS_2     256
#define IN_HEIGHT_2     31 // 27
#define IN_WIDTH_2      31 // 27
#define IN_DEPTH_2      96
#define OUT_HEIGHT_2    27
#define OUT_WIDTH_2     27
#define OUT_DEPTH_2     256
#define STRIDE_2        1
/*****************************/

/******* Layer Conv 3 ********/
#define FILTER_HEIGHT_3 3
#define FILTER_WIDTH_3  3
#define K_FILTERS_3     256
#define IN_HEIGHT_3     15
#define IN_WIDTH_3      15
#define IN_DEPTH_3      256
#define OUT_HEIGHT_3    13
#define OUT_WIDTH_3     13
#define OUT_DEPTH_3     384
#define STRIDE_3        1
/*****************************/

/******* Layer Conv 4 ********/
#define FILTER_HEIGHT_4 3
#define FILTER_WIDTH_4  3
#define K_FILTERS_4     384
#define IN_HEIGHT_4     13
#define IN_WIDTH_4      13
#define IN_DEPTH_4      384
#define OUT_HEIGHT_4    15
#define OUT_WIDTH_4     15
#define OUT_DEPTH_4     384
#define STRIDE_4        1
/*****************************/

/******* Layer Conv 5 ********/
#define FILTER_HEIGHT_5 3
#define FILTER_WIDTH_5  3
#define K_FILTERS_5     256
#define IN_HEIGHT_5     15
#define IN_WIDTH_5      15
#define IN_DEPTH_5      384
#define OUT_HEIGHT_5    13
#define OUT_WIDTH_5     13
#define OUT_DEPTH_5     256
#define STRIDE_5        1
/*****************************/

void conv(float in[], float weights[], float bias[], float out[],
  const unsigned kh, const unsigned kw, const unsigned ih, const unsigned iw,
  const unsigned id, const unsigned oh, const unsigned ow, const unsigned od,
  const unsigned s){

  unsigned o_s;
  unsigned i_s;
  unsigned o_x;
  unsigned o_y;
  unsigned f_x;
  unsigned f_y;

  float sum;
  float * in_el;
  float * f_el;
  float * in_slice;
  float * filter_slice;
  float * in_line;
  float * o_line;
  float * out_slice;

  unsigned ls = s * ih;

  for ( o_s = 0; o_s < od; o_s++) {
    out_slice = &out[ o_s * oh * ow ];
    for ( i_s = 0; i_s < id ; i_s++) {
      filter_slice  = &weights[ (o_s * kh * kw * id) + (i_s * kh * kw) ];
      in_slice = &in[ i_s * ih * iw ];
      o_line = out_slice;
      for ( o_y = 0; o_y < oh; o_y++) {
        in_line = in_slice;
        for ( o_x = 0; o_x < ow; o_x++) {
          in_el = in_line;
          f_el = filter_slice;
          sum = 0.0;
          for ( f_y = 0; f_y < kh; f_y++) {
            for ( f_x = 0; f_x < kw; f_x++) {
              sum += f_el[f_x] * in_el[f_x];
            }
            f_el += kw;
            in_el += iw;
          }
          o_line[o_x] += sum;
          in_line += s;
        }
        o_line += ow;
        in_slice += ls;
      }
    }

    for(o_x = 0; o_x < oh*ow; o_x++) {
      out_slice[o_x] += bias[o_s];
    }
  }
}

void relu(float in[], const unsigned ih, const unsigned iw, const unsigned id) {
  unsigned size = ih * iw * id;
  unsigned i;

  for( i=0; i < size; ++i) {
    if(in[i] < 0.0){
      in[i] = 0.0;
    }
  }
}

void add_square_sum(float in[], float out[], const unsigned size) {
  unsigned i;
  for (i = 0; i < size; i++) out[i] += in[i] * in[i];
}

void sub_square_sum(float in[], float out[], const unsigned size) {
  unsigned i;
  for (i = 0; i < size; i++) out[i] -= in[i] * in[i];
}

void lrn(float in[], float out[], const unsigned ih, const unsigned iw, const unsigned id) {
  unsigned size_ = 5;             // Alexnet defined
  float alpha_ = 0.000100;        // Alexnet defined
  float beta_  = 0.750000;        // Alexnet defined

  unsigned i,j;
  unsigned wxh = ih * iw;
  unsigned head = size_ / 2;
  long tail = (long)head - (long)size_;
  float alpha_div_size = alpha_ / size_;

  float * dst;
  float * src;
  float * in_square_ = malloc( wxh * sizeof(float));

  for (i = 0; i < size_ / 2; i++) {
    add_square_sum(&(in[i*wxh]), in_square_, wxh);
  }

  for ( i = 0; i < id; i++, head++, tail++) {
    if (head < id)
      add_square_sum(&in[head * wxh], in_square_, wxh);

    if (tail >= 0)
      sub_square_sum(&in[tail * wxh], in_square_, wxh);

    dst = &out[i * wxh];
    src = &in[i * wxh];
    for (j = 0; j < wxh; j++)
      dst[j] = src[j] * pow(1.0 + alpha_div_size * in_square_[j], -beta_);
  }

}

void maxpool(float in[], float out[], const unsigned ih, const unsigned iw,
  const unsigned id, const unsigned oh, const unsigned ow, const unsigned od,
  const unsigned stride, const unsigned ps) {
  // Assuming input values above zero (in our case maxpool always come after a relu)

  unsigned x_o, y_o, k, wx, wy;
  unsigned in_idx, out_idx;
  float max;

  for( k = 0; k < od; ++k) {
    for( y_o = 0; y_o < oh; y_o++ ) {
      for( x_o = 0; x_o < ow; x_o++ ) {
        max = 0.0;
        for( wy = 0; wy < ps; wy++ ) {
          for( wx = 0; wx < ps; wx++ ) {
            in_idx = (ih*k + (y_o*stride+wy)) * iw + (x_o*stride+wx);
            if(in[in_idx] > max) {
              max = in[in_idx];
            }
          }
        }
        out_idx = (oh*k + y_o) * ow + x_o;
        out[out_idx] = max;
      }
    }
  }

}

int main(int argc, char** argv) {

  size_t alloc_size;

  unsigned i;
  unsigned o_s;
  unsigned o_x;
  float error;
  float max;

  alloc_size = OUT_WIDTH_1*OUT_HEIGHT_1*OUT_DEPTH_1;
  float * out_1 = malloc(alloc_size * sizeof(float));
  if (!out_1) { perror("malloc failed"); exit(EXIT_FAILURE); };

  conv(in_1, weight_1, bias_1, out_1,
    FILTER_HEIGHT_1, FILTER_WIDTH_1,
    IN_HEIGHT_1, IN_WIDTH_1, IN_DEPTH_1,
    OUT_HEIGHT_1, OUT_WIDTH_1, OUT_DEPTH_1,
    STRIDE_1);

  relu(out_1, OUT_HEIGHT_1, OUT_WIDTH_1, OUT_DEPTH_1);

  alloc_size = OUT_WIDTH_1*OUT_HEIGHT_1*OUT_DEPTH_1;
  float * out_lrm_1 = malloc(alloc_size * sizeof(float));
  if (!out_lrm_1) { perror("malloc failed"); exit(EXIT_FAILURE); };

  lrn(out_1, out_lrm_1, OUT_HEIGHT_1, OUT_WIDTH_1, OUT_DEPTH_1);

  free(out_1);

  // alloc_size = IN_HEIGHT_2*IN_WIDTH_2*IN_DEPTH_2;
  alloc_size = 27*27*IN_DEPTH_2;
  float * in_2_ = malloc(alloc_size * sizeof(float));
  if (!in_2_) { perror("malloc failed"); exit(EXIT_FAILURE); };

  maxpool(out_lrm_1, in_2_, OUT_HEIGHT_1, OUT_WIDTH_1, OUT_DEPTH_1, 27, 27, IN_DEPTH_2, 2, 3);

  // conv(out_1, weight_2, bias_2, out_2,
  //   FILTER_HEIGHT_2, FILTER_WIDTH_2,
  //   IN_HEIGHT_2, IN_WIDTH_2, IN_DEPTH_2,
  //   OUT_HEIGHT_2, OUT_WIDTH_2, OUT_DEPTH_2,
  //   STRIDE_2);



  FILE * out_f = fopen("transfer_files/OUT_DATA.dat","r");
  // FILE * out_f = fopen("transfer_files/IN_DATA.dat","r");
  alloc_size = 27*27*IN_DEPTH_2;
  float * out_test = malloc(alloc_size * sizeof(float));
  if (!out_test) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
   fscanf(out_f, "%f", &(out_test[i]));
  }
  fclose(out_f);

  error = 0.0;
  max = 0.0;
  i = 0;

  for(o_x = 0; o_x < 27*27*IN_DEPTH_2; ++o_x) {

    if ( in_2_[o_x]-out_test[o_x] < 0) {
      error = -1.0 * (in_2_[o_x]-out_test[o_x]);
    } else {
      error = in_2_[o_x]-out_test[o_x];
    }

    printf("in_2_[%4d] = %+2.6f | test[%4d] = %+2.6f ->\t %+2.6f\n", o_x, in_2_[o_x], o_x, out_test[o_x], error);
    if (error > max) {
      max = error;
    }
  }

  printf("Max Error = %f\n", max);

  // error = 0.0;
  // max = 0.0;
  // i = 0;
  //
  // for( o_s = 0; o_s < OUT_DEPTH_1; ++o_s ) {
  //   printf("OUTPUT SLICE %d\n", o_s);
  //   for(o_x = 0; o_x < OUT_HEIGHT_1*OUT_WIDTH_1; ++o_x) {
  //
  //     i = o_s*OUT_HEIGHT_1*OUT_WIDTH_1 + o_x;
  //
  //     if ( out_1[i]-out_test[i] < 0) {
  //       error = -1.0 * (out_1[i]-out_test[i]);
  //     } else {
  //       error = out_1[i]-out_test[i];
  //     }
  //     printf("OUT[%4d] = %+2.6f | out_test[%4d] = %+2.6f -> %+2.6f\n", i, out_1[i], i, out_test[i], error);
  //     if (error > max) {
  //       max = error;
  //     }
  //   }
  // }
  //
  // printf("Max Error = %f\n", max);

  return  0;
}
