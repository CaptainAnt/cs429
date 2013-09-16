#
#   Makefile for region adjacency
#
#CFLAGS = -O3 -march=native -funroll-loops -pg -Wall
CFLAGS = -O0 -ggdb3

all: regioncalc

regioncalc: main.o
	gcc $(CFLAGS) main.o -o regioncalc -lm

#utils.o: utils.c utils.h
#	gcc $(CFLAGS) utils.c -c -lm

main.o: main.c utils.h
	gcc $(CFLAGS) main.c -c -lm

clean:
	rm -f *.o regioncalc

# COMMENTED OUT FOR REFERENCE

#all:   cachesim

#cachesim: cds.o main.o read_cds.o simulate.o utils.o
#	gcc $(CFLAGS)  cds.o main.o read_cds.o simulate.o utils.o -o cachesim

#cds.o: cds.c global.h cds.h
#	gcc $(CFLAGS) cds.c -c

#main.o: main.c global.h caches.h
#	gcc $(CFLAGS) main.c -c

#read_cds.o: read_cds.c global.h cds.h caches.h utils.h
#	gcc $(CFLAGS) read_cds.c -c

#simulate.o: simulate.c global.h cds.h caches.h utils.h
#	gcc $(CFLAGS) simulate.c -c

#utils.o: utils.c global.h utils.h
#	gcc $(CFLAGS) utils.c -c

#clean: 
#	rm -f *.o cachesim