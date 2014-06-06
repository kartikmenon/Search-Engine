/* ========================================================================== */
/* File: web.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * Author: Ira Ray Jenkins - jenkins@cs.dartmouth.edu
 * Date: Fri Apr 25, 2014
 *
 * These functions are provided for the students to simplify the html parsing
 * and web retrieval.
 */
/* ========================================================================== */
#define _GNU_SOURCE                          // strncasecmp, strdup
                                             // students shouldn't take advantage
                                             // of the gnu extensions, but
                                             // parsing html without is a pain

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdlib.h>                          // free, calloc, realloc
#include <ctype.h>                           // tolower, isalpha
#include <string.h>                          // strchr, strncpy

// ---------------- Local includes  e.g., "file.h"
#include "web.h"                             // web functionality

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

// ---------------- Public functions

/*
 * GetNextWord - returns the next word from doc[pos] into word
 *
 * Assumptions:
 *     1. doc has been allocated
 *     2. don't care about opening/closing tags: ignore anything between <...>
 *     3. if the html is malformed, we don't care: match '<' with next '>'
 *
 * Pseudocode:
 *     1. skip any leading non-alphabetic characters
 *     2. if we find a tag, i.e., <...tag...>, skip that tag
 *     3. save beginning of the word
 *     4. find the end, i.e., first non-alphabetic character
 *     5. create a new word buffer
 *     6. copy the word into the new buffer
 *     7. return first position past end of word
 */
int GetNextWord(const char* doc, int pos, char **word)
{
    const char *beg;                         // beginning of word
    const char *end;                         // end of word

    // make sure we have something to search
    if(!doc) {
        return -1;
    }

    // consume any non-alphabetic characters
    while(doc[pos] && !isalpha(doc[pos])) {
        // if we find a tag, i.e., <...tag...>, skip it
        if(doc[pos] == '<') {
            end = strchr(&doc[pos], '>');    // find the close

            if(!end || !(++end)) {           // ran out of html
                return -1;
            }

            pos = end - doc;                 // skip forward

            continue;                        // keep going
        }

        pos++;                               // just move forward
    }

    // ran out of html
    if (!doc[pos])
        return -1;

    // we're at the beginning of a word
    beg = &(doc[pos]);

    // consume word
    while (doc[pos] && isalpha(doc[pos])) {
       pos++;
    }

    // if we go to far, back up one
    if(!doc[pos]) {
        pos--;
    }

    // we're at the end of a word
    end = &(doc[pos]);

    // allocate new word + '\0'
    *word = calloc(end - beg + 1, sizeof(char));
    if(!word) {                              // ruh roh raggy!
        return -1;
    }

    // copy the new word
    strncpy(*word, beg, end - beg);

    return pos;
}

/*
 * NormalizeWord - lowercases all the alphabetic characters in word
 *
 *
 * Pseudocode:
 *     1. scan each letter of word, try to lowercase it
 */
void NormalizeWord(char *word)
{
    for(char *ptr = word; *ptr; ++ptr)
        *ptr = tolower(*ptr);
}
