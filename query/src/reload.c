
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "../../util/genHashtable.h"
#include "../../util/genlist.h"
#include "../../util/file.h"
#include "../../util/wordweb.h"
#include "../../util/header.h"


/* ==========================================================================
 * Makes a filepath out of a supplied directory and file name
 * ========================================================================== */

char *makeFilePath(char *directory, char *fileName) {
    int i = strlen(directory);
    int j = strlen(fileName);
    char *path = (char*)calloc(i + j + 1 + 1, sizeof(char));   // null terminator
    sprintf(path, "%s/%s", directory, fileName);
    return path;
}

/* ==========================================================================
 * Same thing as LoadDoc but without skipping the first two lines of file
 * ========================================================================== */
char *LoadDocIndex(char *fileName) {
    char *HTMLstring = NULL;
    long size = -1;
    
    if (!fileName) {
        fprintf(stderr, "Supplied argument(s) does not exist.\n");
        exit (1);
    }
    
    FILE *fp;
    fp = fopen(fileName, "r");
    if (!fp) {
        fprintf(stderr, "File %s failed to open in LoadDoc()\n", fileName);
        exit (1);
    }
    
    if (fseek(fp, 0, SEEK_END) == 0) { // fseek weird; returns 0 on success
        size = ftell(fp);
        if (size < 0) {                     // ftell returns -1 on failure
            fprintf(stderr, "Error: File size could not be determined.\n");
            exit (1);
        }
    }
    rewind(fp);                             // beginning of the buffer
    
    HTMLstring = (char*)calloc(1, sizeof(char) * (size + 1 + 1)); // '/0,
    
    size_t result = fread(HTMLstring, sizeof(char), size, fp); // copy to html buffer
    
    if (!result) {
        fprintf(stderr, "Error getting file text into HTMLstring buffer.\n");
        exit (1);
    }
    
    HTMLstring[result++] = '\0';     // need to null-terminate, fread doesn't
    
    fclose(fp);
    return HTMLstring;
}

/* ==========================================================================
 * Gets the document ID from the original unique (1, 2, ... n) from crawler
 *
 * *** Content ***
 * Just converts passed character filename to an integer.
 * ========================================================================== */
int GetDocID(char *fileName) {
    int ID = atoi(fileName);
    return ID;
}

/* ==========================================================================
 * Separate function to calloc new docnode and deal with all tests
 *
 * *** Content ***
 * Allocate a new document node in memory, checks its validity.
 * Set the new document node's next to NULL and its ID, freq to the parameters.
 * ========================================================================== */

DocumentNode *DNode(int docID, int freq) {
    DocumentNode *node = (DocumentNode *)calloc(1, sizeof(DocumentNode));
    MALLOC_CHECK(stderr, node);
    
    node->next = NULL;
    node->docID = docID;
    node->freq = freq;
    
    return node;
    
}

/* ==========================================================================
 * Separate function to calloc new wordnode and deal with all tests
 *
 * *** Content ***
 * Allocate a new word node in memory, checks its validity.
 * Set the new document node's next to NULL and its docNode, word to passed
 * parameters
 * ========================================================================== */

WordNode *WNode(DocumentNode *docNode, char *word) {
    WordNode *wordNode = (WordNode *)calloc(1, sizeof(WordNode));
    MALLOC_CHECK(stderr, wordNode);
    
    //wordNode->next = NULL;
    wordNode->page = docNode;
    
    wordNode->word = (char *)calloc(strlen(word) + 1, sizeof(char));       // null terminator
    
    strcpy(wordNode->word, word);
    return wordNode;
}


/* ==========================================================================
 * Take a word and DocumentNode information and a hashtable and try to put
 * or find the information in the index and update the index appropriately.
 *
 * *** Content ***
 * Case 1: Nothing found in word-hashed slot. Insert new word & doc nodes
 * Case 2: Linked list of wordnodes hashed to slot. Loop over them, if one
 * of them is for the parameter-passed word, break and assign a wordnode. If
 * not, reached end of linked list without finding word (a collision of hash
 * number). If the word was found, loop over the doc nodes. If one found,
 * increment docID, if not found, make a new one and set it to the next doc
 * node. If that doc node doesn't exist for the parameter passed document ID,
 * make new document node and add to end of document node linked lists.
 * ========================================================================== */
