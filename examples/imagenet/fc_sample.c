#include <stdio.h>
#include <stdlib.h>

/******* Layer FC 6 ********/
// #include "param_headers/6_weight.h"
// #include "param_headers/6_in.h"
// #include "param_headers/6_bias.h"
// #define IN_HEIGHT 6
// #define IN_WIDTH  6
// #define IN_DEPTH  256
//
// #define OUT_HEIGHT 4096
// #define OUT_WIDTH  1
// #define OUT_DEPTH  1
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
#include "param_headers/8_weight.h"
#include "param_headers/8_in.h"
#include "param_headers/8_bias.h"
#define IN_HEIGHT 4096
#define IN_WIDTH  1
#define IN_DEPTH  1

#define OUT_HEIGHT 1000
#define OUT_WIDTH  1
#define OUT_DEPTH  1
/*****************************/

#define WEIGHT_HEIGHT IN_HEIGHT
#define WEIGHT_WIDTH  IN_WIDTH
#define WEIGHT_DEPTH  IN_DEPTH
#define K_WEIGHTS     OUT_HEIGHT

#define IN_LOOP IN_DEPTH*IN_HEIGHT*IN_WIDTH

int main(int argc, char** argv) {
  int i;

  FILE *out_f;

  // Loop indexes
  int i_s;     // Input depth slice
  int o_y;     // Output y coordinate

  float error;
  float max;

  out_f = fopen("transfer_files/OUT_DATA.dat","r");

  size_t alloc_size;
  // Input/Output vectors
  // Dynamically allocating, so they dont blow the stack out!
  alloc_size = IN_WIDTH*IN_HEIGHT*IN_DEPTH;
  float * IN = malloc( alloc_size * sizeof(float));
  if (!IN) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
    IN[i] = in[i];
  }

  alloc_size = OUT_HEIGHT;
  float * BIAS = malloc(alloc_size * sizeof(float));
  if (!BIAS) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
    BIAS[i] = bias[i];
  }

  alloc_size = OUT_HEIGHT;
  float * OUT_TEST = malloc(alloc_size * sizeof(float));
  if (!OUT_TEST) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
    fscanf(out_f, "%f", &(OUT_TEST[i]));
  }
  fclose(out_f);

  float * OUT = malloc(alloc_size * sizeof(float));
  if (!OUT) { perror("malloc failed"); exit(EXIT_FAILURE); };

  alloc_size = WEIGHT_WIDTH*WEIGHT_HEIGHT*WEIGHT_DEPTH*K_WEIGHTS;
  float * W = malloc( alloc_size * sizeof(float));
  if (!W) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
    W[i] = weight[i];
  }

  printf("Input size  = %d\n", IN_WIDTH*IN_HEIGHT*IN_DEPTH );
  printf("Weight size = %d\n", WEIGHT_WIDTH*WEIGHT_HEIGHT*WEIGHT_DEPTH*K_WEIGHTS );
  printf("Bias size   = %d\n", OUT_HEIGHT );
  printf("Input out test = %d\n\n", OUT_HEIGHT );

  // FULLY CONNECTED
  for ( o_y = 0; o_y < OUT_HEIGHT; o_y++) {
    // Percorre os elementos no vetor saida (0 ~ 2048)
    OUT[o_y] = 0.0;

    for ( i_s = 0; i_s < IN_LOOP; i_s++) {
      // Percorre o vetor entrda (0 ~ 9216)

      OUT[o_y] += IN[i_s] * W[i_s * OUT_HEIGHT + o_y];
      // Incrementa a saida com a mult. do peso pela entrada

    }

    OUT[o_y] += BIAS[o_y];
    // Soma bias
  }

  error = 0.0;
  max = 0.0;

  for( o_y = 0; o_y < OUT_HEIGHT; o_y++ ) {

    if ( OUT[o_y]-OUT_TEST[o_y] < 0)
      error = -1.0 * (OUT[o_y]-OUT_TEST[o_y]);
    else
    error = OUT[o_y]-OUT_TEST[o_y];

    printf("OUT[%4d] = %+2.6f | OUT_TEST[%4d] = %+2.6f\t-> %+2.6f\n", o_y, OUT[o_y], o_y, OUT_TEST[o_y], error);

    if (error > max) {
      max = error;
    }
  }

  printf("Max Error = %f\n", max);

  free(W);
  free(IN);
  free(BIAS);
  free(OUT);
  free(OUT_TEST);

  return 0;
}
