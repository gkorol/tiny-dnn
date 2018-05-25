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

conv_sample:
	rm -f conv_sample
	gcc -Wall conv_sample.c -o conv_sample -lm

fc_sample:
	rm -f fc_sample
	gcc -Wall fc_sample.c -o fc_sample -lm


# NOTE
#
# Linha para gerar graficos do massif:
# $ valgrind --tool=massif --pages-as-heap=yes --heap=yes ./fc_sample
#
# Linha para gerar .h include files de parametros da rede
# $ ./write_array.py 1 1 256 bias transfer_files/BIAS_CONV.dat param_headers/bias_256.h
# $ ./write_array.py 31 31 96 in transfer_files/IN_DATA_CONV.dat param_headers/in_92256.h
# $ ./write_array.py 25 96 256 weight transfer_files/FILTER_CONV.dat param_headers/weight_614400.h (1o arg é w.h pq pesos tem 4 dimensoes)
#
