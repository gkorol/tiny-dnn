#include <stdio.h>
#include <stdlib.h>

/******* Layer FC 6 ********/
#include "param_headers/6_weight.h"
// #include "param_headers/6_in.h"
// #include "param_headers/6_bias.h"
#define IN_HEIGHT 6
#define IN_WIDTH  6
#define IN_DEPTH  256

#define OUT_HEIGHT 4096
#define OUT_WIDTH  1
#define OUT_DEPTH  1
/*****************************/

/******* Layer FC 7 ********/
// #include "param_headers/7_weight.h"
// #include "param_headers/7_in.h"
// #include "param_headers/7_bias.h"
// #define IN_HEIGHT 4096
// #define IN_WIDTH  1
// #define IN_DEPTH  1
//
// #define OUT_HEIGHT 4096
// #define OUT_WIDTH  1
// #define OUT_DEPTH  1
/*****************************/

/******* Layer FC 8 ********/
// #include "param_headers/8_weight.h"
// #include "param_headers/8_in.h"
// #include "param_headers/8_bias.h"
// #define IN_HEIGHT 4096
// #define IN_WIDTH  1
// #define IN_DEPTH  1
//
// #define OUT_HEIGHT 1000
// #define OUT_WIDTH  1
// #define OUT_DEPTH  1
/*****************************/

#define WEIGHT_HEIGHT IN_HEIGHT
#define WEIGHT_WIDTH  IN_WIDTH
#define WEIGHT_DEPTH  IN_DEPTH
#define K_WEIGHTS     OUT_HEIGHT

#define IN_LOOP IN_DEPTH*IN_HEIGHT*IN_WIDTH

int main(int argc, char** argv) {
  int i,j,k;
  float* current_map;
  float* target_map;

  float* cluster_dir[OUT_HEIGHT] = {NULL};
  for(i=0; i<OUT_HEIGHT; i++) {
    // aloca array de ponteiros do tamanho do filtro. 4096 filtros.
    cluster_dir[i] = malloc( IN_DEPTH * sizeof(float*));
  }

  alloc_size = WEIGHT_WIDTH*WEIGHT_HEIGHT*WEIGHT_DEPTH*K_WEIGHTS;
  float * W = malloc( alloc_size * sizeof(float));
  if (!W) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
    // W[i] = weight[i];
    W[i] = 0.0;
  }

  // Rearranja vetor de pesos por entrada
  k = 0;
  for ( i = 0; i < OUT_HEIGHT; i++) {
    for ( j = 0; j < IN_LOOP; j++) {
      W[k] = weight[j * OUT_HEIGHT + i];
      k++;
    }
  }

  // for ( i = 0; i < OUT_HEIGHT*IN_DEPTH; i++ ) {
  //   current_map = W + (i_s * OUT_HEIGHT + o_y);
  //   for ( j = 0; j < OUT_HEIGHT*IN_DEPTH; j++ ) {
  //     target_map
  //   }
  // }

  // for ( o_y = 0; o_y < OUT_HEIGHT; o_y++) {
  //   // Percorre os elementos no vetor saida (0 ~ 2048)
  //   OUT[o_y] = 0.0;
  //   for ( i_s = 0; i_s < IN_LOOP; i_s++) {
  //     // Percorre o vetor entrda (0 ~ 9216)
  //     OUT[o_y] += IN[i_s] * W[i_s * OUT_HEIGHT + o_y];
  //     // Incrementa a saida com a mult. do peso pela entrada
  //   }
  //   OUT[o_y] += BIAS[o_y];
  //   // Soma bias
  // }

  free(W);

  return 0;
}
