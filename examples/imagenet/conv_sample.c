#include <stdio.h>

// Valores do primeiro layer

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
#define LINE_STRIDE (STRIDE * IN_HEIGHT)

int main(int argc, char** argv) {
  int i;

  // Input/Output Files
  FILE *filter_f;
  FILE *in_f;
  FILE *bias_f;
  FILE *out_f;

  // Input/Output vectors
  float W[FILTER_WIDTH*FILTER_HEIGHT*K_FILTERS];
  float IN[IN_WIDTH*IN_HEIGHT*IN_DEPTH];
  float BIAS[OUT_DEPTH];
  float OUT_TEST[OUT_WIDTH*OUT_HEIGHT*OUT_DEPTH];
  float OUT[OUT_WIDTH*OUT_HEIGHT*OUT_DEPTH];

  // Loop indexes
  int o_s;     // Output depth slice
  int i_s;     // Input depth slice
  int o_x;     // Output x coordinate
  int o_y;     // Output y coordinate
  int f_x;     // Filter x coordiniate
  int f_y;     // Filter y coordinate

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

  filter_f = fopen("FILTER_CONV.dat","rb");
  in_f = fopen("IN_DATA_CONV.dat","rb");
  out_f = fopen("OUT_DATA_CONV.dat","rb");
  bias_f = fopen("BIAS_CONV.dat","rb");

  fread(W,sizeof(W),1,filter_f);
  fread(IN,sizeof(IN),1,in_f);
  fread(BIAS,sizeof(BIAS),1,bias_f);
  fread(OUT_TEST,sizeof(OUT_TEST),1,out_f);

  printf("Input size  = %ld\n", sizeof(IN)/sizeof(float) );
  printf("Filter size = %ld\n", sizeof(W)/sizeof(float) );
  printf("Bias size   = %ld\n", sizeof(BIAS)/sizeof(float) );
  printf("Input out test = %ld\n\n", sizeof(OUT_TEST)/sizeof(float) );

  // Inicializa OUT
  for( i = 0; i < (OUT_WIDTH*OUT_HEIGHT*OUT_DEPTH); ++i) {
    OUT[i] = 0.0;
  }

  // CONVOLUCAO
  for ( o_s = 0; o_s < OUT_DEPTH; o_s++) {
    out_slice = &OUT[o_s*OUT_HEIGHT*OUT_WIDTH];
    // Aponta para slice (ou canal) da saída (0 ~ 95)

    for ( i_s = 0; i_s < IN_DEPTH; i_s++) {
      filter_slice  = &W[ (o_s*FILTER_HEIGHT*FILTER_WIDTH*IN_DEPTH) + (i_s*FILTER_HEIGHT*FILTER_WIDTH) ];
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

          for ( f_y = 0; f_y < FILTER_HEIGHT; f_y++) {
            for ( f_x = 0; f_x < FILTER_WIDTH; f_x++) {
              sum += f_el[f_x] * in_el[f_x];
              // if( i_s == 0 && o_s == 0) {
              //   printf("%f\n", sum);
              // }
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

  for( o_s = 0; o_s < OUT_DEPTH; ++o_s ) {
    printf("OUTPUT SLICE %d\n", o_s);
    for(o_x = 0; o_x < OUT_HEIGHT*OUT_WIDTH; ++o_x) {

      if ( OUT[o_x]-OUT_TEST[o_x] < 0) {
        error = -1.0 * (OUT[o_x]-OUT_TEST[o_x]);
      } else {
        error = OUT[o_x]-OUT_TEST[o_x];
      }
      printf("OUT[%4d] = %+2.6f | OUT_TEST[%4d] = %+2.6f\t-> %+2.6f\n", o_x, OUT[o_x], o_x, OUT_TEST[o_x], error);
      if ((OUT[o_x]-OUT_TEST[o_x]) > max) {
        max = error;
      }
    }
  }

  printf("Max Error = %f\n", max);

  fclose(filter_f);
  fclose(in_f);
  fclose(out_f);
  fclose(bias_f);

  return 0;
}