int reloadIndexHash(char *word, int docID, int freq, HashTable *index) {
    unsigned long hashNumber = JenkinsHash(word, MAX_HASH_SLOT);
    
    DocumentNode *docNode = NULL;
    WordNode *wordNode = NULL;
    
    // Update index backwards, essentially.
    if (index->table[hashNumber]->hashKey == NULL) {
        docNode = DNode(docID, freq);
        wordNode = WNode(docNode, word);
        index->table[hashNumber]->hashKey = wordNode;    // nothing found, slot in new wordnode
        return 1;
    }
    
    else {                                  // loop in that bucket in index table
        GenHashTableNode *loopNode = index->table[hashNumber];
        WordNode *relevantWordNode = NULL;
        WordNode *lastWordNode = NULL;
        
        while (loopNode->hashKey) {
            wordNode = loopNode->hashKey;
            if (!strcmp(wordNode->word, word)) {    // if wordnode found in linked list break
                relevantWordNode = wordNode;
                break;
            }
            if (!loopNode->next) {
                lastWordNode = wordNode;        // last wordnode in linked list. assign wordnode
                break;
            }
            loopNode = loopNode->next;
        } // end while
        
        // depending on outcome of loop: relevantWordNode is a match for the inputted word
        // or lastWordNode refers to the last word node in the linked list
        
        if (relevantWordNode) {
            docNode = relevantWordNode->page;
            
            // loop through that WordNode's list of document nodes
            while (docNode) {
                if (docNode->docID == docID) {      // found a document node, increment freq.
                    docNode->freq++;
                    return 1;
                }
                
                if (!docNode->next) {               // didn't find. make new.
                    DocumentNode *addition = NULL;
                    addition = DNode(docID, freq);
                    docNode->next = addition;
                    return 1;
                }
                docNode = docNode->next;
            }
        }
        if (lastWordNode) {      // word node not found. make new word node and new doc node
            GenHashTableNode *nextNode = (GenHashTableNode*)calloc(1, sizeof(GenHashTableNode));
            
            docNode = DNode(docID, freq);
            WordNode *addition = NULL;
            addition = WNode(docNode, word);
            loopNode->next = nextNode;
            loopNode->next->hashKey = addition;
            nextNode->next = NULL;
            return 1;
            
        }
    } // end else
    return 1;
    
}


/* ==========================================================================
 * Function that calls my reload to Index function (reloadIndexHash())
 * on a file, reads the file and reconstitutes the hash index.
 *
 * *** Content ***
 * Read in characters from file until file is over, and split up the string
 * by a whitespace (strtok) and place the relevant parts into the hash table
 * ========================================================================== */
HashTable *ReadFile(char *filePath) {
    FILE *fp;
    fp = fopen(filePath, "r");
    if (!fp) {
        fprintf(stderr, "HashTable reload file failed to open.\n");
        exit (1);
    }
    
    HashTable *reloadIndex;
    reloadIndex = initHash();
    
    char *indexInfo = LoadDocIndex(filePath);         // get index.dat into a string
    
    char *readIn = NULL;
    readIn = (char*)calloc(strlen(indexInfo) + 1, sizeof(char));
    
    for ( ;; ) {
        if (fgets(readIn, strlen(indexInfo) + 1, fp) == NULL) {
            break;      // no more lines in file, stop infinite loop
        }
        
        else {
            char *document;
            char *word;
            // get the first white-spaced delimited element in index.dat
            // if properly formatted, this will be the word character.
            char copy[strlen(readIn) + 1];
            strcpy(copy, readIn);   // want to free readIn later
            
            // don't need the number of docnodes. skip over
            word = strtok(copy, " "); strtok(NULL, " ");
            
            while ((document = strtok(NULL, " ")) != NULL) {  // "document" now refers to docID
                char *freqChar = strtok(NULL, " ");     // every other number is frequency
                
                if (strchr(document, '\n')) {
                    break;  // this means that freq will be NULL, so break out
                }
                
                int ID = GetDocID(document);
                int freq = atoi(freqChar);
                
                reloadIndexHash(word, ID, freq, reloadIndex);
            }
        }
    }
    fclose(fp);
    free(readIn);
    free(indexInfo);    // char returned by LoadDoc() needs to be freed
    return reloadIndex;
    
}

