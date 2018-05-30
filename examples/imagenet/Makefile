#mini makefile pra facilitar...

CXX = g++
# CXX = arm-linux-gnueabi-g++

CFLAGS = -std=gnu++11 -g -O3 -ftree-vectorize -funsafe-math-optimizations -DDNN_USE_IMAGE_API -I../../
# CFLAGS = -std=gnu++14 -g -O1 -march=armv8-a -ftree-vectorize -funsafe-math-optimizations -DDNN_USE_IMAGE_API -I../../

LDFLAGS = -lm
# LDFLAGS = -lm -static

src = test.cpp

all: clean alexnet

alexnet:
	$(CXX) $(CFLAGS) $(LDFLAGS) $(src) -o alexnet

.PHONY: clean

clean:
	rm -f alexnet

.PHONY: run

run:
	./alexnet tiny-model-alexnet classes.txt cat.jpg

.PHONY: conv_sample

conv_sample:
	rm -f conv_sample
	gcc -Wall conv_sample.c -o conv_sample -lm

.PHONY: fc_sample

fc_sample:
	rm -f fc_sample
	gcc -Wall fc_sample.c -o fc_sample -lm

.PHONY: net

net:
	rm -f fc_sample
	gcc -Wall net.c -o net -lm


# NOTE
#
# Linha para gerar graficos do massif:
# $ valgrind --tool=massif --pages-as-heap=yes --heap=yes ./fc_sample
#
# Linha para gerar .h include files de parametros da rede
# $ ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# $ ./write_array.py 121 3 96 weight_1 transfer_files/FILTER_DATA.dat param_headers/1_weight.h
# $ ./write_array.py 1 1 96 bias_1 transfer_files/BIAS_DATA.dat param_headers/1_bias.h
