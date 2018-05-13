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
