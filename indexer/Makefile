CC = gcc
CFLAGS = -Wall -pedantic -std=c11

# Project details
EXEC = indexer
OBJS = indexer.o web.o genHashtable.o file.o
CODEDIR = ./src/
SRCS = $(CODEDIR)web.c $(CODEDIR)genHashtable.c $(CODEDIR)file.c $(CODEDIR)indexer.c

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS) 

debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS) 
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)

clean:
	rm -f *~
	rm -f ./indexer
	rm -f *o 
	rm -f *x.dat
	rm -f *t.dat
