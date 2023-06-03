common = ../../../common/C
libs = $(common)/timing.o $(common)/benchmark.o

timing:
	gcc -c $(common)/timing.c -o $(common)/timing.o

benchmark:
	gcc -c $(common)/benchmark.c -o $(common)/benchmark.o

build: timing benchmark
	gcc latency.c $(libs) -o latency

build-debug: timing benchmark
	gcc latency.c $(libs) -g -o latency