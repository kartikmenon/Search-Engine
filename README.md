Search Engine:

Components:

Crawler - Crawls the requested URL at a requested depth (number of links to follow through) and saves the crawled data files in a requested directory.

Indexer - Looks over the crawled data files and outputs a .dat file that lists all words and their instances in each of the crawled data files.

Query Engine - Looks in the .dat file supplied by indexer and according to command-line user search query input, returns a list URLs the search query is found in, ranked by frequency. Supports AND and OR operators, so cat AND dog will return only the pages that feature both words while cat OR dog will return the union of both sets.


Note that code will not compile because of instructor request to remove some supplied files.
