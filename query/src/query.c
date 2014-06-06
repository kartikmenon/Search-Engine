/* ========================================================================== */
/* File: query.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Query Engine
 *
 * Author: Kartik Menon
 * Date: May 2014
 *
 * Input: Indexed file and crawled pages directory
 * Output: User interface queried results
 *
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

#define QUERY_LENGTH 1001

/* ==========================================================================
 * Check that the arguments are validated.
 *
 * *** Content ***
 * Looks for wrong number of arguments.
 * Looks a proper target directory.
 * Sees and warns user if index.dat already exists.
 * ========================================================================== */

void validateArgs(int argc, char* argv[]) {
    struct stat dir;
    
    if (argc != 3) {
        fprintf(stderr, "Error. Need arguments: indexer data file, crawled"
                " data directory, query.\n");
        exit(1);
    }
    
    if (stat(argv[1], &dir)) {
        fprintf(stderr, "The path to the indexer data file not found.\n");
        exit(1);
    }
    
    if (stat(argv[2], &dir)) {
        fprintf(stderr, "The crawled data directory not found.\n");
        exit(1);
    }
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
            free(queryList);     // only initList() that is one calloc for list
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
    
    // Get an array filled with the document nodes associated with the
    // first word in the word list.
    key = JenkinsHash(wordList[0], MAX_HASH_SLOT);
    loopNode = index->table[key];
    
    if (!loopNode) {
        return NULL;
    }
    for (; loopNode->hashKey != NULL; loopNode = loopNode->next) {
        if (!strncmp(((WordNode*)loopNode->hashKey)->word, wordList[0],
                     strlen(wordList[0]) + 1)) {
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
    DocumentNode **docNodeList = (DocumentNode **)calloc(numDocNodes + 1,
                                                         sizeof(DocumentNode));
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
            
            if (!strncmp(((WordNode*)loopNode->hashKey)->word,
                         wordList[i], strlen(wordList[i]) + 1)) {
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
    
    DocumentNode **toReturn = (DocumentNode **)calloc(returnSize + 1,
                                                      sizeof(DocumentNode));
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
    
    if (ranked == NULL) {
        //free(ranked);
        
        return NULL;
    }
    
    ranked[n] = NULL;
    
    int maxfreq;
    int maxDoc;
    maxfreq = 0;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (maxfreq < linkedList[j]->freq) {
                DocumentNode *copy = DNode(linkedList[j]->docID,
                                           linkedList[j]->freq);
                
                if (copy == NULL) {
                    free(ranked);
                    ranked = NULL;
                    //free(copy);
                    return NULL;
                }
                
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
    
    // allocate space for URL based on length of first line of crawled data
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
        
        if (combined == NULL) { // this is new
            free(combined);
            return NULL;
        }
        
        for (int g = 0; g < inputLength; g++) {
            DocumentNode *copy = DNode(input[g]->docID, input[g]->freq);
            
            if (copy == NULL) {
                free(copy);
                return NULL;    // this is new.
            }
            
            combined[g] = copy;
        }
        combined[inputLength] = NULL;
        
        return combined;
    }
    
    for (int i = 0; i < combinedLength; i++) {
        for (int j = 0; j < inputLength; j++) {
            if (combined[i]->docID == input[j]->docID) { //setmax?
                combined[i]->freq += input[j]->freq;
                input[j]->count = -1;   // keep track of the ones already seen
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
            temp = (DocumentNode **)realloc(combined, sizeof(DocumentNode)
                                            * (combinedLength + 1 + counter));
            
            
            if (temp != NULL) { // protect from realloc failing
                combined = temp;
            }
            
            else {
                free(temp);
                temp = NULL;
                if (combined) {
                    freeDocNodeList(combined);
                }
                return NULL;
            }
            //MALLOC_CHECK(stderr, temp);
            
            //combined = temp;
            
            DocumentNode *copy = DNode(input[h]->docID, input[h]->freq);
            
            if (copy == NULL) {
                free(copy);
                return NULL;
            }
            combined[combinedLength + counter] = copy;
            input[h]->count = -1;
            combined[combinedLength + counter + 1] = NULL;
            counter++;
            
        }
    }
    
    return combined;
    
    
}

/* ==========================================================================
 * Frees a linked list of **void
 *
 * *** Content ***
 * - Go through each node in the linked list.
 * - In the array inside each node, free all of the calloc'd space for words 
 * - Once that's done, free the array, stored in ListNode->data
 * - Move on to the next node and free the one that was just being looked at.
 * - Free the list
 * ========================================================================== */
void freeWordList(List *list) {

    int counter = 0;
    
    while (!isEmpty(list)) {
        ListNode *temp;
        temp = list->head->next;
        counter = 0;
        while (list->head->data[counter]) {
            free(list->head->data[counter]);
            list->head->data[counter] = NULL;
            
            counter++;
        }
        
        free(list->head->data); // change
        list->head->data = NULL;    //change

        free(list->head);
        list->head = NULL;
        
        list->head = temp;
        if (list->head) {
            list->head->prev = NULL;
        }
    }
    
    if (list->head) {
        free(list->head);
    }
    
    list->head = NULL;
    free(list);
    list = NULL;

}



int main(int argc, char *argv[]) {
    validateArgs(argc, argv);
    int queryWordCount = 0;
    char queryBuffer[QUERY_LENGTH];// Assume total query is less than 1000 characters
    char *indexData = argv[1];
    char *crawlDir = argv[2];
    fprintf(stdout, "Starting up the query engine...\n");
    HashTable *reload;
    reload = ReadFile(indexData);
    List *list = NULL;
    char *URL;
    int docListSize = 0;
    DocumentNode **lookUpDocList = NULL;
    DocumentNode **allQueriedDocs = NULL;
    DocumentNode **ranked = NULL;
    int len = 0;
    int combineLen = 0;
    
    fprintf(stdout, "Ready\n");
    for ( ;; ) {
        fprintf(stdout, "QUERY:> ");
        
        char *test;
        test = fgets(queryBuffer, QUERY_LENGTH, stdin);
        
        // If user puts in EOF (^D), terminate query engine
        if (!test) {
            break;
        }
        
        // Count number of words in buffer to make wordList
        queryWordCount = countWords(queryBuffer);
        // Fragment the query buffer into words, store in wordList array
        list = retrieveQuery(queryWordCount, queryBuffer);
        
        if (!list) {
            // list already freed in retrieve query
            fprintf(stderr, "Input was not valid.\n");
            continue;
        }
        
        ListNode *loopNode = list->head;
        
        while (loopNode) {
            
            lookUpDocList = indexLookUp((char**)loopNode->data, reload);
            
            allQueriedDocs = combineArrays(allQueriedDocs, lookUpDocList);
            
            len = findDocListSize(lookUpDocList);
            
            // Warn user if one of their search terms wans't found, keep going
            if (!lookUpDocList || 0 == len) {
                fprintf(stderr, "No results matched one or more of your words.\n");
            }
            
            // This will be allocated again on next loop iteration so free.
            if (lookUpDocList) {
                freeDocNodeList(lookUpDocList);
                lookUpDocList = NULL;
            }
            
            loopNode = loopNode->next;
        }
        
        combineLen = findDocListSize(allQueriedDocs);
        
        // If there's nothing in allQueriedDocs, no search results to return
        if (!allQueriedDocs || 0 == combineLen) {
            fprintf(stderr, "No results matched your query.\n");
            if (list) {
                freeWordList(list);
            }
            freeDocNodeList(allQueriedDocs);
            allQueriedDocs = NULL;

            continue;
        }
        
        docListSize = findDocListSize(allQueriedDocs);
        
        ranked = rankQuery(allQueriedDocs, docListSize);
        
        if (!ranked) {      // a calloc failed in rank.
            if (list) {
                freeWordList(list);
            }
            freeDocNodeList(allQueriedDocs);
            allQueriedDocs = NULL;
            //freeDocNodeList(ranked);
            continue;
        }
        
        for (int i = 0; i < docListSize; i++) {
            URL = findQueryURL(ranked[i],crawlDir);
            fprintf(stdout,"DocumentID:%d URL:%s\n", ranked[i]->docID, URL);
            free(URL);
            URL = NULL;
        }
        
        freeDocNodeList(ranked);
        ranked = NULL;
        if (list) {
            freeWordList(list);
        }
        freeDocNodeList(allQueriedDocs);
        allQueriedDocs = NULL;

        
    }
    
    fprintf(stdout, "\nQuery engine terminated. Cleaning up...\n");
    freeIndexTest(reload);

    // If any of these things are still unfreed, free them here.
    if (URL) {
        free(URL);
    }
    
    if (allQueriedDocs) {
        freeDocNodeList(allQueriedDocs);
    }
    
    if (lookUpDocList) {
        freeDocNodeList(lookUpDocList);
    }
    
    if (ranked) {
        freeDocNodeList(ranked);    // change
    }
    
    return 1;
    
}