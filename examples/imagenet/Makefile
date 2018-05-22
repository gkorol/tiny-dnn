#mini makefile pra facilitar...

CXX = g++
# CXX = arm-linux-gnueabi-g++

CFLAGS = -std=gnu++11 -g -O1 -ftree-vectorize -funsafe-math-optimizations -DDNN_USE_IMAGE_API -I../../
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

#valgrind --tool=massif --pages-as-heap=yes --heap=yes ./fc_sample
