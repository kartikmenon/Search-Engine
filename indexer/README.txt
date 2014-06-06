CS50 Tiny Search Engine: Lab 5 Indexer

README.txt

Indexer takes the downloaded web page information files created by crawler, parses the curled HTML for words of letter length > 3, and reports the instances of the words in a data file index.dat.

The indexer executable is built with make, using the Makefile in the ./indexer directory. “make” produces the executable, “make debug” produces a version of the executable compiled with the flags -ggdb -g. “make clean” runs the command <rm -f> on all *.dat, ./indexer, *.o object files, cleaning the current index of everything but the ./src folder and the Makefile. ./src contains web.c, for parsing words in the curled HTML, file.c for finding filenames in directories among others, genHashTable.c, and indexer.c

The data structures used in indexer are:

HashTable, containing a (void*) hashKey and a (GenHashTableNode *) next.
WordNode, containing a (char*) word and a (DocumentNode *) page.
DocumentNode, containing a (int) docID, (int) freq, (DocumentNode *) next.

The usage of the indexer is:

./indexer  [TARGET DIRECTORY] [RESULTS FILENAME]
./indexer  [TARGET DIRECTORY] [RESULTS FILENAME] [RESULTS FILENAME] [REWRITTEN FILENAME]

[TARGET DIRECTORY] contains files with downloaded HTML.
[RESULTS FILENAME] is the name of the file the user supplies for the index data to be written. If the [RESULTS FILENAME] already exists at the time of call, indexer will warn that it will be overwritten and run. If it doesn’t exist indexer will create it, same with [REWRITTEN FILENAME].

NOTE:

The indexer will put all output files into the directory from which it was called. 