/* ==========================================================================
 * Goes through the DocumentNode linked list of a WordNode and frees all
 * Document Nodes.
 *
 * *** Content ***
 * Keeps track of previous pointer and loops through linked list.
 * ========================================================================== */
void freeDocNode(WordNode *wordNode) {
    DocumentNode *docNode = wordNode->page;
    while (docNode) {
        if (!docNode->next) {
            free(docNode);
            break;
        }
        
        DocumentNode *prev = docNode;
        docNode = docNode->next;
        free(prev);
    }
}

/* ==========================================================================
 * Given a General HashTable Node pointer, frees the calloc'd word inside the
 * WordNode, the linked list of DocumentNodes inside the WordNode and finally
 * the WordNode itself.
 * ========================================================================== */
void freeWordNode(GenHashTableNode *loopNode) {
    WordNode *wordNode = loopNode->hashKey;
    free(wordNode->word);
    freeDocNode(wordNode);
    free(wordNode);
}


/* ==========================================================================
 * Frees all of the elements of the HashTable index
 *
 * *** Content ***
 * Loop over all the 'buckets' in the hashtable. For each of them, loop over
 * the list of word nodes, and for each node loop over the list of docnodes.
 * Free in this order: WordNode->word, WordNode->DocumentNode, WordNode,
 * GenHashTableNode containing WordNode. Finally free actual HashTable struct
 * ========================================================================== */
void freeIndexTest(HashTable *index) {
    for (int i = 0; i < MAX_HASH_SLOT; i++) {
        GenHashTableNode *loopNode = NULL;
        GenHashTableNode *prevLoopNode = NULL;
        loopNode = index->table[i];
        while (loopNode->hashKey) {
            if (!loopNode->next) {
                
                freeWordNode(loopNode);
                
                free(loopNode);
                loopNode = NULL;
                break;
            }
            
            prevLoopNode = loopNode;
            loopNode = loopNode->next;
            
            freeWordNode(prevLoopNode);
            
            free(prevLoopNode);
            prevLoopNode = NULL;
        }
        
        if (loopNode) {
            free(loopNode);
        }
        if (prevLoopNode) {
            free(prevLoopNode);
        }
    }
    free(index);
}


int SaveIndexToFile(HashTable *index, char *fileName) {
    WordNode *wordNode = NULL;
    DocumentNode *docNode = NULL;
    
    FILE *fp;
    fp = fopen(fileName, "w");
    if (!fp) {
        fprintf(stderr, "Error opening target file for index info.\n");
        exit (1);
    }
    
    GenHashTableNode *loopNode;
    
    int docCount;
    for (int i = 0; i < MAX_HASH_SLOT; i++) {
        
        for (loopNode = index->table[i]; loopNode->hashKey != NULL; loopNode = loopNode->next) {
            
            wordNode = loopNode->hashKey;
            
            docCount = 0;
            docNode = wordNode->page;
            for (; docNode != NULL; docNode = docNode->next) {
                docCount++;             // want the number of documents for index.dat
            }
            
            fprintf(fp, "%s %d ", wordNode->word, docCount);     // first part of index.dat
            
            docNode = wordNode->page;
            for (; docNode != NULL; docNode = docNode->next) {
                fprintf(fp, "%d %d ", docNode->docID, docNode->freq); // print pairs
            }
            
            fprintf(fp, "\n");          // formatting
            
            if (!loopNode->next) {
                break;
            }
        }
    }
    
    fclose(fp);
    return 1;
}


