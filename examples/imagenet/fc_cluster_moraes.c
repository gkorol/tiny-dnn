#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DEBUG  0


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
#define MAX_ERROR 3


typedef struct  { float  m[IN_SIZE];
                  int    group, used;
                  char   k[5];
                } s_weights;

s_weights  mapas[IN_DEPTH*OUT_HEIGHT];

int cmpfunc (const void * a, const void * b) {
   return ( *(float*)a > *(float*)b );
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

int normal(float v, float min, float step) {

   float idx;

   idx = (v-min) / step;

   return (  (int) idx );

}

int main(int argc, char** argv) {
  FILE *file_in;

  int pt, p,i,j,k,l, cont, tot_elementos;
  int group_number;
  float max,min,step,error,sum_n,mean,sd;


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

  min =  10000000.00;
  max = -10000000.00;
  for(i=0; i<IN_DEPTH*OUT_HEIGHT; i++)
     for(j=0; j<IN_SIZE; ++j) {
        if (mapas[i].m[j] < min)  min = mapas[i].m[j];
        if (mapas[i].m[j] > max)  max = mapas[i].m[j];
      }
  step = (max-min)/32;  // passo para discretizar
  printf("MIN  %f  MAX %f step %f ...\n", min, max, step);


  tot_elementos = 0;

   // nenhum mapa está utilizado em algum grupo /////
  for(i=0; i<IN_DEPTH*OUT_HEIGHT; i++)
     {  mapas[i].used = 0;
        mapas[i].group = -1;
     }

  //
  //  MONTAGEM DOS GRUPOS
  //
  for(group_number=i=0; i<IN_DEPTH*OUT_HEIGHT; i++)
    if( !mapas[i].used )    // mapas já usados em outros grupos não são utilizados
    {
       /////////////// marca o mapa como visitado e atribui um grupo a ele
       mapas[i].used = 1;
       mapas[i].group = group_number;

       for(j=i+1; j<IN_DEPTH*OUT_HEIGHT; j++)
         if( !mapas[j].used )   // mapa não foi utilizado
         {
            int kill0 = 0;
            int kill1 = 0;
            int kill2 = 0;
            int kill3 = 0;
            int kill4 = 0;
            int kill5 = 0;

            // percorre todos os pontos da matriz 'i'
            for(pt=0; pt<IN_SIZE; pt++)
               {
                  int diff =  abs(normal(mapas[i].m[pt], min, step) - normal(mapas[j].m[pt], min, step));

                  if( diff == 0) kill0++;
                  if( diff == 1) kill1++;
                  if( diff == 2) kill2++;
                  if( diff == 3) kill3++;
                  if( diff == 4) kill4++;
                  if( diff == 5) kill5++;
               }

            // abaixo de 50% do universo - pelo menos 21 muito próximos  e o restante com +/- 3
            if( tot_elementos < ( IN_DEPTH*OUT_HEIGHT/2) )
                  if(  (kill0+kill1+kill2)>20 && (kill0+kill1+kill2+kill3)>35  )
                     {  mapas[j].group = group_number;     // atribui um numero de grupo para 'j'
                        mapas[j].used =  1;                // nao usa mais o mapa 'j' para testescont++;
                     }

            // entre 50% e 95% do universo:  relaxa:  32 próximos e 4 até distância 4
            if( (tot_elementos > ( IN_DEPTH*OUT_HEIGHT/2) ) && (tot_elementos < ( (IN_DEPTH*OUT_HEIGHT*95) / 100)) )
                  // começa a relaxar
                  if(  (kill0+kill1+kill2+kill3)>31 &&  kill4 <5  )
                     {  mapas[j].group = group_number;     // atribui um numero de grupo para 'j'
                        mapas[j].used =  1;                // nao usa mais o mapa 'j' para testescont++;
                     }

           // acima de 75% do universo
           if( tot_elementos > ( (IN_DEPTH*OUT_HEIGHT*3) / 4) )
                  // começa a relaçar
                  if(   (kill0+kill1+kill2+kill3)>25 &&  kill4 <4 && kill5<3  )
                     {  mapas[j].group = group_number;     // atribui um numero de grupo para 'j'
                        mapas[j].used =  1;                // nao usa mais o mapa 'j' para testescont++;
                     }
        }


      if(DEBUG) for(pt=0; pt<IN_SIZE; pt++)  printf( "%3d ",  normal(mapas[i].m[pt], min, step) );
      if(DEBUG) puts("");

      cont = 1;

      for(j=i+1; j<IN_DEPTH*OUT_HEIGHT; j++)
          if( mapas[i].group == mapas[j].group)                 // faz parte do grupo 'i
              { if(DEBUG) printf("-------------%d\n", j);

                cont++;

                if(DEBUG) for(pt=0; pt<IN_SIZE; pt++)  printf( "%3d ",  normal(mapas[i].m[pt], min, step) - normal(mapas[j].m[pt], min, step) );
                if(DEBUG) puts("");
              }

     if( tot_elementos < ( IN_DEPTH*OUT_HEIGHT/2) )                          printf("00");
     if( (tot_elementos > ( IN_DEPTH*OUT_HEIGHT/2 +1)) && (tot_elementos < ( (IN_DEPTH*OUT_HEIGHT*3) / 4)) )                           printf("11");
     if( tot_elementos > ( (IN_DEPTH*OUT_HEIGHT*3) / 4) )                          printf("22");


     tot_elementos += cont;

     printf("--->  Mapa: %5d  group: %4d   elementos: %6d      TOTAL: %6d   \n", i, group_number, cont, tot_elementos);


     group_number++;

   }


   printf("Number of groups: %d\n", group_number);

// return 0;

  int* group_sizes = malloc(group_number*sizeof(int));
   k =0;
   for(i=0; i<group_number; i++) {
       l = 0;
       for(j=i+1; j<IN_DEPTH*OUT_HEIGHT; j++)
          if( mapas[j].group == i )
            { l++; k++;}
       // printf("Grupo %3d:  elementos: %5d\n", i, l);
       group_sizes[i] = l;
     }
   printf("Total de elementos %3d: \n", k);

   ///////////////////
   // Cria um vetor de group_number mapas
   float **group_reps = (float**)malloc(group_number*sizeof(float*));
   for(i=0;i<group_number;i++)
     group_reps[i] = (float*)malloc(IN_SIZE*sizeof(float));
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
   min =  10000000.00;
   max = -10000000.00;

   printf("FC executions done!\n");
   printf("Calculating absolute error...\n\n");

   for(sum_n=i=0; i < OUT_HEIGHT; i++ ) {
     error = fabs(OUT[i]-OUT_TEST[i]);
     sum_n += error;
     printf("OUT[%4d] = %+2.6f | OUT_TEST[%4d] = %+2.6f\t -> %+2.6f\n", i, OUT[i], i, OUT_TEST[i], error);

     if(error > max)
       max = error;
     if(error < min)
       min = error;
   }

   mean = sum_n/(float)OUT_HEIGHT;

   for(sd=i=0; i<OUT_HEIGHT; i++)
       sd += pow(OUT_TEST[i] - (mean), 2);

   sd = sqrt(sd/OUT_HEIGHT);

   printf("Max Error = %f\n", max);
   printf("Min Error = %f\n", min);
   printf("Erro medio = %f\n", mean);
   printf("Desvio pad. = %f\n", sd);

   free(W);
   free(OUT);
   free(OUT_TEST);
   free(W_in);

  return 0;
}
