CC = gcc
CFLAGS = -Wall -pedantic -std=c11

# Project details
EXEC = crawler
OBJS = crawler.o web.o list.o hashtable.o
UTILDIR = ./util/
CODEDIR = ./src/
SRCS = $(UTILDIR)web.c $(UTILDIR)list.c $(UTILDIR)hashtable.c $(CODEDIR)crawler.c

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) -lcurl

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS) -lcurl

debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS) -lcurl
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS) -lcurl 

clean:
	rm -f *~
	rm -f *#
	rm -f ./crawler
	rm -f *.o
	rm -f *.gch
	rm -f *.html.*

cleanmore:
	cd ./testDir/ && rm -f *
	cd ./valgrindTest/ && rm -f *
