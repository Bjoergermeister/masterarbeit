common = ../../../common/C
libs = $(common)/timing.o $(common)/benchmark.o $(common)/ipc.o

ipc: 
	gcc -c $(common)/ipc.c -o $(common)/ipc.o

timing:
	gcc -c $(common)/timing.c -o $(common)/timing.o

benchmark:
	gcc -c $(common)/benchmark.c -o $(common)/benchmark.o

build: timing benchmark ipc
	gcc allocation.c $(libs) -o allocation