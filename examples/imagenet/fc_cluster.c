#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <sys/queue.h>

/******* Layer FC 6 ********/
// #include "param_headers/6_weight.h"
#include "param_headers/6_in.h"
#include "param_headers/6_bias.h"
#define IN_HEIGHT 6
#define IN_WIDTH  6
#define IN_DEPTH  256

#define OUT_HEIGHT 4096
// #define OUT_HEIGHT 20
/*****************************/


#define WEIGHT_HEIGHT IN_HEIGHT
#define WEIGHT_WIDTH  IN_WIDTH
#define WEIGHT_DEPTH  IN_DEPTH
#define K_WEIGHTS     OUT_HEIGHT
#define IN_LOOP IN_DEPTH*IN_HEIGHT*IN_WIDTH
#define IN_SIZE IN_HEIGHT*IN_WIDTH
#define TOTAL_MAPS IN_DEPTH*OUT_HEIGHT
#define MAX_ERROR 2


typedef struct  { float  m[IN_SIZE];
                  int    group, used;
                } s_weights;

s_weights  mapas[IN_DEPTH*OUT_HEIGHT];

int percent_of_avg(int size, float A[], float B[], float error_threshold, float map_threshold) {
  // Compare A and B as in:
  // % = | (a - b) / ((a + b)/2) |
  // true if % < threshold, false otherwise
  int i;
  int error_in_map;
  float diff;

  for(error_in_map=i=0; i<size; ++i) {
    diff = fabs( (A[i] - B[i]) ) / ( (A[i] + B[i])*0.5 );
    if( diff > error_threshold ) {
      // printf("Failed for %f with %f x %f (%f)\n", error_threshold, A[i], B[i], diff);
      error_in_map++;
    }
  }

  // printf("Returning with %d erros inside map\n", error_in_map);
  if( error_in_map > map_threshold )
    return 0;
  else
    return 1;
}

int delta_a(int size, float A[], float B[], float delta, float map_threshold) {
  // Return true when up to size-map_threshold points in B are in the range
  // of delta*A points
  // delta must be > 0
  int i;
  int error_in_map;
  float diff;
  float upper_ref, lower_ref;

  if(delta<=0)
    return 0;

  for(error_in_map=i=0; i<size; ++i) {
    if( A[i] >= 0) {
      upper_ref = A[i] + A[i]*delta;
      lower_ref = A[i] - A[i]*delta;
    } else {
      upper_ref = A[i] - A[i]*delta;
      lower_ref = A[i] + A[i]*delta;
    }
    if( !(B[i] >= lower_ref && B[i] <= upper_ref) ) {
      // printf("Failed for delta = %f: %f < %f < %f @ %f\n", delta, lower_ref, A[i], upper_ref, B[i]);
      error_in_map++;
    // } else {
      // printf("Passed for delta = %f: %f < %f < %f @ %f\n", delta, lower_ref, A[i], upper_ref, B[i]);
    }
  }
  // printf("Returning with %d erros inside map\n", error_in_map);
  if( error_in_map > map_threshold )
    return 0;
  else
    return 1;
}

void fc_old(float in[], float weights[], float bias[], float out[],
  const unsigned ih, const unsigned iw, const unsigned id, const unsigned oh ){

  unsigned o_y, i_s;
  unsigned in_size = ih*iw*id;

  for ( o_y = 0; o_y < oh; o_y++) {
    out[o_y] = 0.0;
    for ( i_s = 0; i_s < in_size; i_s++) {
      out[o_y] += in[i_s] * weights[i_s * oh + o_y];
    }
    out[o_y] += bias[o_y];
  }
}

void fc(float in[], float weights[], float bias[], float out[],
  const unsigned ih, const unsigned iw, const unsigned id, const unsigned oh ){

  unsigned o_y, i_s;
  unsigned in_size = ih*iw*id;

  for ( o_y = 0; o_y < oh; o_y++) {
    out[o_y] = 0.0;
    for ( i_s = 0; i_s < in_size; i_s++) {
      out[o_y] += in[i_s] * weights[i_s + o_y * in_size];
    }
    out[o_y] += bias[o_y];
  }
}

