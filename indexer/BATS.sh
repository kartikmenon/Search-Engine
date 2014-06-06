#!/bin/bash
# Test file for Indexer.

logFile="./LogIndexer.$(date +"%a_%b_%d_%T_%Y")"

printf "Starting test of indexer\n\n" > "$logFile"
printf "Start date: `date`" >> "$logFile"
printf "\n" >> "$logFile"

make clean
make

printf "Indexer usage:\n" >> "$logFile"
printf "indexer ./directory index.dat\n" >> "$logFile"
printf "indexer ./directory index.dat index.dat indexOut.dat\n\n" >> "$logFile"

# Test for bad arguments.
# Bad number of inputs (= not 3 or 5)

printf "1) Bad number of arguments\n" >> "$logFile"
printf "Command: indexer hello have fourArgs\n" >> "$logFile"
printf "[indexer output]: " >> "$logFile"
indexer hello have fourArgs 2>> "$logFile"

printf "\n\n" >> "$logFile"

# Test for a bum directory
# First, unreadable.
mkdir unreadable
chmod -r unreadable
printf "2) Unreadable directory error\n" >> "$logFile"
printf "Command: indexer ./unreadable index.dat\n" >> "$logFile"
printf "[indexer output]: " >> "$logFile"
indexer ./unreadable index.dat 2>> "$logFile"
rmdir unreadable


printf "\n\n" >> "$logFile"

printf "3) Nonexistent directory error\n" >> "$logFile"
printf "Command: indexer ./nosuchthing index.dat\n" >> "$logFile"
printf "[indexer output]: " >> "$logFile"
indexer ./nosuchthing index.dat 2>> "$logFile"

printf "\n\n" >> "$logFile"

mkdir badFileNames
cd badFileNames; touch abc
cd ..

printf "4) Bad file name within directory\n" >> "$logFile"
printf "Command: indexer ./badFileNames index.dat\n" >> "$logFile"
printf "[indexer output]: " >> "$logFile"
indexer ./badFileNames index.dat 2>> "$logFile"

printf "\n\n" >> "$logFile"

cd badFileNames; rm -f abc
cd ..
rmdir badFileNames

# Make a file in the directory badFormat with only 2 lines (HTML supposed to be on third line)
mkdir badFormat
cd badFormat; touch 1; printf "hello line 1, URL\n" > 1; printf "hello line 2, Depth\n" >> 1; cd ..
printf "5) Bad file format in directory\n" >> "$logFile"
printf "Command: indexer ./badFormat index.dat\n" >> "$logFile"
printf "[indexer output]: " >> "$logFile"
indexer ./badFormat index.dat 2>> "$logFile"
cd badFormat; rm -f 1; cd ..
rmdir badFormat

printf "\n\n" >> "$logFile"

printf "Now testing indexer functionality.\n\n" >> "$logFile"

printf "Indexer regular mode: indexer ~cs50/tse/crawler/lvl0 lvl0index.dat\n\n" >> "$logFile"

indexer ~cs50/tse/crawler/lvl0 lvl0index.dat >> "$logFile"

printf "\n\n" >> "$logFile"

printf "A small sample of the produced indexlvl0.dat file:\n" >> "$logFile"

(head -5 lvl0index.dat) >> "$logFile"
printf "\n" >> "$logFile"

printf "Indexer testing mode: indexer ~cs50/tse/crawler/lvl1 lvl1index.dat indexlvl1.dat indexOut.dat\n\n" >> "$logFile"

indexer ~cs50/tse/crawler/lvl1 lvl1index.dat lvl1index.dat indexOut.dat >> "$logFile"
printf "\n\n" >> "$logFile"

printf "Running a UNIX 'diff' command on indexlvl1.dat and indexOut.dat\n" >> "$logFile"

printf "[diff -u lvl1index.dat indexOut.dat] Output: " >> "$logFile"
(diff -u lvl1index.dat indexOut.dat) >> "$logFile"

printf "\n" >> "$logFile"
printf "If output nonexistent, files identical.\n\n" >> "$logFile"

printf "Testing valgrind on the indexer.\n" >> "$logFile"

printf "command: valgrind indexer ~cs50/tse/crawler/lvl1 valTest.dat valTest.dat valTestOut.dat\n\n" >> "$logFile"

(valgrind indexer ~cs50/tse/crawler/lvl0 valTest.dat valTest.dat valTestOut.dat) &>> "$logFile"



printf "\n\n" >> "$logFile"

printf "Making index file for submission... Run on ~cs50/tse/crawler/lvl3.\n" >> "$logFile"
indexer ~cs50/tse/crawler/lvl3 submission.dat

printf "End date: `date`" >> "$logFile"
printf "\n" >> "$logFile"

printf "End indexer testing.\n" >> "$logFile"


make clean