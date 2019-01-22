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
// #define OUT_HEIGHT 10
/*****************************/


#define WEIGHT_HEIGHT IN_HEIGHT
#define WEIGHT_WIDTH  IN_WIDTH
#define WEIGHT_DEPTH  IN_DEPTH
#define K_WEIGHTS     OUT_HEIGHT
#define IN_LOOP IN_DEPTH*IN_HEIGHT*IN_WIDTH
#define IN_SIZE IN_HEIGHT*IN_WIDTH
#define TOTAL_MAPS IN_DEPTH*OUT_HEIGHT
#define MAX_ERROR 3


typedef struct  { float  m[IN_SIZE];
                  int    group, used;
                } s_weights;

s_weights  mapas[IN_DEPTH*OUT_HEIGHT];

int cmpfunc (const void * a, const void * b) {
   return ( *(float*)a > *(float*)b );
}

int percent_of_avg(int size, float A[], float B[], float error_threshold, float map_threshold) {
  // Compare A and B as in:
  // % = | (a - b) / ((a + b)/2) |
  // true if % < threshold, false otherwise
  int i;
  int error_in_map;
  float diff;

  for(error_in_map=i=0; i<size; ++i) {

    diff = fabs( (A[i] - B[i]) ) / ( (A[i] + B[i])*0.5 );


    printf("[%3.4f  diff: %3.4f  --  %3.4f] ", A[i], diff, B[i]);

    if( diff > error_threshold ) {
      // printf("Failed for %f with %f x %f (%f)\n", error_threshold, A[i], B[i], diff);
      error_in_map++;
      printf(" **\n");
    }
    else
       printf("   \n");
  }

     printf("  ------------------------------MAX ERRO %d\n", error_in_map );

  // printf("Returning with %d erros inside map\n", error_in_map);
  if( error_in_map > map_threshold )
    return 0;
  else
    return 1;
}

int delta_a(int size, float A[], float B[], float min, float step, float map_threshold) {
  int i;
  int idx1, idx2;
  int semelhantes;

  for(semelhantes=i=0; i<size; ++i) {

    // discretiza
    idx1= (A[i] - min) / step;
    idx2= (B[i] - min) / step;

    //printf("[%3.4f %3.4f -  %d %d ] ", A[i],  B[i], idx1, idx2);

    if( abs(idx1-idx2) < 3 )       /// diferença de dois nos índices de discretização *
      { semelhantes++;
        if  (semelhantes > 25 ) return(1);    // alcançou 25 semelhantes
      }
  }

  return(0);
}

int delta_a_old(int size, float A[], float B[], float delta, float map_threshold) {
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

    printf("[%3.4f %3.4f %3.4f  --  %3.4f] ", A[i], upper_ref, lower_ref, B[i]);


    if( !(B[i] >= lower_ref && B[i] <= upper_ref) ) {
      // printf("Failed for delta = %f: %f < %f < %f @ %f\n", delta, lower_ref, A[i], upper_ref, B[i]);
      error_in_map++;
      printf(" **\n");
    // } else {
      // printf("Passed for delta = %f: %f < %f < %f @ %f\n", delta, lower_ref, A[i], upper_ref, B[i]);
    }
     else
       printf("   \n");
  }

     printf("  ------------------------------MAX ERRO %d\n", error_in_map );


  // printf("Returning with %d erros inside map\n", error_in_map);
  if( error_in_map > map_threshold )
    return 0;
  else
    return 1;
}

void avg(int size, int n, float avg[], float new[]) {
  int i;

  if(n==0) {
    for(i=0; i<size; i++)
      avg[i] = new[i];
  } else {
    for(i=0; i<size; i++)
      avg[i] = avg[i] + (new[i]-avg[i])/(n+1);
  }
}

