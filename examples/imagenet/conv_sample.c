#include <stdio.h>
#include <stdlib.h>

/******* Layer Conv 1 ********/
#include "param_headers/1_weight.h"
#include "param_headers/1_in.h"
#include "param_headers/1_bias.h"
#define BIPARTITE 0
#define FILTER_HEIGHT 11
#define FILTER_WIDTH  11
#define K_FILTERS     96
#define IN_HEIGHT 227
#define IN_WIDTH  227
#define IN_DEPTH  3
#define OUT_HEIGHT 55
#define OUT_WIDTH  55
#define OUT_DEPTH  96
#define STRIDE 4
#define LINE_STRIDE 908 //(STRIDE * IN_HEIGHT)
/*****************************/

/******* Layer Conv 2 ********/
// #include "param_headers/2_weight.h"
// #include "param_headers/2_in.h"
// #include "param_headers/2_bias.h"
// #define BIPARTITE 1
// #define FILTER_HEIGHT 5
// #define FILTER_WIDTH  5
// #define K_FILTERS     256
//
// #define IN_HEIGHT 31
// #define IN_WIDTH  31
// #define IN_DEPTH  96
//
// #define OUT_HEIGHT 27
// #define OUT_WIDTH  27
// #define OUT_DEPTH  256
// #define STRIDE 1
// #define LINE_STRIDE 31 //(STRIDE * IN_HEIGHT)
/*****************************/

/******* Layer Conv 3 ********/
// #include "param_headers/3_weight.h"
// #include "param_headers/3_in.h"
// #include "param_headers/3_bias.h"
// #define BIPARTITE 1
// #define FILTER_HEIGHT 3
// #define FILTER_WIDTH  3
// #define K_FILTERS     256
//
// #define IN_HEIGHT 15
// #define IN_WIDTH  15
// #define IN_DEPTH  256
//
// #define OUT_HEIGHT 13
// #define OUT_WIDTH  13
// #define OUT_DEPTH  384
// #define STRIDE 1
// #define LINE_STRIDE 15 //(STRIDE * IN_HEIGHT)
/*****************************/

/******* Layer Conv 4 ********/
// #include "param_headers/4_weight.h"
// #include "param_headers/4_in.h"
// #include "param_headers/4_bias.h"
// #define BIPARTITE 1
// #define FILTER_HEIGHT 3
// #define FILTER_WIDTH  3
// #define K_FILTERS     384
//
// #define IN_HEIGHT 13
// #define IN_WIDTH  13
// #define IN_DEPTH  384
//
// #define OUT_HEIGHT 15
// #define OUT_WIDTH  15
// #define OUT_DEPTH  384
// #define STRIDE 1
// #define LINE_STRIDE 13 //(STRIDE * IN_HEIGHT)
/*****************************/

/******* Layer Conv 5 ********/
// #include "param_headers/5_weight.h"
// #include "param_headers/5_in.h"
// #include "param_headers/5_bias.h"
// #define BIPARTITE 1
// #define FILTER_HEIGHT 3
// #define FILTER_WIDTH  3
// #define K_FILTERS     256
//
// #define IN_HEIGHT 15
// #define IN_WIDTH  15
// #define IN_DEPTH  384
//
// #define OUT_HEIGHT 13
// #define OUT_WIDTH  13
// #define OUT_DEPTH  256
// #define STRIDE 1
// #define LINE_STRIDE 15 //(STRIDE * IN_HEIGHT)
/*****************************/

