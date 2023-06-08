common = ../../../common/C
libs = $(common)/timing.o $(common)/benchmark.o $(common)/network.o

timing:
	gcc -c $(common)/timing.c -o $(common)/timing.o

benchmark:
	gcc -c $(common)/benchmark.c -o $(common)/benchmark.o

network:
	gcc -c $(common)/network.c -o $(common)/network.o

build: timing benchmark network
	gcc latency.c $(libs) -o latency