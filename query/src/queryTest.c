/* ========================================================================== */
/* File: queryTest.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Query Engine
 *
 * Author: Kartik Menon
 * Date: May 2014
 *
 * Do unit testing on the functions used in the query engine.
 * Essentially making artificial circumstances in which the functions can
 * operate. Test the expected result. If main returns 0, testing failed.
 * If main returns 1, all tests were passed.
 */
/* ========================================================================== */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "../../util/genHashtable.h"
#include "../../util/genlist.h"
#include "../../util/file.h"
#include "../../util/wordweb.h"
#include "../../util/header.h"
#include "./reload.h"
#include "./query.h"


// Tests whether the query string was properly loaded into linked list
int testRetrieveQuery(void) {
    int returnValue = 1;

    char *p = "this is query OR buffer";
    char *input = calloc(strlen(p) + 1, sizeof(char));
    int wordCount = countWords(p);
    strncpy(input, p, strlen(p) + 1);
    
    List *list;
    list = retrieveQuery(wordCount, input);
    
    if (strcmp(list->head->data[0], "this")) {
        returnValue = 0;
    }
    if (strcmp(list->head->data[1], "is")) {
        returnValue = 0;
    }
    if (strcmp(list->head->data[2], "query")) {
        returnValue = 0;
    }
    
    if (strcmp(list->head->next->data[0], "buffer")) {
        returnValue = 0;
    }
    
    return returnValue;

}

// tests if query was properly looked up in hash table
int testIndexLookUp(void) {
    int returnValue = 1;
    
    char *p = "computer";
    char *input = calloc(strlen(p) + 1, sizeof(char));
    int wordCount = countWords(p);
    strncpy(input, p, strlen(p) + 1);
    
    List *list;
    list = retrieveQuery(wordCount, input);
    
    HashTable *test;
    test = initHash();
    DocumentNode *docTest1 = DNode(1, 2);
    DocumentNode *docTest2 = DNode(3, 4);
    WordNode *wordTest = WNode(docTest1, p);
    wordTest->page->next = docTest2;
    long key = JenkinsHash(p, MAX_HASH_SLOT);
    test->table[key]->hashKey = wordTest;
    
    DocumentNode **testList;
    testList = indexLookUp((char**)list->head->data, test);
    
    if (testList[0]->docID != 1) {
        returnValue = 0;
    }
    
    if (testList[0]->freq != 2) {
        returnValue = 0;
    }
    if (testList[1]->docID != 3) {
        returnValue = 0;
    }
    if (testList[1]->freq != 4) {
        returnValue = 0;
    }
    
    return returnValue;
    
    
}

// tests if document nodes were properly ranked.
int testRank(void) {
    int returnValue = 1;
    
    DocumentNode **list = (DocumentNode **)calloc(3, sizeof(DocumentNode));
    DocumentNode *copy1 = DNode(1, 10);
    DocumentNode *copy2 = DNode(4, 5);
    DocumentNode *copy3 = DNode(3, 7);
    
    list[0] = copy1;
    list[1] = copy2;
    list[2] = copy3;
    
    DocumentNode **ranked = NULL;
    ranked = rankQuery(list, 3);
    
    if (ranked[0]->freq != 10) {
        returnValue = 0;
    }
    
    if (ranked[1]->freq != 7) {
        returnValue = 0;
    }
    
    if (ranked[2]->freq != 5) {
        returnValue = 0;
    }
    
    return returnValue;
}

int testCombine(void) {
    int returnValue = 1;
    
    DocumentNode **combine = (DocumentNode **)calloc(3, sizeof(DocumentNode));
    
    DocumentNode **input = (DocumentNode **)calloc(2, sizeof(DocumentNode));
    
    DocumentNode *copy1 = DNode(1, 10);
    DocumentNode *copy2 = DNode(4, 5);
    DocumentNode *copy3 = DNode(3, 7);
    // copy 4 should not be added to list. duplicate doc ID.
    DocumentNode *copy4 = DNode(4, 5);
    DocumentNode *copy5 = DNode(8, 2);

    
    combine[0] = copy1;
    combine[1] = copy2;
    combine[2] = copy3;
    
    input[0] = copy4;
    input[1] = copy5;
    
    combine = combineArrays(combine, input);
    
    if (combine[0]->docID != 1) {
        returnValue = 0;
    }
    
    if (combine[1]->docID != 4) {   // copy4 compounded here
        returnValue = 0;
    }
    if (combine[2]->docID != 3) {
        returnValue = 0;
    }
    if (combine[3]->docID != 8) {   // copy5
        returnValue = 0;
    }
    
    if (combine[4]) {       // should be null
        returnValue = 0;
    }
    

    return returnValue;
}

