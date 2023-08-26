common = ../common/C
libs = $(common)/timing.o $(common)/benchmark.o $(common)/ipc.o

make:
	gcc -c $(common)/ipc.c -o $(common)/ipc.o
	gcc -c $(common)/benchmark.c -o $(common)/benchmark.o
	gcc receiver.c $(libs) -o receiver