common = ../../../common/C
libs = $(common)/timing.o $(common)/benchmark.o

timing:
	gcc -c $(common)/timing.c -o $(common)/timing.o

benchmark:
	gcc -c $(common)/benchmark.c -o $(common)/benchmark.o

build: timing benchmark
	gcc allocation.c $(libs) -o allocation