# Makefile for libtseutil

CC=gcc
CFLAGS= -Wall -pedantic -std=c11
LIBC=./wordweb.c ./genlist.c ./genHashtable.c ./file.c
LIBOBJS=$(LIBC:.c=.o)

all:libtseutil.a

libtseutil.a: $(LIBOBJS)
	rm -f $@
	ar -cvq $@ $(LIBOBJS)

clean:
	rm -f *.o
	rm -f *.a