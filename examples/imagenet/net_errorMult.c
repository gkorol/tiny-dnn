#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// #include "param_headers/1_in.h" // lendo de arquivo
#include "param_headers/1_weight.h"
#include "param_headers/1_bias.h"

#include "param_headers/2_weight.h"
#include "param_headers/2_bias.h"

#include "param_headers/3_weight.h"
#include "param_headers/3_bias.h"

#include "param_headers/4_weight.h"
#include "param_headers/4_bias.h"

#include "param_headers/5_weight.h"
#include "param_headers/5_bias.h"

#include "fc_include.h"

/******* Layer Conv 1 ********/
#define FILTER_HEIGHT_1    11
#define FILTER_WIDTH_1     11
#define IN_HEIGHT_1        227
#define IN_WIDTH_1         227
#define IN_DEPTH_1         3
#define OUT_CONV_HEIGHT_1  55
#define OUT_CONV_WIDTH_1   55
#define OUT_HEIGHT_1       27
#define OUT_WIDTH_1        27
#define OUT_DEPTH_1        96
// #define OUT_DEPTH_1        2
#define STRIDE_CONV_1      4
#define STRIDE_MAX_1       2
#define POOL_SIZE_1        3
/*****************************/

/******* Layer Conv 2 ********/
#define PAD_IN_2          2
#define FILTER_HEIGHT_2   5
#define FILTER_WIDTH_2    5
#define IN_HEIGHT_2       31
#define IN_WIDTH_2        31
#define IN_DEPTH_2        96
// #define IN_DEPTH_2        2
#define OUT_CONV_HEIGHT_2 27
#define OUT_CONV_WIDTH_2  27
#define OUT_HEIGHT_2      13
#define OUT_WIDTH_2       13
#define OUT_DEPTH_2       256
// #define OUT_DEPTH_2       3
#define STRIDE_CONV_2     1
#define STRIDE_MAX_2      2
#define POOL_SIZE_2       3
/*****************************/

/******* Layer Conv 3 ********/
#define PAD_IN_3        1
#define FILTER_HEIGHT_3 3
#define FILTER_WIDTH_3  3
#define IN_HEIGHT_3     15
#define IN_WIDTH_3      15
#define IN_DEPTH_3      256
// #define IN_DEPTH_3      3
#define OUT_HEIGHT_3    13
#define OUT_WIDTH_3     13
#define OUT_DEPTH_3     384
// #define OUT_DEPTH_3     3
#define STRIDE_CONV_3   1
/*****************************/

/******* Layer Conv 4 ********/
#define PAD_IN_4        1
#define FILTER_HEIGHT_4 3
#define FILTER_WIDTH_4  3
#define IN_HEIGHT_4     15
#define IN_WIDTH_4      15
#define IN_DEPTH_4      384
// #define IN_DEPTH_4      3
#define OUT_HEIGHT_4    13
#define OUT_WIDTH_4     13
#define OUT_DEPTH_4     384
// #define OUT_DEPTH_4     3
#define STRIDE_CONV_4   1
/*****************************/

/******* Layer Conv 5 ********/
#define PAD_IN_5          1
#define FILTER_HEIGHT_5   3
#define FILTER_WIDTH_5    3
#define IN_HEIGHT_5       15
#define IN_WIDTH_5        15
#define IN_DEPTH_5        384
// #define IN_DEPTH_5        3
#define OUT_CONV_HEIGHT_5 13
#define OUT_CONV_WIDTH_5  13
#define OUT_HEIGHT_5      6
#define OUT_WIDTH_5       6
#define OUT_DEPTH_5       256
// #define OUT_DEPTH_5       3
#define STRIDE_MAX_5      2
#define STRIDE_CONV_5     1
#define POOL_SIZE_5       3
/*****************************/

/******* Layer FC 6 ********/
#define IN_HEIGHT_6  6
#define IN_WIDTH_6   6
#define IN_DEPTH_6   256
#define OUT_HEIGHT_6 4096
#define OUT_WIDTH_6  1
#define OUT_DEPTH_6  1
/*****************************/

/******* Layer FC 7 ********/
#define IN_HEIGHT_7  4096
#define IN_WIDTH_7   1
#define IN_DEPTH_7   1
#define OUT_HEIGHT_7 4096
#define OUT_WIDTH_7  1
#define OUT_DEPTH_7  1
/*****************************/

/******* Layer FC 8 ********/
#define IN_HEIGHT_8  4096
#define IN_WIDTH_8   1
#define IN_DEPTH_8   1
#define OUT_HEIGHT_8 1000
#define OUT_WIDTH_8  1
#define OUT_DEPTH_8  1
/*****************************/

