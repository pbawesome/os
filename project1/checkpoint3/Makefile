# Selim Tanriverdi and William Kelley
# CS 3013 C02 2016
CC=gcc
# CFLAGS= -g -ansi -std=c99 -lm

all: shell2.o
	$(CC) $(CFLAGS) -o shell2 shell2.o

shell2.o: shell2.c
	$(CC) $(CFLAGS) -c shell2.c

# commandInfo.o: commandInfo.c commandInfo.h
# 	$(CC) $(CFLAGS) -c commandInfo.c

# bgProc.o: bgProc.c bgProc.h]
# 	$(CC) $(CFLAGS) -c bgProc.c

clean:
	rm -v *.o