int main(void) {
    int value = 1;
    value = testRetrieveQuery();
    if (!value) {
        fprintf(stdout, " Retrieve failed return code %d", value);
        return value;
    }
    
    value = testIndexLookUp();
    if (!value) {
        fprintf(stdout, " Indexlook failed return code %d", value);
        return value;
    }
    
    value = testRank();
    if (!value) {
        fprintf(stdout, " Rank failed return code %d", value);
        return value;
    }
    
    value = testCombine();
    if (!value) {
        fprintf(stdout, " Combine failed return code %d", value);
        return value;
    }

    return value;
    
}

/* ==========================================================================
 * Get the list of words from the input
 *
 * *** Content ***
 * Go through the input query, delimit by space and get all of the words.
 * Store every word in a linked list (i.e., each linked list node has an
 * array of chars in it). If the word is OR, make a new node and put a new
 * array into that. If just delimited by " " or "AND", then add to the same
 * node
 * ========================================================================== */

List *retrieveQuery(int len, char *input) {
    char *tok = NULL;
    char *newLine = NULL;
    List* queryList;
    queryList = initList();
    ListNode *prev;
    
    
    
    
    // Dump out the newline character because it messes up the hash function
    newLine = strchr(input, '\n');
    if (newLine) {
        *newLine = 0;
    }
    
    
    
    tok = strtok(input, " ");
    int counter = 0;
    
    if (!tok) {
        if (queryList) {
            free(queryList);     // only initList() been called, that is one calloc for list
            queryList = NULL;   // change
        }
        
        return NULL;
    }
    
    // Normalize it. No uppercase characters
    for (int i = 0; tok[i]; i++) {
        tok[i] = tolower(tok[i]);
    }
    
    
    // need to have strtok as while condition, so start in advance
    
    ListNode *addition = calloc(1, sizeof(ListNode));
    MALLOC_CHECK(stderr, addition);
    
    addition->data = calloc(len + 1, sizeof(char*));
    MALLOC_CHECK(stderr, addition->data);
    
    addition->data[counter] = (char*) calloc(strlen(tok) + 1 + 1, sizeof(char));
    MALLOC_CHECK(stderr, addition->data[counter]);
    
    addition->data[counter + 1] = NULL;
    strncpy(addition->data[counter], tok, strlen(tok) + 1);
    append(queryList, addition);
    prev = queryList->head;
    
    while ((tok = strtok(NULL, " ")) != NULL) {
        if (!strncmp(tok, "AND", strlen("AND") + 1)) {
            continue;
        }
        
        if (!strncmp(tok, "OR", strlen("OR") + 1)) {
            ListNode *addition = calloc(1, sizeof(ListNode));
            MALLOC_CHECK(stderr, addition);
            
            prev->next = addition;
            addition->next = NULL;
            addition->data = calloc(len, sizeof(char*));
            MALLOC_CHECK(stderr, addition->data);
            
            prev = addition;
            append(queryList, addition);
            counter = -1;
            continue;
            
        }
        
        // Normalize it. No uppercase characters
        for (int i = 0; tok[i]; i++) {
            tok[i] = tolower(tok[i]);
        }
        
        counter++;
        prev->data[counter] = (char*)calloc(strlen(tok) + 1 + 1, sizeof(char));
        MALLOC_CHECK(stderr, prev->data[counter]);
        
        prev->data[counter + 1] = NULL;
        MALLOC_CHECK(stderr, prev->data[counter]);
        strncpy(prev->data[counter], tok, strlen(tok) + 1);
        
    }
    
    
    return queryList;
}


