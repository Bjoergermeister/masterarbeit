common = ../../../common/C
tools = ../../../tools
libs = $(common)/timing.o $(common)/benchmark.o

tools:
	gcc $(tools)/containerize.c -o containerize

timing:
	gcc -c $(common)/timing.c -o $(common)/timing.o

benchmark:
	gcc -c $(common)/benchmark.c -o $(common)/benchmark.o

build: timing benchmark
	gcc write.c $(libs) -o write