int main(int argc, char** argv) {
  unsigned i;

  // Input/Output Files
  FILE *out_f;

  // Loop indexes
  unsigned o_s;     // Output depth slice
  unsigned i_s;     // Input depth slice
  unsigned o_x;     // Output x coordinate
  unsigned o_y;     // Output y coordinate
  unsigned f_x;     // Filter x coordiniate
  unsigned f_y;     // Filter y coordinate

  float sum;
  float * in_el;
  float * f_el;
  float * in_slice;
  float * filter_slice;
  float * in_line;
  float * o_line;
  float * out_slice;

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
    IN[i] = in_1[i];
  }

  alloc_size = OUT_DEPTH;
  float * BIAS = malloc(alloc_size * sizeof(float));
  if (!BIAS) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
    BIAS[i] = bias_1[i];
  }

  alloc_size = OUT_WIDTH*OUT_HEIGHT*OUT_DEPTH;
  float * OUT_TEST = malloc(alloc_size * sizeof(float));
  if (!OUT_TEST) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
    fscanf(out_f, "%f", &(OUT_TEST[i]));
  }
  fclose(out_f);

  float * OUT = malloc(alloc_size * sizeof(float));
  if (!OUT) { perror("malloc failed"); exit(EXIT_FAILURE); };

  alloc_size = FILTER_WIDTH*FILTER_HEIGHT*IN_DEPTH*K_FILTERS;
  float * W = malloc( alloc_size * sizeof(float));
  if (!W) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
    W[i] = weight_1[i];
  }

  printf("Input size  = %d\n", IN_WIDTH*IN_HEIGHT*IN_DEPTH );
  printf("Weight size = %d\n", FILTER_WIDTH*FILTER_HEIGHT*IN_DEPTH*K_FILTERS );
  printf("Bias size   = %d\n", OUT_DEPTH );
  printf("Input out test = %d\n\n", OUT_WIDTH*OUT_HEIGHT*OUT_DEPTH );


  // Inicializa OUT
  for( i = 0; i < (OUT_WIDTH*OUT_HEIGHT*OUT_DEPTH); ++i) {
    OUT[i] = 0.0;
  }

  // CONVOLUCAO
  for ( o_s = 0; o_s < OUT_DEPTH; o_s++) {
    out_slice = &OUT[o_s*OUT_HEIGHT*OUT_WIDTH];
    // Aponta para slice (ou canal) da saída (0 ~ 95)

    for ( i_s = 0; i_s < IN_DEPTH ; i_s++) {

      // if( BIPARTITE && o_s < (OUT_DEPTH/2) ) {
      //   if( i_s < (IN_DEPTH/2) ) {
      //     // fprintf(test, "o_s = %d , i_s = %d -> 0\n", o_s, i_s);
      //     break;
      //   } else {
      //     // fprintf(test, "o_s = %d , i_s = %d -> 1\n", o_s, i_s);
      //     continue;
      //   }
      // } else if (BIPARTITE) {
      //   if( i_s < (IN_DEPTH/2) ) {
      //     // fprintf(test, "o_s = %d , i_s = %d -> 1\n", o_s, i_s);
      //     continue;
      //   } else {
      //     // fprintf(test, "o_s = %d , i_s = %d -> 0\n", o_s, i_s);
      //     break;
      //   }
      // }

      filter_slice  = &W[ (o_s*FILTER_HEIGHT*FILTER_WIDTH*IN_DEPTH) + (i_s*FILTER_HEIGHT*FILTER_WIDTH) ]; //
      // Aponta para conjunto de filtros que gera cada slice de saida (0 ~ 95)
      in_slice = &IN[ i_s*IN_HEIGHT*IN_WIDTH ];
      // Aponta para slice da entrada (R G B)

      o_line = out_slice;
      // Aponta para uma linha do slice de saida atual (0 ~ 54)

      for ( o_y = 0; o_y < OUT_HEIGHT; o_y++) {
        in_line = in_slice;
        // Aponta para uma janela do slice de entrada atual (0 ~ 226)

        for ( o_x = 0; o_x < OUT_WIDTH; o_x++) {
          in_el = in_line;
          // Aponta para posicao na janela de entrada que vai convoluir (0 ~ 226)

          f_el = filter_slice;
          // Aponta para o filtro atual (0 ~ 96)

          sum = 0.0;
          // Inicializa somatorio

          // fprintf(test, "%3d %3d %3d %3d %3d\n", o_s, i_s, o_y, o_x, (o_s*FILTER_HEIGHT*FILTER_WIDTH*IN_DEPTH) + (i_s*FILTER_HEIGHT*FILTER_WIDTH));

          for ( f_y = 0; f_y < FILTER_HEIGHT; f_y++) {
            for ( f_x = 0; f_x < FILTER_WIDTH; f_x++) {
              sum += f_el[f_x] * in_el[f_x]; //
              // Multiplica posicao no filtro atual com posicao na janela atual de entrada
            }
            f_el += FILTER_WIDTH;
            // Proxima linha do filtro

            in_el += IN_WIDTH;
            // Proxima linha de entrada
          }
          o_line[o_x] += sum;
          // Somatorio da posicao o_x de saida (0 ~ 2)

          in_line += STRIDE;
          // Proxima janela
        }
        o_line += OUT_WIDTH;
        // Proxima linha da saida

        in_slice += LINE_STRIDE;
        // Proxima linha (considerando strides)
      }
    }

    for(i = 0; i < OUT_HEIGHT*OUT_WIDTH; i++) {
      out_slice[i] += BIAS[o_s];
      // Soma bias no slice
    }
  }

  error = 0.0;
  max = 0.0;
  i = 0;

  for( o_s = 0; o_s < OUT_DEPTH; ++o_s ) {
    printf("OUTPUT SLICE %d\n", o_s);
    for(o_x = 0; o_x < OUT_HEIGHT*OUT_WIDTH; ++o_x) {

      i = o_s*OUT_HEIGHT*OUT_WIDTH + o_x;

      if ( OUT[i]-OUT_TEST[i] < 0) {
        error = -1.0 * (OUT[i]-OUT_TEST[i]);
      } else {
        error = OUT[i]-OUT_TEST[i];
      }
      printf("OUT[%4d] = %+2.6f | OUT_TEST[%4d] = %+2.6f -> %+2.6f\n", i, OUT[i], i, OUT_TEST[i], error);
      if (error > max) {
        max = error;
      }
    }
  }

  printf("Max Error = %f\n", max);

  return 0;
}
