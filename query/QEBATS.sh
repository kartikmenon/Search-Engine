#!/bin/bash
# Test file for Query Engine

logFile="./LogIndexer.$(date +"%a_%b_%d_%T_%Y")"
printf "Starting test of indexer\n\n" > "$logFile"
printf "Start date: `date`" >> "$logFile"
printf "\n" >> "$logFile"

make clean
make tests
printf "Running unit tests on query engine functions...\n" >> "$logFile"
# Make sure query engine passed all unit tests
./tests >> "$logFile"


if [ $? -eq 0 ]; then
    printf "Unit test of Query Engine failed. Exiting...\n" >> "$logFile"
    exit 1

else
    return_code=$?
    printf "All tests of Query Engine functions passed, return code $return_code. Proceeding to test drive.\n\n" >> "$logFile"

# Get rid of the "tests" executable and make the real engine
    make clean
    make
fi

printf "Query Engine usage:\n" >> "$logFile"
printf "./queryEngine .../indexer data .../crawled data dir\n\n" >> "$logFile"

# Bum arguments

# Bad number of inputs (= not 3)

printf "1) Bad number of arguments\n" >> "$logFile"
printf "Command: ./queryEngine hello have fourArgs\n" >> "$logFile"
printf "[engine output]: " >> "$logFile"
./queryEngine hello have fourArgs 2>> "$logFile"

printf "\n\n" >> "$logFile"

# Test for a bum indexer data file
# First, unreadable.
touch ./unreadable.dat
chmod -r unreadable.dat
printf "2) Unreadable indexer data check\n" >> "$logFile"
printf "Command: ./queryEngine ./unreadable.dat ~cs50/tse/crawler/lvl1\n" >> "$logFile"
printf "[engine output]: " >> "$logFile"
./queryEngine ./unreadable.dat ~cs50/tse/crawler/lvl1 2>> "$logFile"
rm -f unreadable.dat


printf "\n\n" >> "$logFile"

printf "3) Nonexistent indexer data check\n" >> "$logFile"
printf "Command: ./queryEngine ./nosuchthing.dat ~cs50/tse/crawler/lvl1\n" >> "$logFile"
printf "[engine output]: " >> "$logFile"
./queryEngine ./nosuchthing.dat ~cs50/tse/crawler/lvl1 2>> "$logFile"

printf "\n\n" >> "$logFile"


printf "\n\n" >> "$logFile"

printf "Now testing actual query engine functionality.\n\n" >> "$logFile"

#some variables
q1="idontexistinanyhtmlprobably"
q2="head"
q3="cat"
q4="dog"


printf "Commands given to query engine:\n1. $q4 $q2 $q3\n2. $q2 OR $q4\n3. $q1\n\n" >> "$logFile"

echo -e "$q4 $q2 $q3\n$q2 OR $q4\n$q1\0" | ./queryEngine ~cs50/tse/indexer/cs_lvl3.dat ~cs50/tse/crawler/lvl3/ &>> "$logFile"



printf "\n\nEnd date: `date`" >> "$logFile"
printf "\n" >> "$logFile"

printf "End indexer testing.\n" >> "$logFile"


make clean