#define DEBUG_LAYER 5

void conv(const float in[], const float weights[], const float bias[], float out[],
  const unsigned kh, const unsigned kw, const unsigned ih, const unsigned iw,
  const unsigned id, const unsigned oh, const unsigned ow, const unsigned od,
  const unsigned s, float alpha, char layer){

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

  unsigned print_i = 0;

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
              // sum += f_el[f_x] * in_el[f_x];
              sum += (f_el[f_x] * in_el[f_x])*(1-alpha);
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

void softmax(float in[], float out [],
  const unsigned ih, const unsigned iw, const unsigned id) {

  unsigned size = ih * iw * id;
  unsigned i;
  float denominator = 0.0;

  float alpha = in[0];
  for( i=1; i < size; ++i) {
    if(in[i] > alpha)
      alpha = in[i];
  }

  for( i=0; i < size; ++i) {
    out[i] = exp(in[i] - alpha);
    denominator += out[i];
  }

  for( i=0; i < size; ++i) {
    out[i] /= denominator;
  }
}

void pad(float in[], float out[], const unsigned ih, const unsigned iw,
          const unsigned id, const unsigned pad) {
  unsigned oh = ih + 2*pad;
  unsigned ow = iw + 2*pad;
  unsigned d, x, y, o_idx, i_idx;

  for( d = 0; d < id; ++d ){
    for( y = 0; y < oh; ++y ){
      for( x = 0; x < ow; ++x ){

        o_idx = (oh*d + y) * ow + x;
        i_idx = 0;
        if( y < pad || y > (ih+pad-1) || x < pad || x > (iw+pad-1) ){
          out[o_idx] = 0.0;
        } else {
          i_idx = (ih*d + y - pad) * iw + x - pad;
          out[o_idx] = in[i_idx];
        }

      }
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

void lrn(float in[], float out[],
  const unsigned ih, const unsigned iw, const unsigned id) {

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

  free(in_square_);
}

void maxpool(float in[], float out[], const unsigned ih, const unsigned iw,
  const unsigned id, const unsigned oh, const unsigned ow, const unsigned od,
  const unsigned stride, const unsigned ps, char layer) {
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

// void fc(float in[], float weights[], float bias[], float out[],
//   const unsigned ih, const unsigned iw, const unsigned id,
//   const unsigned oh, const unsigned ow, const unsigned od ){
//
//   unsigned o_y, i_s;
//   unsigned in_size = ih*iw*id;
//
//   for ( o_y = 0; o_y < oh; o_y++) {
//     out[o_y] = 0.0;
//     for ( i_s = 0; i_s < in_size; i_s++) {
//       out[o_y] += in[i_s] * weights[i_s * oh + o_y];
//     }
//     out[o_y] += bias[o_y];
//   }
//
// }

void top5 (float gold[], int gold_top5[],
           float test[], int test_top5[], int size) {

  unsigned i;
  unsigned j;
  unsigned current_max_gold_i = 0;
  float current_max_gold = 0.0;

  unsigned current_max_test_i = 0;
  float current_max_test = 0.0;

  for(i=0; i<5; i++) {
    current_max_gold = 0.0;
    current_max_test = 0.0;

    for(j=0; j<size; j++) {
      if(gold[j] > current_max_gold) {
        current_max_gold_i = j;
        current_max_gold = gold[j];
      }
      if(test[j] > current_max_test) {
        current_max_test_i = j;
        current_max_test = test[j];
      }
    }

    gold_top5[i] = current_max_gold_i;
    gold[current_max_gold_i] = -1.0;

    test_top5[i] = current_max_test_i;
    test[current_max_test_i] = -1.0;
  }
}

int main(int argc, char** argv) {

  size_t alloc_size;

  float alpha;
  unsigned i;
  unsigned o_x;
  float error;
  float max;

  // Passa em argv erro + arq de entrada

  alpha = atof(argv[1]);

  FILE * file_in = fopen(argv[2],"r");

  alloc_size = IN_HEIGHT_1*IN_WIDTH_1*IN_DEPTH_1;
  float * in_1 = malloc(alloc_size * sizeof(float));
  if (!in_1) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
   fscanf(file_in, "%f", &(in_1[i]));
  }
  fclose(file_in);

  /******************************** Layer 1 ***********************************/
  alloc_size = OUT_CONV_WIDTH_1*OUT_CONV_HEIGHT_1*OUT_DEPTH_1;
  float * out_1 = malloc(alloc_size * sizeof(float));
  if (!out_1) { perror("malloc failed"); exit(EXIT_FAILURE); };
  // Inicializa out_1
  for( i = 0; i < OUT_CONV_WIDTH_1*OUT_CONV_HEIGHT_1*OUT_DEPTH_1; ++i) {
    out_1[i] = 0.0;
  }

  conv(in_1, weight_1, bias_1, out_1,
    FILTER_HEIGHT_1, FILTER_WIDTH_1,
    IN_HEIGHT_1, IN_WIDTH_1, IN_DEPTH_1,
    OUT_CONV_HEIGHT_1, OUT_CONV_WIDTH_1, OUT_DEPTH_1,
    STRIDE_CONV_1, alpha, 1);

  free(in_1);

  relu(out_1, OUT_CONV_HEIGHT_1, OUT_CONV_WIDTH_1, OUT_DEPTH_1);

  alloc_size = OUT_CONV_WIDTH_1*OUT_CONV_HEIGHT_1*OUT_DEPTH_1;
  float * out_lrn_1 = malloc(alloc_size * sizeof(float));
  if (!out_lrn_1) { perror("malloc failed"); exit(EXIT_FAILURE); };

  lrn(out_1, out_lrn_1, OUT_CONV_HEIGHT_1, OUT_CONV_WIDTH_1, OUT_DEPTH_1);

  free(out_1);

  alloc_size = OUT_HEIGHT_1*OUT_WIDTH_1*OUT_DEPTH_1;
  float * out_pool_1 = malloc(alloc_size * sizeof(float));
  if (!out_pool_1) { perror("malloc failed"); exit(EXIT_FAILURE); };

  maxpool(out_lrn_1, out_pool_1, OUT_CONV_HEIGHT_1, OUT_CONV_WIDTH_1, OUT_DEPTH_1,
  // maxpool(out_1, out_pool_1, OUT_CONV_HEIGHT_1, OUT_CONV_WIDTH_1, OUT_DEPTH_1,
          OUT_HEIGHT_1, OUT_WIDTH_1, OUT_DEPTH_1,
          STRIDE_MAX_1, POOL_SIZE_1, 1);
  // free(out_1);
  free(out_lrn_1);

  /******************************** Layer 2 ***********************************/
  alloc_size = IN_HEIGHT_2*IN_WIDTH_2*IN_DEPTH_2;
  float * in_2 = malloc(alloc_size * sizeof(float));
  if (!in_2) { perror("malloc failed"); exit(EXIT_FAILURE); };

  pad(out_pool_1, in_2, OUT_HEIGHT_1, OUT_WIDTH_1, OUT_DEPTH_1, PAD_IN_2);
  //
  free(out_pool_1);

  alloc_size = OUT_CONV_WIDTH_2*OUT_CONV_HEIGHT_2*OUT_DEPTH_2;
  float * out_2 = malloc(alloc_size * sizeof(float));
  if (!out_2) { perror("malloc failed"); exit(EXIT_FAILURE); };
  // Inicializa out_2
  for( i = 0; i < OUT_CONV_WIDTH_2*OUT_CONV_HEIGHT_2*OUT_DEPTH_2; ++i) {
    out_2[i] = 0.0;
  }

  conv(in_2, weight_2, bias_2, out_2,
    FILTER_HEIGHT_2, FILTER_WIDTH_2,
    IN_HEIGHT_2, IN_WIDTH_2, IN_DEPTH_2,
    OUT_CONV_HEIGHT_2, OUT_CONV_WIDTH_2, OUT_DEPTH_2,
    STRIDE_CONV_2, alpha, 2);

  free(in_2);

  relu(out_2, OUT_CONV_HEIGHT_2, OUT_CONV_WIDTH_2, OUT_DEPTH_2);

  alloc_size = OUT_CONV_WIDTH_2*OUT_CONV_HEIGHT_2*OUT_DEPTH_2;
  float * out_lrn_2 = malloc(alloc_size * sizeof(float));
  if (!out_lrn_2) { perror("malloc failed"); exit(EXIT_FAILURE); };

  lrn(out_2, out_lrn_2, OUT_CONV_HEIGHT_2, OUT_CONV_WIDTH_2, OUT_DEPTH_2);

  free(out_2);

  alloc_size = OUT_HEIGHT_2*OUT_WIDTH_2*OUT_DEPTH_2;
  float * out_pool_2 = malloc(alloc_size * sizeof(float));
  if (!out_pool_2) { perror("malloc failed"); exit(EXIT_FAILURE); };

  maxpool(out_lrn_2, out_pool_2, OUT_CONV_HEIGHT_2, OUT_CONV_WIDTH_2, OUT_DEPTH_2,
  // maxpool(out_2, out_pool_2, OUT_CONV_HEIGHT_2, OUT_CONV_WIDTH_2, OUT_DEPTH_2,
          OUT_HEIGHT_2, OUT_WIDTH_2, OUT_DEPTH_2,
          STRIDE_MAX_2, POOL_SIZE_2, 2);

  // free(out_2);
  free(out_lrn_2);

  /******************************** Layer 3 ***********************************/
  alloc_size = IN_HEIGHT_3*IN_WIDTH_3*IN_DEPTH_3;
  float * in_3 = malloc(alloc_size * sizeof(float));
  if (!in_3) { perror("malloc failed"); exit(EXIT_FAILURE); };

  pad(out_pool_2, in_3, OUT_HEIGHT_2, OUT_WIDTH_2, OUT_DEPTH_2, PAD_IN_3);

  free(out_pool_2);

  alloc_size = OUT_WIDTH_3*OUT_HEIGHT_3*OUT_DEPTH_3;
  float * out_3 = malloc(alloc_size * sizeof(float));
  if (!out_3) { perror("malloc failed"); exit(EXIT_FAILURE); };
  // Inicializa out_3
  for( i = 0; i < OUT_WIDTH_3*OUT_HEIGHT_3*OUT_DEPTH_3; ++i) {
    out_3[i] = 0.0;
  }

  conv(in_3, weight_3, bias_3, out_3,
    FILTER_HEIGHT_3, FILTER_WIDTH_3,
    IN_HEIGHT_3, IN_WIDTH_3, IN_DEPTH_3,
    OUT_HEIGHT_3, OUT_WIDTH_3, OUT_DEPTH_3,
    STRIDE_CONV_3, alpha, 3);

  free(in_3);

  relu(out_3, OUT_HEIGHT_3, OUT_WIDTH_3, OUT_DEPTH_3);

  /******************************** Layer 4 ***********************************/
  alloc_size = IN_HEIGHT_4*IN_WIDTH_4*IN_DEPTH_4;
  float * in_4 = malloc(alloc_size * sizeof(float));
  if (!in_4) { perror("malloc failed"); exit(EXIT_FAILURE); };

  pad(out_3, in_4, OUT_HEIGHT_3, OUT_WIDTH_3, OUT_DEPTH_3, PAD_IN_4);

  free(out_3);

  alloc_size = OUT_WIDTH_4*OUT_HEIGHT_4*OUT_DEPTH_4;
  float * out_4 = malloc(alloc_size * sizeof(float));
  if (!out_4) { perror("malloc failed"); exit(EXIT_FAILURE); };
  // Inicializa out_4
  for( i = 0; i < OUT_WIDTH_4*OUT_HEIGHT_4*OUT_DEPTH_4; ++i) {
    out_4[i] = 0.0;
  }

  conv(in_4, weight_4, bias_4, out_4,
    FILTER_HEIGHT_4, FILTER_WIDTH_4,
    IN_HEIGHT_4, IN_WIDTH_4, IN_DEPTH_4,
    OUT_HEIGHT_4, OUT_WIDTH_4, OUT_DEPTH_4,
    STRIDE_CONV_4, alpha, 4);

  free(in_4);

  relu(out_4, OUT_HEIGHT_4, OUT_WIDTH_4, OUT_DEPTH_4);

  /******************************** Layer 5 ***********************************/
  alloc_size = IN_HEIGHT_5*IN_WIDTH_5*IN_DEPTH_5;
  float * in_5 = malloc(alloc_size * sizeof(float));
  if (!in_5) { perror("malloc failed"); exit(EXIT_FAILURE); };

  pad(out_4, in_5, OUT_HEIGHT_4, OUT_WIDTH_4, OUT_DEPTH_4, PAD_IN_5);

  free(out_4);

  alloc_size = OUT_CONV_WIDTH_5*OUT_CONV_HEIGHT_5*OUT_DEPTH_5;
  float * out_5 = malloc(alloc_size * sizeof(float));
  if (!out_5) { perror("malloc failed"); exit(EXIT_FAILURE); };
  // Inicializa out_5
  for( i = 0; i < OUT_CONV_WIDTH_5*OUT_CONV_HEIGHT_5*OUT_DEPTH_5; ++i) {
    out_5[i] = 0.0;
  }

  conv(in_5, weight_5, bias_5, out_5,
    FILTER_HEIGHT_5, FILTER_WIDTH_5,
    IN_HEIGHT_5, IN_WIDTH_5, IN_DEPTH_5,
    OUT_CONV_HEIGHT_5, OUT_CONV_WIDTH_5, OUT_DEPTH_5,
    STRIDE_CONV_5, alpha, 5);

  free(in_5);

  relu(out_5, OUT_CONV_HEIGHT_5, OUT_CONV_WIDTH_5, OUT_DEPTH_5);

  alloc_size = OUT_HEIGHT_5*OUT_WIDTH_5*OUT_DEPTH_5;
  float * out_pool_5 = malloc(alloc_size * sizeof(float));
  if (!out_pool_5) { perror("malloc failed"); exit(EXIT_FAILURE); };

  maxpool(out_5, out_pool_5, OUT_CONV_HEIGHT_5, OUT_CONV_WIDTH_5, OUT_DEPTH_5,
          OUT_HEIGHT_5, OUT_WIDTH_5, OUT_DEPTH_5,
          STRIDE_MAX_5, POOL_SIZE_5, 5);

  free(out_5);

  /******************************** Layer 6 ***********************************/
  alloc_size = OUT_HEIGHT_6*OUT_WIDTH_6*OUT_DEPTH_6;
  float * out_6 = malloc(alloc_size * sizeof(float));
  if (!out_6) { perror("malloc failed"); exit(EXIT_FAILURE); };

  fc_6(out_pool_5, out_6);

  free(out_pool_5);

  relu(out_6, OUT_HEIGHT_6, OUT_WIDTH_6, OUT_DEPTH_6);

  /******************************** Layer 7 ***********************************/
  alloc_size = OUT_HEIGHT_7*OUT_WIDTH_7*OUT_DEPTH_7;
  float * out_7 = malloc(alloc_size * sizeof(float));
  if (!out_7) { perror("malloc failed"); exit(EXIT_FAILURE); };

  fc_7(out_6, out_7);

  free(out_6);

  relu(out_7, OUT_HEIGHT_7, OUT_WIDTH_7, OUT_DEPTH_7);
  /******************************** Layer 8 ***********************************/
  alloc_size = OUT_HEIGHT_8*OUT_WIDTH_8*OUT_DEPTH_8;
  float * out_8 = malloc(alloc_size * sizeof(float));
  if (!out_8) { perror("malloc failed"); exit(EXIT_FAILURE); };

  fc_8(out_7, out_8);

  free(out_7);

  alloc_size = OUT_HEIGHT_8*OUT_WIDTH_8*OUT_DEPTH_8;
  float * soft_8 = malloc(alloc_size * sizeof(float));
  if (!soft_8) { perror("malloc failed"); exit(EXIT_FAILURE); };

  softmax(out_8, soft_8, OUT_HEIGHT_8, OUT_WIDTH_8, OUT_DEPTH_8);

  free(out_8);

  /****************************** Comparison **********************************/
  // FILE * out_f_w = fopen("outfiles_error/OUT_DATA_NO_ERROR.dat","w");
  // for (o_x = 0; o_x < OUT_HEIGHT_8*OUT_WIDTH_8*OUT_DEPTH_8; o_x++) {
  //   fprintf(out_f_w, "%f\n", soft_8[o_x]);
  // }

  FILE * out_f = fopen(argv[3],"r");
  alloc_size = OUT_HEIGHT_8*OUT_WIDTH_8*OUT_DEPTH_8;
  float * out_test = malloc(alloc_size * sizeof(float));
  if (!out_test) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
   fscanf(out_f, "%f", &(out_test[i]));
  }
  fclose(out_f);
  //
  error = 0.0;
  max = 0.0;
  i = 0;

  for(o_x = 0; o_x < OUT_HEIGHT_8*OUT_WIDTH_8*OUT_DEPTH_8; ++o_x) {

    // fprintf(out_f_w, "%f\n", soft_8[o_x]);

    if ( soft_8[o_x]-out_test[o_x] < 0) {
      error = -1.0 * (soft_8[o_x]-out_test[o_x]);
    } else {
      error = soft_8[o_x]-out_test[o_x];
    }

    // printf("soft_8[%4d] = %+2.9f | gold[%4d] = %+2.9f ->\t %2.9f\n", o_x, soft_8[o_x], o_x, out_test[o_x], error);
    if (error > max) {
      max = error;
    }
  }

  // printf("Max Error = %.9f\n", max);

  int top5_test[5] = {0};
  int top5_gold[5] = {0};
  // Deixar por ultimo, pq top5 escreve no vetor
  top5(out_test, top5_gold, soft_8, top5_test, 1000);

  // printf("Top 5 classes:\n");
  for(o_x=0;o_x<5;o_x++){
    // printf("rank %d: %d (gold = %d)\n", o_x, top5_test[o_x], top5_gold[o_x]);
    printf("%d=%d\n", top5_test[o_x], top5_gold[o_x]);
  }

  return  0;
}
