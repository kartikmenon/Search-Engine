/* ========================================================================== */
/* File: hashtable.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * Author: Kartikeya Menon
 * Date: April 2014
 *
 * 
 *
 *
 * You should include in this file your functionality for the hashtable as
 * described in the assignment and lecture.
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <string.h>                          // strlen

// ---------------- Local includes  e.g., "file.h"
#include "common.h"      // common functionality
#include "hashtable.h"
#include "list.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

HashTable* initHash() {
    // So that I can "free(newHash)" at some point in my main code.
    HashTable *newHash = (struct HashTable*)calloc(1, sizeof(HashTable));
    MALLOC_CHECK(stderr, newHash);
    
    int i;
    
    // Initialize space for table and set each of its elements (i.e., a linked list) to NULL.
    for (i = 0; i < MAX_HASH_SLOT; i++) {
        newHash->table[i] = (struct HashTableNode*)calloc(1, sizeof(HashTableNode));
        MALLOC_CHECK(stderr, newHash->table[i]);
    }
    
    return newHash;
}

HashTableNode* hashLookUp(HashTable *hash, char *url) {
    // Make sure that the function will run
    if (hash == NULL) {
        fprintf(stderr, "Error. HashTable failed to initialize.\n");
        exit (1);
    }
    if (url == NULL) {
        fprintf(stderr, "Error. URL supplied to lookup HashTable invalid.\n");
        exit (1);
    }
    
    // Get HashKey out of URL with JenkinsHash function and find associated List of hashTableNodes
    unsigned long key;
    key = JenkinsHash(url, MAX_HASH_SLOT);
    HashTableNode *hashList;
    
    // Else need to loop through linked list in hash slot
    for (hashList = hash->table[key]; hashList != NULL; hashList = hashList->next) {
        
        if (!hashList->url) {
            continue;
        }
        
        if (!strcmp(url, hashList->url)) {
            return hashList;
        }
    }

    return NULL; // If that loop fails, element not found.
    
    
}

int hashInsert(HashTable *table, char *url) {
    unsigned long key = JenkinsHash(url, MAX_HASH_SLOT);
    
    // See if it's already been inserted.
    HashTableNode *hashList;
    hashList = hashLookUp(table, url);
    
    if (hashList != NULL) {

        return 0; // Item is already in hash table, don't bother inserting again.
        
    }
    
    // Otherwise, we need a new HashTableNode list to put into hash table and need to allocate memory for new stuff
    HashTableNode *toAdd;
    
    toAdd = (struct HashTableNode*)calloc(1, sizeof(HashTableNode));
    MALLOC_CHECK(stderr, toAdd);
    
    toAdd->url = (char*) calloc(1 + strlen(url), sizeof(char));
    MALLOC_CHECK(stderr, toAdd->url);
    
    strcpy(toAdd->url, url);
    toAdd->next = NULL;

    // if key'd table is null, insert. if not, loop through singly linked list of hashtable nodes
    
    if (table->table[key] == NULL) {
        table->table[key] = toAdd;
        //printf("%s\n", "First insert!");
    }
    
    else {
        HashTableNode *temp;
        temp = table->table[key];
        HashTableNode *prev;
        // Get to the end of the singly linked list (i.e., have temp as the last HashTableNode in the hash table list)
        prev = temp;
        while ((temp = temp->next) != NULL) {
            prev = temp;
            continue;
        }
        
        // add the new node and set it's next to NULL
        prev->next = toAdd;
        toAdd->next = NULL;
        
    }
    

    return 1;
    
}

void freeHash(HashTable *table) {
    if (table == NULL) {
        return; // Give up
    }
    
    HashTableNode *LL;
    // Need to free each bucket in the table as well as the table
    for (int i = 0; i < MAX_HASH_SLOT; i++) {
        
        // If no linked list in the hash, nothing to free
        /*CHANGE 1 COMMENTED OUTif (!table->table[i]) {
            continue; // nothing to free
        }*/
        
        while (table->table[i]) {
            LL = table->table[i];
            table->table[i] = table->table[i]->next;
            free(LL->url);
            LL->url = NULL;
            free(LL);
            LL = NULL;
        }
        
        if (table->table[i]) {
            free(table->table[i]);
        }
    }
    free(table);

}

// ---------------- Private prototypes

unsigned long JenkinsHash(const char *str, unsigned long mod)
{
    size_t len = strlen(str);
    unsigned long hash, i;
    
    for(hash = i = 0; i < len; ++i)
    {
        hash += str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    
    return hash % mod;
}

 //HASHTABLE/FUNCTIONS TESTING

 /*int main(void) {
     HashTable *new;
     new = initHash();
    
     long key1 = JenkinsHash("www.dartmouth.edu", MAX_HASH_SLOT);
     new->table[key1]->url = (char*)calloc(strlen("www.dartmouth.edu") + 1, sizeof(char));
     
     hashLookUp(new, "www.dartmouth.edu");
    
     hashInsert(new, "www.dartmouth.edu");
 

    
     freeHash(new);
     return 0;
}
*/

