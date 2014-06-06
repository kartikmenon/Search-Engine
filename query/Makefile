# Makefile for query engine; library libtseutil

CC=gcc
CFLAGS= -Wall -pedantic -std=c11
SRCS=./src/query.c ./src/reload.c ./src/reload.h

UTILDIR=../util/
UTILFLAG=-ltseutil
UTILC=$(UTILDIR)genHashtable.c $(UTILDIR)wordweb.c $(UTILDIR)genlist.c $(UTILDIR)file.c
UTILLIB=$(UTILDIR)libtseutil.a
UTILH=$(UTILC:.c=.h)

EXEC = queryEngine

$(EXEC):$(SRCS) $(UTILDIR)header.h $(UTILLIB)
	$(CC) $(CFLAGS) -o $@ $(SRCS) -L$(UTILDIR) $(UTILFLAG)

$(UTILLIB): $(UTILC) $(UTILH)
	cd $(UTILDIR); make;

debug: $(SRCS) $(UTILDIR)header.h $(UTILLIB)
	$(CC) $(CFLAGS) -g -ggdb -o $@ $(SRCS) -L$(UTILDIR) $(UTILFLAG)

tests: ./src/queryTest.c ./src/query.c $(UTILDIR)header.h $(UTILLIB)
	$(CC) $(CFLAGS) -g -ggdb -o $@ ./src/queryTest.c ./src/reload.c ./src/reload.h ./src/query.h -L$(UTILDIR) $(UTILFLAG)

clean:
	rm -f *~
	rm -f ./queryEngine
	rm -f *o
	cd $(UTILDIR); make clean;
	rm -f debug
	rm -f ./tests
	