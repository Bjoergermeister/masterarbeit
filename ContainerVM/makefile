common = ../common/C
libs = $(common)/timing.o $(common)/benchmark.o $(common)/network.o

timing:
	gcc -c $(common)/timing.c -o $(common)/timing.o

benchmark:
	gcc -c $(common)/benchmark.c -o $(common)/benchmark.o

network:
	gcc -c $(common)/network.c -o $(common)/network.o

build: timing benchmark network
	gcc GameOfLife/main.c $(libs) -o GameOfLife/gameoflife
	gcc qsort/main.c $(libs) -o qsort/qsort
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=ivybridge -fopenmp -D_FILE_OFFSET_BITS=64 -I/usr/include/apr-1.0 $(libs) binary-trees/binary-trees.c -o binary-trees/binary-trees -lapr-1 -lgomp -lm
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=ivybridge n-body/nbody.c $(libs) -o n-body/nbody -lm
