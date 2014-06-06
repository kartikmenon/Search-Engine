/* ========================================================================== */
/* File: indexer.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Indexer
 *
 * Author: Kartik Menon
 * Date: April 2014
 *
 * Input:
 * Output:
 *
 * Pseudocode:
 *
 */
/* ========================================================================== */

#include "./web.h"
#include "./header.h"
#include "./genHashtable.h"
#include "./file.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

/* ==========================================================================
 * Check that the arguments are validated.
 *
 * *** Content ***
 * Looks for wrong number of arguments.
 * Looks a proper target directory.
 * Sees and warns user if index.dat already exists.
 * ========================================================================== */

void validate(int argc, char* argv[]) {
    struct stat doc;
    
    if ((argc != 3) && (argc != 5)) {
        fprintf(stderr, "Error: Insufficient arguments. 3 required for indexing, 5 for testing mode.\n");
        exit (1);
    }
    
    if (stat(argv[1], &doc) != 0) {
        fprintf(stderr, "Error: The supplied directory was not found.\n");
        exit (1);
    }
    
    struct stat indexFile;
    
    if (stat(argv[2], &indexFile) == 0) {
        fprintf(stderr, "Warning: The supplied file already exists, overwriting.\n");
    }
    
    if (argc == 5) {
        struct stat outFile;
        if (stat(argv[4], &outFile) == 0) {
            fprintf(stderr, "Warning: The supplied re-write file already exists, overwriting.\n");
        }
    }
    
    
    if (!IsDir(argv[1])) {
        fprintf(stderr, "IsDir returned failure. Supplied directory not valid.\n");
        exit (1);
    }
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
 * Loads the HTML document from the supplied file.
 *
 * *** Content ***
 * Checks to see if the supplied parameter file name is a valid file.
 * File will have format as follows:
 * Line 1: URL
 * Line 2: Depth of URL in crawler
 # Line 3 - n: HTML.
 * Read file length with ftell, allocate that length for the HTML.
 * Rewind the buffer position to the beginning of the file and read in.
 * ========================================================================== */
char *LoadDoc(char *fileName) {
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
    
    int line1 = charCount(fp);
    int line2 = charCount(fp);
    
    long offset = line1 + line2;            // get offset to read in crawled file
    
    if (fseek(fp, offset, SEEK_END) == 0) { // fseek weird; returns 0 on success
        size = ftell(fp);
        if (size <= 0) {                     // ftell returns -1 on failure
            fprintf(stderr, "Error: HTML file could not be read.\n");
            exit (1);
        }
    }
    rewind(fp);                             // beginning of the buffer
    
    charCount(fp); charCount(fp);           // move buffer up by two lines
    
    HTMLstring = (char*)calloc(1, sizeof(char) * (size + 1 + 1)); // '/0', "/"
    
    size_t result = fread(HTMLstring, sizeof(char), size, fp); // copy to html buffer
    
    if (!result) {
        fprintf(stderr, "Error getting file text into HTMLstring buffer.\n");
        exit (1);
    }
    // change '/0'
    HTMLstring[result++] = '\0';     // need to null-terminate, fread doesn't
    
    fclose(fp);
    return HTMLstring;
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
 * Updates the index (a.k.a. a hash table) for each new word looped through
 *
 * *** Content ***
 * Looks up the word in the hashtable. Can't use conventional hashLookUp()
 * given in genHashtable.c, because trying to look up a word in the hash
 * bucket where a WordNode is stored. Can't compare word and WordNode.
 * If not found, make new WordNode and DocumentNode (freq = 1 initially) for
 * that word.
 * If found, go through the WordNode's list (->next) of DocumentNodes to see if
 * DocumentNode exists for current document
 * If no, add new DocumentNode with identifier from loop and freq = 1
 * If yes, increment DocumentNode's freq.
 * ========================================================================== */
int FillIndex(char *word, int documentID, HashTable *index) {
    unsigned long hashNumber;
    hashNumber = JenkinsHash(word, MAX_HASH_SLOT);
    
    DocumentNode *docNode = NULL;
    WordNode *wordNode = NULL;
    
    // No wordnode in index for the parameter word
    if (index->table[hashNumber]->hashKey == NULL) {
        docNode = DNode(documentID, 1);         // first insertion freq = 1
        wordNode = WNode(docNode, word);
        index->table[hashNumber]->hashKey = wordNode;
        index->table[hashNumber]->next = NULL;  // pre-emptively set
        return 1;
        
    }
    
    else {
        
        GenHashTableNode *loopNode; // wordnode somewhere in loopNode->hashKey
        
        // Loop over all possible word nodes stored within one hash slot (collisions)
        for (loopNode = index->table[hashNumber]; loopNode->hashKey != NULL; loopNode = loopNode->next) {
            wordNode = loopNode->hashKey;
            
            // Word found within an existing word node hashed into index.
            if (!strcmp(wordNode->word, word)) {
                
                docNode = wordNode->page;
                // Loop over all of that WordNode's DocumentNodes to see if one exists for
                // current documentID (passed as parameter)
                for (; docNode != NULL; docNode = docNode->next) {
                    if (docNode->docID == documentID) {
                        docNode->freq += 1;
                        return 1;                  // done. Found document node.
                    }
                    
                    if (!docNode->next) {
                        break;
                    }
                }
                
                // Current document not found in WordNode's set of DocumentNodes. Make a new one.
                DocumentNode *addition = NULL;
                addition = DNode(documentID, 1);        // initial freq = 1
                
                // loopNode->hashKey now refers to the relevant wordNode
                docNode->next = addition;
                return 1;                           // done. new document node added.
            }
            
            if (!loopNode->next) {
                break;
            }
            
        } // end for loop
        
        // loopNode now refers to the outermost word node hashed into the same slot in index hash table. Set it's next to the WordNode and the next after that to NULL
        // Make a new node for the word node to go into.
        GenHashTableNode *nextNode = (GenHashTableNode*)calloc(1, sizeof(GenHashTableNode));
        
        docNode = DNode(documentID, 1);
        wordNode = WNode(docNode, word);
        loopNode->next = nextNode;
        loopNode->next->hashKey = wordNode;
        nextNode->next = NULL;
        return 1;
    } // end else
}

/* ==========================================================================
 * Prints out the entire hash table into a file
 *
 * *** Content ***
 * Loop through all buckets of hash list, and for each bucket loop through
 * linked list of word nodes.
 * ========================================================================== */

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
 * Frees all of the elements of the HashTable index - OLD VERSION
 * Keeping this function here for nostalgic purposes.
 * ========================================================================== */
void freeIndex(HashTable *index) {
    
    for (int i = 0; i < MAX_HASH_SLOT; i++) {
        GenHashTableNode *loopNode;     // going to free this
        loopNode = index->table[i];
        
        //for (; loopNode->hashKey != NULL; loopNode = loopNode->next) {
        while (loopNode->hashKey) {
            if (!loopNode->next) {    // avoid seg fault
                
                WordNode *wordNode = loopNode->hashKey;
                free(wordNode->word);
                DocumentNode *docNode = wordNode->page;
                while (docNode) {   // go through and destroy all doc nodes
                    if (!docNode->next) {   // avoid seg fault
                        free(docNode);
                        break;
                    }
                    DocumentNode *prev = docNode;
                    docNode = docNode->next;
                    free(prev);
                }
                free(wordNode);
                free(loopNode);
                break;
            }
            
            GenHashTableNode *prevLoopNode;     // hold on to previous to free it
            prevLoopNode = loopNode;
            loopNode = loopNode->next;          // increment
            
            WordNode *prevwd = prevLoopNode->hashKey;   // same as if !loopNode->next
            free(prevwd->word);
            DocumentNode *prevdd = prevwd->page;
            while (prevdd) {
                if (!prevdd->next) {
                    free(prevdd);
                    break;
                }
                DocumentNode *beforePrev = prevdd;
                prevdd = prevdd->next;
                free(beforePrev);
            }
            free(prevwd);
            free(prevLoopNode);
        }
    }
    free(index);
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

int main(int argc, char *argv[]) {
    int fileNum;
    char *targetDir;
    char **fileNamesInDir = NULL;
    char *html = NULL;
    int pos;
    char *word = NULL;
    char *targetFile;
    char *fileName;
    
    char *filePath;
    int newDocID;
    int wordLen;
    int testID;
    validate(argc, argv);
    HashTable *index;
   
    targetDir = argv[1];
    
    index = initHash();
    LOG(stdout, "Building the index.\n");
    
    fileNum = GetFilenamesInDir(targetDir, &fileNamesInDir);

    
    if (fileNum <= 0) {
        fprintf(stderr, "Failed to get any filenames.\n");

        if (fileNamesInDir) {
            free(fileNamesInDir);
        }
        return 1;
    }
    
    targetFile = argv[2];
    
    for (int i = 0; i < fileNum; i++) {
        fileName = fileNamesInDir[i];
        testID = atoi(fileName);
        if (testID < 1) {
            fprintf(stderr, "The files in your directory are improperly named. They should be called: 1, 2, ... n.\n");
            exit (1);
        }

        
        filePath = makeFilePath(targetDir, fileName);

        html = LoadDoc(filePath);
        pos = 0;
        while ((pos = GetNextWord(html, pos, &word)) > 0) {
            NormalizeWord(word);
            
            wordLen = strlen(word);
            if (wordLen < 3) {
                free(word);
                continue;       // discount words less than 3 characters
            }
            
            newDocID = GetDocID(fileName);
            
            FillIndex(word, newDocID, index);
            free(word);
            word = NULL;
            
        }
        free(fileName);
        fileName = NULL;
        free(html);
        html = NULL;
        free(filePath);
        filePath = NULL;
        
    }
    
    if (word) {
        free(word);
    }
    
    SaveIndexToFile(index, targetFile);
    LOG(stdout, "Done building index. Index saved in target file.\n");
    if (html) {
        free(html);
    }
    
    if (filePath) {
        free(filePath);
    }
    free(fileNamesInDir);
    free(filePath);
    freeIndexTest(index);

    if (argc == 5) {
        LOG(stdout, "Testing index rebuilding.\n");
        
        HashTable *reload;
        char *outFile = argv[4];
        
        // Read in from the same index.dat that was outputted.
        // ASSUMES index.dat was placed in same directory as where the call to indexer
        // executable was made
        char *filePath = makeFilePath(".", argv[3]); // change def char*
        reload = ReadFile(filePath);
        SaveIndexToFile(reload, outFile);
        LOG(stdout, "Index rebuild test complete.\n");
        free(filePath);
        freeIndexTest(reload);
        
    }
    
}