#include <stdio.h>

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

int main(int argc, char** argv) {
  int i;
  FILE *filter_f;
  FILE *in_f;
  FILE *out_f;

  float W[FILTER_WIDTH*FILTER_HEIGHT*K_FILTERS];
  float IN[IN_WIDTH*IN_HEIGHT*IN_DEPTH];
  float OUT[OUT_WIDTH*OUT_HEIGHT*OUT_DEPTH];

  filter_f = fopen("FILTER.dat","rb");
  in_f = fopen("IN_DATA.dat","rb");
  out_f = fopen("OUT_DATA.dat","rb");

  fread(W,sizeof(W),1,filter_f);
  fread(IN,sizeof(IN),1,in_f);
  fread(OUT,sizeof(OUT),1,out_f);

  for( i = 0; i < (FILTER_WIDTH*FILTER_HEIGHT*K_FILTERS); ++i) {
    printf("W[%d] = %f\n", i, W[i]);
  }

  for( i = 0; i < (IN_WIDTH*IN_HEIGHT*IN_DEPTH); ++i) {
    printf("IN[%d] = %f\n", i, IN[i]);
  }

  for( i = 0; i < (OUT_WIDTH*OUT_HEIGHT*OUT_DEPTH); ++i) {
    printf("OUT[%d] = %f\n", i, OUT[i]);
  }

  fclose(filter_f);
  fclose(in_f);
  fclose(out_f);
}
