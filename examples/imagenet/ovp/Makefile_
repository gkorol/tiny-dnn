
#mini makefile pra facilitar...

CXX = g++
#CXX = arm-linux-gnueabi-gcc

#CFLAGS = -std=gnu++11 -g -O3 -ftree-vectorize -funsafe-math-optimizations -DDNN_USE_IMAGE_API -I../../
#CFLAGS = -O0 -march=armv8-a -ftree-vectorize -I../../
#CFLAGS = -O0 -mcpu=cortex-a53

#LDFLAGS = -lm
LDFLAGS = -lm -static

src = test.cpp

all: clean alexnet

alexnet:
	$(CXX) $(CFLAGS) $(LDFLAGS) $(src) -o alexnet

.PHONY: clean

clean:
	rm -f alexnet

.PHONY: run

run:
	./alexnet tiny-model-alexnet classes.txt imagens/slippers.jpg

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
	rm -f net
	gcc -O0 net_ovp.c fc_6.c fc_7.c fc_8.c -o net -lm
	#$(CXX) $(CFLAGS) net_ovp.c fc_6.c fc_7.c fc_8.c -o net $(LDFLAGS)
	#gcc -Wall net.c -o net -lm
	#gcc net.c fc_6.c fc_7.c fc_8.c -o net -lm
	# gcc net.c fc_6.c -o net -lm


# NOTE
#
# Linha para gerar graficos do massif:
# $ valgrind --tool=massif --pages-as-heap=yes --heap=yes ./fc_sample
#
# Linha para gerar .h include files de parametros da rede
# $ ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# $ ./write_array.py 121 3 96 weight_1 transfer_files/FILTER_DATA.dat param_headers/1_weight.h
# $ ./write_array.py 1 1 96 bias_1 transfer_files/BIAS_DATA.dat param_headers/1_bias.h
#
# Linha para gerar multiplos .h a partir de um arq texto com pesos
# $ ./write_weights_n_files.py 6 6 256 4096 weights_6 transfer_files/FILTER_DATA.dat param_headers/weights_6/weights_6
# $ ./write_weights_n_files.py 1 1 4096 4096 weights_7 transfer_files/FILTER_DATA.dat param_headers/weights_7/weights_7
# $ ./write_weights_n_files.py 1 1 4096 1000 weights_8 transfer_files/FILTER_DATA.dat param_headers/weights_8/weights_8
#
# Linha para gerar histograma a partir do header de pesos
# $ ./histogram.py 6 6 256 4096 param_headers/6_weight.h
# $ ./histogram.py 1 1 4096 4096 param_headers/7_weight.h
# $ ./histogram.py 1 1 4096 1000 param_headers/8_weight.h