int main(int argc, char** argv) {
  FILE *file_in;

  // float THRESHOLD = 0.05;
  float THRESHOLD = 0.5;
  int set, i,j,k,l;
  int group_number;
  float error,max;

  for(i=0; i<IN_DEPTH*OUT_HEIGHT; i++)
      mapas[i].used = mapas[i].group = 0;

  printf("Reading input file...\n");
  file_in = fopen("6_weight.txt","r");

  unsigned alloc_size = WEIGHT_WIDTH*WEIGHT_HEIGHT*WEIGHT_DEPTH*K_WEIGHTS;
  float * W_in = malloc( alloc_size * sizeof(float));
  if (!W_in) { perror("malloc failed"); exit(EXIT_FAILURE); };
  for (i=0; i < alloc_size; ++i){
    fscanf(file_in, "%f", &(W_in[i]));
  }
  fclose(file_in);

  // Arranja vetor de pesos por canais de entrada e nao por neuronio
  for ( i = 0; i < OUT_HEIGHT; i++) {
    for ( j = 0; j < IN_DEPTH; j++) {
      for ( l = 0; l < IN_SIZE; l++) {
        mapas[i*IN_DEPTH+j].m[l] = W_in[(j*IN_SIZE+l) * OUT_HEIGHT + i];
      }
    }
  }

  printf("MAP of size = %d (%d x %d)\n", IN_SIZE, IN_HEIGHT, IN_WIDTH);
  printf("MAPs by neuron (channels) = %d\n", IN_DEPTH);
  printf("Total of neurons = %d\n", OUT_HEIGHT);
  printf("Number of maps = %d\n", OUT_HEIGHT*IN_DEPTH);
  printf("Starting for %.2f threshold at most %d positions...\n", THRESHOLD, MAX_ERROR);

  for(group_number=i=0; i<IN_DEPTH*OUT_HEIGHT; i++) {
     if( !mapas[i].used ) {
       mapas[i].used = 1;
       mapas[i].group = group_number;
       group_number++;
       for(j=i+1; j<IN_DEPTH*OUT_HEIGHT; j++) {
          if(percent_of_avg(IN_SIZE, &(mapas[i].m[0]), &(mapas[j].m[0]), THRESHOLD, MAX_ERROR)) {
          // if(delta_a(IN_SIZE, &(mapas[i].m[0]), &(mapas[j].m[0]), THRESHOLD, MAX_ERROR)) {
            // printf("Match: %d x %d\n", i, j);
            mapas[j].used = 1;  // marcados
            mapas[j].group = group_number;
          }
       }
     }
   }

   printf("Number of groups: %d\n", group_number);

   // Para executar a camada e medir error FC com os novos pesos:
  //  float * W = malloc( alloc_size * sizeof(float));
  //  if (!W) { perror("malloc failed"); exit(EXIT_FAILURE); };
  //
  //  for ( k = i = 0; i < OUT_HEIGHT*IN_DEPTH; i++) {
  //    for ( l = 0; l < IN_SIZE; l++) {
  //      W[k++] = mapas[i].m[l];
  //    }
  //  }
  //
  // // Check the result
  // alloc_size = OUT_HEIGHT;
  // float * OUT_TEST = malloc(alloc_size * sizeof(float));
  // if (!OUT_TEST) { perror("malloc failed"); exit(EXIT_FAILURE); };
  //
  // float * OUT = malloc(alloc_size * sizeof(float));
  // if (!OUT) { perror("malloc failed"); exit(EXIT_FAILURE); };
  //
  // printf("Starting Fully-Connected executions...\n");
  //
  // // With orginal set of weights
  // fc_old(in, W_in, bias_6, OUT, IN_HEIGHT, IN_WIDTH, IN_DEPTH, OUT_HEIGHT);
  //
  // printf("FC with orignal set done!\n");
  //
  // // With reduced set of weights
  // fc(in, W, bias_6, OUT_TEST, IN_HEIGHT, IN_WIDTH, IN_DEPTH, OUT_HEIGHT);
  //
  // printf("FC with reduced set done!\n");
  //
  // error = 0.0;
  // max = 0.0;
  //
  // printf("FC executions done!\n");
  // printf("Calculating absolute error...\n\n");
  //
  // for( i = 0; i < OUT_HEIGHT; i++ ) {
  //   error = fabs(OUT[i]-OUT_TEST[i]);
  //
  //   printf("OUT[%4d] = %+2.6f | OUT_TEST[%4d] = %+2.6f\t -> %+2.6f\n", i, OUT[i], i, OUT_TEST[i], error);
  //
  //   if (error > max) {
  //     max = error;
  //   }
  // }
  //
  // printf("Max Error = %f\n", max);
  //
  //
  // free(W);
  // free(OUT);
  // free(OUT_TEST);
  free(W_in);

  return 0;
}