/* ==========================================================================
 * Counts number of document nodes in the linked list given a word node
 * ========================================================================== */
int countDocNodes(WordNode *wordNode) {
    int counter = 0;
    DocumentNode *page;
    for (page = wordNode->page; page != NULL; page = page->next) {
        counter++;
        if (!page->next) {
            break;
        }
    }
    return counter;
}

/* ==========================================================================
 * Counts number of strings stored in an array of strings
 * ========================================================================== */
int wordArraySize(char **list) {
    int counter = 0;
    while (list[counter]) {
        counter++;
        
    }
    return counter;
}

/* ==========================================================================
 * Copies a document node into a new one given a document node
 * ========================================================================== */
DocumentNode *copyDocNode(DocumentNode *node) {
    DocumentNode *new = DNode(node->docID, node->freq);
    return new;
}

/* ==========================================================================
 * Free an array of document nodes
 * ========================================================================== */
void freeDocNodeList(DocumentNode **list) {
    int counter = 0;
    while (list[counter]) {
        /* if (list[counter]) {
         free(list[counter]);
         list[counter] = NULL; //change
         }*/
        free(list[counter]);
        list[counter] = NULL;
        counter ++;
    }
    free(list);
    list = NULL;
    
}

/* ==========================================================================
 * Finds the linked list of document nodes associated with a word in the
 * hash table index.
 *
 * *** Content ***
 * Most of the heavy lifting done here. Takes an array of characters, which
 * will be passed in from a linked list node. Finds the relevant linked list
 * of document nodes in the hash table and saves each of them in an array.
 * Then take every subsequent word in the list (i.e., words that are "AND"ed
 * together in the query) and see if any of the document IDs match each other.
 * If they match, copy the document node to a new array of document nodes,
 * which is returned.
 * ========================================================================== */
DocumentNode **indexLookUp(char **wordList, HashTable *index) {
    
    unsigned long key;
    GenHashTableNode *loopNode;
    int numDocNodes = 0;
    WordNode *wordNode = NULL;
    int returnSize = 0;
    
    int len = wordArraySize(wordList);
    
    // Get an array filled with the document nodes associated with the first word in the
    // word list.
    key = JenkinsHash(wordList[0], MAX_HASH_SLOT);
    loopNode = index->table[key];
    
    if (!loopNode) {
        return NULL;
    }
    for (; loopNode->hashKey != NULL; loopNode = loopNode->next) {
        if (!strncmp(((WordNode*)loopNode->hashKey)->word, wordList[0], strlen(wordList[0]) + 1)) {
            wordNode = loopNode->hashKey;
            break;
        }
        if (!loopNode->next) {      // don't go further
            break;
        }
    }
    
    if (!wordNode) {
        return NULL;
    }
    
    DocumentNode *firstDoc;
    numDocNodes = countDocNodes(wordNode);
    DocumentNode **docNodeList = (DocumentNode **)calloc(numDocNodes + 1, sizeof(DocumentNode));
    docNodeList[numDocNodes] = NULL;
    MALLOC_CHECK(stderr, docNodeList);
    
    int counter = 0;
    for (firstDoc = wordNode->page; firstDoc != NULL; firstDoc = firstDoc->next) {
        DocumentNode *copy = copyDocNode(firstDoc);
        docNodeList[counter] = copy;
        docNodeList[counter]->count = 1;
        counter++;
        if (!firstDoc->next) {
            break;
        }
    }
    
    if (1 == len) {
        return docNodeList;
    }
    
    // Now have an array of docnodes corresponding to first query word.
    
    // Loop over all other words in word list, comparing their doc nodes to the doc nodes
    // in the docNodeList. If the doc node is found, increment the count. If not found,
    // do nothing.
    
    
    for (int i = 1; i < len; i++) {
        wordNode = NULL;
        key = JenkinsHash(wordList[i], MAX_HASH_SLOT);
        loopNode = index->table[key];
        
        if (!loopNode) {
            freeDocNodeList(docNodeList);
            return NULL;
        }
        
        // try to find the wordlist's word in the hash table.
        for (; loopNode->hashKey != NULL; loopNode = loopNode->next) {
            
            if (!strncmp(((WordNode*)loopNode->hashKey)->word, wordList[i], strlen(wordList[i]) + 1)) {
                wordNode = loopNode->hashKey;
                break;
            }
            if (!loopNode->next) {      // don't go further
                break;
            }
        }
        if (!wordNode) {
            freeDocNodeList(docNodeList);
            return NULL;        // nothing found
        }
        
        DocumentNode *additionals;
        additionals = wordNode->page;
        
        
        for (; additionals != NULL; additionals = additionals->next) {
            for (int j = 0; j < numDocNodes; j++) {
                // if the doc nodes of the other words in the wordlist match any of the nodes
                // in the docNodeList, increment that count.
                if (docNodeList[j]->docID == additionals->docID) {
                    docNodeList[j]->freq += additionals->freq;
                    docNodeList[j]->count++;
                    returnSize++;
                }
                
                // otherwise, since it's an "AND" don't care about the word
                else {
                    continue;
                }
            }
        }
    }
    
    if (returnSize == 0) {
        freeDocNodeList(docNodeList);
        return NULL;    // No common documents found between words
    }
    
    DocumentNode **toReturn = (DocumentNode **)calloc(returnSize + 1, sizeof(DocumentNode));
    toReturn[returnSize] = NULL;
    // probably over-allocating
    MALLOC_CHECK(stderr, toReturn);
    
    int returnCount = 0;
    // Only get the list of document nodes with common query words
    for (int k = 0; k < numDocNodes; k++) {
        if (docNodeList[k]->count == len) {
            DocumentNode *copyReturn = copyDocNode(docNodeList[k]);
            toReturn[returnCount] = copyReturn;
            toReturn[returnCount]->count = docNodeList[k]->count;
            returnCount++;
        }
        else {
            continue;
        }
    }
    
    freeDocNodeList(docNodeList);
    return toReturn;
    
}