float med(int size, float in[]) {
  if(size==0) {
    return 0.0;
  } else {
    qsort(in,size,sizeof(float),cmpfunc);
    if(size%2==0){
      return (in[size / 2 - 1] + in[size / 2]) / 2;
    } else {
      return in[size/2];
    }
  }
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
  float THRESHOLD = 0.2;
  int set,i,j,k,l,p;
  int group_number;
  float error,max,min,step,sum_n;

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

  min =  10000000.00;
  max = -10000000.00;
  for(i=0; i<IN_DEPTH*OUT_HEIGHT; i++)
     for(j=0; j<IN_SIZE; ++j) {
        if (mapas[i].m[j] < min)  min = mapas[i].m[j];
        if (mapas[i].m[j] > max)  max = mapas[i].m[j];
      }
  step = (max-min)/32;  // passo para discretizar
  printf("MIN  %f  MAX %f step %f ...\n", min, max, step);


  for(group_number=i=0; i<IN_DEPTH*OUT_HEIGHT; i++) {

     if( !(i%1000) ) printf("..%d..", i);
     fflush(stdout);

     if( !mapas[i].used ) {

       mapas[i].used = 1;
       mapas[i].group = group_number;

       for(j=i+1; j<IN_DEPTH*OUT_HEIGHT; j++) {
          //if(percent_of_avg(IN_SIZE, &(mapas[i].m[0]), &(mapas[j].m[0]), THRESHOLD, MAX_ERROR)) {
          if(delta_a(IN_SIZE, &(mapas[i].m[0]), &(mapas[j].m[0]), min, step, MAX_ERROR)) {
            //printf("Match: %d x %d\n", i, j);
            mapas[j].used = 1;  // marcados
            mapas[j].group = group_number;
          }
       }
       group_number++;  // estava no local errado // moraes
     }
   }

   printf("Number of groups: %d\n", group_number);


  // Cria um vetor de group_number mapas
  float **group_reps = (float**)malloc(group_number*sizeof(float*));
  for(i=0;i<group_number;i++)
    group_reps[i] = (float*)malloc(IN_SIZE*sizeof(float));

  // k =0;
  // for(i=0; i<group_number; i++) {
  //   l = 0;
  //   for(j=i+1; j<IN_DEPTH*OUT_HEIGHT; j++)
  //     if( mapas[j].group == i ) {
  //       avg(IN_SIZE, l, &(group_reps[i][0]),&(mapas[j].m[0]));
  //       l++; k++;
  //     }
  //   // printf("Grupo %3d:  elementos: %5d\n", i, l);
  // }
  int* group_sizes = malloc(group_number*sizeof(int));
  k =0;
  for(i=0; i<group_number; i++) {
    l = 0;
    for(j=i+1; j<IN_DEPTH*OUT_HEIGHT; j++)
      if( mapas[j].group == i ) {
        l++; k++;
      }
      group_sizes[i] = l;
  }
  printf("Total de elementos: %3d \n", k);

  // vec 3d temporario para guardar os elementos (mapas) de cada grupo
  float ***temp_vec = (float***)malloc(group_number*sizeof(float**));
  for(i=0; i<group_number; i++) {
    temp_vec[i] = (float**)malloc(group_sizes[i]*sizeof(float*));
    for(j=0; j<group_sizes[i]; j++)
      temp_vec[i][j] = (float*)malloc(IN_SIZE*sizeof(float));
  }

  for(i=0; i<group_number; i++) {
    p = 0;
    for(j=i+1; j<IN_DEPTH*OUT_HEIGHT; j++)
      if( mapas[j].group == i ) {
        for(k=0;k<IN_SIZE;k++){
          temp_vec[i][p][k] = mapas[j].m[k];
        }
        p++;
      }
  }

  // Monta o representante com o mediano em cada posicao dos elementos do grupo
  for(i=0; i<group_number; i++) {
    float *sorting_vec = malloc(group_sizes[i]*sizeof(float));
    for(j=0; j<IN_SIZE; j++){
      for(k=0; k<group_sizes[i]; k++) {
        sorting_vec[k] = temp_vec[i][k][j];
      }
      group_reps[i][j] = med(group_sizes[i],sorting_vec);
    }
    free(sorting_vec);
  }
  free(temp_vec);

  // Para testar a ideia... Subistituir os mapas por seus representantes no grupo
  for(i=0; i<group_number; i++) {
    for(j=i+1; j<IN_DEPTH*OUT_HEIGHT; j++)
      if( mapas[j].group == i ) {
        for(k=0; k<IN_SIZE; k++)
          mapas[j].m[k] = group_reps[i][k];
      }
  }

   // Para executar a camada e medir error FC com os novos pesos:
   float * W = malloc( alloc_size * sizeof(float));
   if (!W) { perror("malloc failed"); exit(EXIT_FAILURE); };

   for ( k = i = 0; i < OUT_HEIGHT*IN_DEPTH; i++) {
     for ( l = 0; l < IN_SIZE; l++) {
       W[k++] = mapas[i].m[l];
     }
   }

  // Check the result
  alloc_size = OUT_HEIGHT;
  float * OUT_TEST = malloc(alloc_size * sizeof(float));
  if (!OUT_TEST) { perror("malloc failed"); exit(EXIT_FAILURE); };

  float * OUT = malloc(alloc_size * sizeof(float));
  if (!OUT) { perror("malloc failed"); exit(EXIT_FAILURE); };

  printf("Starting Fully-Connected executions...\n");

  // With orginal set of weights
  fc_old(in, W_in, bias_6, OUT, IN_HEIGHT, IN_WIDTH, IN_DEPTH, OUT_HEIGHT);

  printf("FC with orignal set done!\n");

  // With reduced set of weights
  fc(in, W, bias_6, OUT_TEST, IN_HEIGHT, IN_WIDTH, IN_DEPTH, OUT_HEIGHT);

  printf("FC with reduced set done!\n");

  error = 0.0;
  max = 0.0;

  printf("FC executions done!\n");
  printf("Calculating absolute error...\n\n");

  for(sum_n=i=0; i < OUT_HEIGHT; i++ ) {
    error = fabs(OUT[i]-OUT_TEST[i]);
    sum_n += error;
    printf("OUT[%4d] = %+2.6f | OUT_TEST[%4d] = %+2.6f\t -> %+2.6f\n", i, OUT[i], i, OUT_TEST[i], error);

    if (error > max) {
      max = error;
    }
  }

  printf("Max Error = %f\n", max);
  printf("Erro medio = %f\n", sum_n/(float)OUT_HEIGHT);

  free(W);
  free(OUT);
  free(OUT_TEST);
  free(W_in);

  return 0;
}
