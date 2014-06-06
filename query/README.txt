CS50 Tiny Search Engine: Lab 6 Query Engine

README.txt

Query engine takes the indexer data from the downloaded web page information files created by crawler and looks up and returns the user results of user-inputted queries.

The query engine executable is built with make, using the Makefile in the ./query directory. “make” produces the executable, “make debug” produces a version of the executable compiled with the flags -ggdb -g. “make clean” runs the command <rm -f> on all of the created executable files, *.o object files, and goes into the library directory and performs the same thing, allowing changes to the library to be recompiled if make clean is called. 

The data structures used in query engine are:

HashTable, containing a (void*) hashKey and a (GenHashTableNode *) next.
WordNode, containing a (char*) word and a (DocumentNode *) page.
DocumentNode, containing a (int) docID, (int) freq, (DocumentNode *) next.
List, containing a (void**) data and a (ListNode*) head and tail.
DocumentNode **, an array.

The usage of query engine is:
./queryEngine [CRAWLED DIR] [INDEXER DATA FILEPATH]

[CRAWLED DIR] contains all of the numerically named files returned by the crawler. 
[INDEXER DATA FILEPATH] is the path to the indexed data created by indexer.

Note: the query engine is terminated by inputting EOF (^D) as a query.

Information on making:
“make” in the lab6/query directory makes the query engine. All source files, including queryTest.c included in lab6/src/. 