/* ==========================================================================
 * Sort the items in a linked list of Document Nodes
 *
 * *** Content ***
 * Loop through the document nodes of the supplied linked list (which will
 * contain the document nodes relevant to the query) and if one of them
 * has higher frequency than seen before, update the ranked linked list.
 * Set the frequency of the document node in the parameter linked list to -1
 * so it isn't looked at in the next iteration. This destroys the frequency
 * data (all nodes will have freq = -1), but the frequency isn't needed for
 * the output, just the order.
 * ========================================================================== */
DocumentNode **rankQuery(DocumentNode **linkedList, int n) {
    // n = number of document nodes in the linked list.
    DocumentNode **ranked = (DocumentNode **)calloc(n + 1, sizeof(DocumentNode));
    ranked[n] = NULL;
    MALLOC_CHECK(stderr, ranked);
    
    int maxfreq;
    int maxDoc;
    maxfreq = 0;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (maxfreq < linkedList[j]->freq) {
                DocumentNode *copy = DNode(linkedList[j]->docID, linkedList[j]->freq);
                ranked[i] = copy;
                maxDoc = linkedList[j]->docID;
                maxfreq = linkedList[j]->freq;
            }
        }
        
        for (int k = 0; k < n; k++) {
            if (linkedList[k]->docID == maxDoc) {
                linkedList[k]->freq = -1;
            }
        }
        maxfreq = 0;
        maxDoc = 1;
    }
    
    return ranked;
}



/* ==========================================================================
 * Function that returns the character count in a line
 * Secondary functionality: moves the position in the file stream a line down
 *
 * *** Content ***
 * Loops forever until line break is found, incremementing character count
 * Returns number of characters, fgetc(FILE stream) moves up buffer position
 * ========================================================================== */
int charCount(FILE *fp) {
    int c, count;
    count = 0;
    for ( ;; ) {
        c = fgetc(fp);
        if (c == EOF) {
            fprintf(stderr, "The supplied HTML file doesn't contain HTML.\n");
            exit(1);
        }
        if (c == '\n') break;
        ++count;
    }
    return count;
}

