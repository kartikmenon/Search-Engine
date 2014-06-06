README
Kartik Menon
Lab 4: Crawler

Submission is built using make on a “Makefile” file in the lab4 directory. This compiles my crawler.c source code (located in lab4/src/) and the helper sources list.c, web.c, and hashtable.c (located in lab4/util/). They are compiled together with the -lcurl option for the curl library. The compiler used is the standard gcc -Wall -pedantic -std=c11.

Make also has two other commands, “debug,” which compiles with -g, and “clean” for removing all of the files that make creates once the program has finished running.

The program assumes it will be given a valid seed url (argument #1), a writeable and already existing directory with a proper address (argument #2) and a depth between 0 and 4 (argument #3). There is code to test if each of these arguments is not valid.

This program is tested and run by crawlerTest.sh, which appends to a log file, called crawlerTestLog.(current_date_and_time) errors that I purposely cause (for testing purposes). The crawlerTest.sh will call crawler on http://old-www.cs.dartmouth.edu/~cs50/tse/wiki/Computer_science.html on various depths, some of which are commented out, into ./testDir. 

This crawler does not find websites without proper HTML (like .pdf, .png extensions). 