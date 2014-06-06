#!/bin/bash
# Test file for crawler.


logFile="./crawlerTestLog.$(date +"%a_%b_%d_%T_%Y")"



printf "Starting test of crawler.c\n\n" > "$logFile"

make

# Test for bad arguments.
# 1) Bad seed URL. Should output something about not reaching seed.
printf "1) Bad Seed URL test\n" >> "$logFile"
printf "crawler http://old-www.cs.dartmouth.edu/~cs50/tse/somegarbage ./testDir 1\n" >> "$logFile"
printf "[crawler Output]: " >> "$logFile"
crawler http://old-www.cs.dartmouth.edu/~cs50/tse/somegarbage ./testDir 1 2>> "$logFile"

printf "\n" >> "$logFile"

printf "crawler http://www.google.com/ ./testDir 1\n" >> "$logFile"
printf "[crawler Output]: " >> "$logFile"
crawler http://www.google.com/ ./testDir 1 2>> "$logFile"

printf "\n\n" >> "$logFile"

# 2) User inputs various bad numbers of arguments

printf "2) Bad Number of arguments\n" >> "$logFile"
printf "crawler\n" >> "$logFile"
printf "[crawler Output]: " >> "$logFile"
crawler 2>> "$logFile"

printf "\n" >> "$logFile"
printf "crawler http://old-www.cs.dartmouth.edu/~cs50/tse/\n" >> "$logFile"
printf "[crawler Output]: " >> "$logFile"
crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ 2>> "$logFile"
printf "\n" >> "$logFile"


printf "crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./testDir hello whatup more than 4 arguments\n" >> "$logFile"
printf "[crawler Output]: " >> "$logFile"
crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./testDir hello whatup more than 4 arguments 2>> "$logFile"

printf "\n\n" >> "$logFile"

printf "3) Case depth invalid\n" >> "$logFile"
printf "crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./testDir -2\n" >> "$logFile"
printf "[crawler Output]: " >> "$logFile"
crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./testDir -2 2>> "$logFile"
printf "\n" >> "$logFile"

printf "crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./testDir 5\n" >> "$logFile"
printf "[crawler Output]: " >> "$logFile"
crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./testDir 5 2>> "$logFile"
printf "\n\n" >> "$logFile"


printf "4) Target directory doesn't exist or is not-writeable\n" >> "$logFile"
printf "crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./fakeDir 1\n" >> "$logFile"
printf "[crawler Output]: " >> "$logFile"
crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./fakeDir 1 2>> "$logFile"
printf "\n" >> "$logFile"


printf "crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./unwriteable 1\n" >> "$logFile"
printf "[crawler Output]: " >> "$logFile"
crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./unwriteable 1 2>> "$logFile"
printf "\n\n" >> "$logFile"


printf "The first call to crawler was this: <crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./testDir 2>.\n It's output, as judged by <ls | xargs -n1 head -n1 | sort | wc -l> is:\n" >> "$logFile"


# Commented out to not cause issues when you guys are running this for grading

crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./testDir 2
#valgrind --show-leak-kinds=all --leak-check=full -v crawler http://old-www.cs.dartmouth.edu/~cs50/tse/wiki/Computer_science.html ./valgrindTest 1
crawler http://old-www.cs.dartmouth.edu/~cs50/tse/wiki/Computer_science.html ./valgrindTest 3


(cd ./testDir/; ls | xargs -n1 head -n1 | sort | wc -l) >> "$logFile"
printf "\n" >> "$logFile"


printf "The second call to crawler was this: <crawler http://old-www.cs.dartmouth.edu/~cs50/tse/wiki/Computer_science.html ./valgrindTest 3>.\n It's output, as judged by <ls | xargs -n1 head -n1 | sort | wc -l> is:\n" >> "$logFile"
(cd ./valgrindTest/;ls | xargs -n1 head -n1 | sort | wc -l) >> "$logFile"
printf "\n" >> "$logFile"

printf "End of crawler.c testing. Cleaning up .o files and crawler executable.\n" >> "$logFile"

make clean