/* ==========================================================================
 * Function to return a URL based on a document node's docID
 *
 * *** Content ***
 * Open the relevant file and allocate space for a URL, return it.
 * ========================================================================== */
char *findQueryURL(DocumentNode* docNode, char *crawledDir) {
    // Did this in crawler; number of websites on internet = 10^9, so safely
    // use 10 characters to store the document ID of the crawled data
    char buffer[10];
    char *filePath;
    char *URL;
    int URLlen;
    FILE *fp;
    
    sprintf(buffer, "%d", docNode->docID);
    filePath = makeFilePath(crawledDir, buffer);
    
    fp = fopen(filePath, "r");
    if (!fp) {
        fprintf(stderr, "Unable to open crawled data, file %s\n.", filePath);
        return NULL;
    }
    
    // Dynamically allocate space for URL based on length of first line of crawled data
    URLlen = charCount(fp);
    rewind(fp);
    URL = (char*)calloc(URLlen + 1, sizeof(char));  // null-terminate
    MALLOC_CHECK(stderr, URL);
    
    fgets(URL, URLlen + 1, fp);
    free(filePath);
    fclose(fp);
    
    return URL;
    
}

/* ==========================================================================
 * Counts number of words in a buffer delimited by spaces
 *
 * *** Content ***
 * Copies string so strtok doesn't destroy input string.
 * ========================================================================== */
int countWords(char* buffer) {
    char copy[strlen(buffer) + 1];
    memset(copy, 0, strlen(buffer) + 1);
    strncpy(copy, buffer, strlen(buffer) + 1);
    
    char *c = strtok(copy, " ");
    int counter = 1;
    
    while ((c = strtok(NULL, " ")) != NULL) {
        if (!strncmp(c, "AND", strlen("AND") + 1)) {
            continue;
        }
        
        if (!strncmp(c, "OR", strlen("OR") + 1)) {
            continue;
        }
        counter++;
    }
    return counter;
}

/* ==========================================================================
 * Counts number of elements in an array, specifically of DocumentNodes
 * ========================================================================== */
int findDocListSize(DocumentNode **list) {
    int counter = 0;
    if (!list) {
        return counter;
    }
    
    
    while (list[counter]) {
        counter++;
    }
    
    return counter;
}

/* ==========================================================================
 * Combines two arrays of Document Nodes
 *
 * *** Content ***
 * Take two arrays, and if document nodes of the second are already in the
 * first, update the first's frequency (for ranking purposes) and if not,
 * add the second's document node onto the end of the first array.
 * ========================================================================== */
DocumentNode **combineArrays(DocumentNode **combined, DocumentNode **input) {
    
    int combinedLength = findDocListSize(combined);
    int inputLength = findDocListSize(input);
    
    DocumentNode **temp = NULL;
    
    // Basically if the first OR
    if (!combined) {
        combined = (DocumentNode **)calloc(inputLength + 1, sizeof(DocumentNode));
        for (int g = 0; g < inputLength; g++) {
            DocumentNode *copy = DNode(input[g]->docID, input[g]->freq);
            
            combined[g] = copy;
        }
        combined[inputLength] = NULL;
        
        return combined;
    }
    
    for (int i = 0; i < combinedLength; i++) {
        for (int j = 0; j < inputLength; j++) {
            if (combined[i]->docID == input[j]->docID) { //setmax?
                combined[i]->freq += input[j]->freq;
                input[j]->count = -1;       // keep track of the ones already seen
            }
        }
    }
    
    int counter = 0;
    for (int k = 0; k < combinedLength; k++) {
        for (int h = 0; h < inputLength; h++) {
            if (input[h]->count == -1) {
                continue;
            }
            
            // Make more space in the combine array
            temp = (DocumentNode **)realloc(combined, sizeof(DocumentNode) * (combinedLength + 1 + counter));
            
            MALLOC_CHECK(stderr, temp);
            
            combined = temp;
            
            DocumentNode *copy = DNode(input[h]->docID, input[h]->freq);
            combined[combinedLength + counter] = copy;
            input[h]->count = -1;
            combined[combinedLength + counter + 1] = NULL;
            counter++;
            
        }
    }
    
    return combined;
    
    
}