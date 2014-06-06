/* ========================================================================== */
/* File: hashtable.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Indexer General HashTable
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


#include <stdio.h>
#include <stdlib.h>
#include "./header.h"
#include "./genHashtable.h"
// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

HashTable* initHash() {

    HashTable *newHash = (struct HashTable*)calloc(1, sizeof(HashTable));
    
    MALLOC_CHECK(stderr, newHash);
    
    int i;
    for (i = 0; i < MAX_HASH_SLOT; i++) {
        newHash->table[i] = (struct GenHashTableNode*)calloc(1, sizeof(GenHashTableNode));
        MALLOC_CHECK(stderr, newHash->table[i]);
        
    }
    
    return newHash;
}

GenHashTableNode* hashLookUp(HashTable *hash, void *thing) {
    // Make sure that the function will run
    if (hash == NULL) {
        fprintf(stderr, "Error. HashTable failed to initialize.\n");
        exit (1);
    }
    if (thing == NULL) {
        fprintf(stderr, "Error. Input supplied to lookup HashTable invalid.\n");
        exit (1);
    }
    
    GenHashTableNode *hashList;
    
    unsigned long key;
    key = JenkinsHash((char*)thing, MAX_HASH_SLOT);
    if (!hash->table[key]->hashKey) {             // if bucket's data null give up
        return NULL;
    }
    
    // Else need to loop through linked list in hash slot
    for (hashList = hash->table[key]; hashList != NULL; hashList = hashList->next) {
        
        if (!strcmp((char*)thing, (char*)hashList->hashKey)) {
            return hashList;
        }
    }

    return NULL;
    
    
}

int hashInsert(HashTable *table, void *thing) {
    unsigned long key = JenkinsHash((char*)thing, MAX_HASH_SLOT);
    // See if it's already been inserted.
    GenHashTableNode *hashList;
    hashList = hashLookUp(table, thing);
    
    if (hashList != NULL) {

        return 0; // Item is already in hash table.
        
    }
    
    // Otherwise, we need a new HashTableNode list to put into hash table and need to allocate memory for new stuff
    GenHashTableNode *toAdd;
    
    toAdd = (struct GenHashTableNode*)calloc(1, sizeof(GenHashTableNode));
    MALLOC_CHECK(stderr, toAdd);
    
    toAdd->hashKey = calloc(1, sizeof(thing));
    
    toAdd->hashKey = thing;
    toAdd->next = NULL;

    // first insert to linked list
    if (table->table[key]->hashKey == NULL) {
        table->table[key] = toAdd;
    }
    
    else {
        GenHashTableNode *temp;
        temp = table->table[key];
        GenHashTableNode *prev;
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
    
    GenHashTableNode *LL;
    for (int i = 0; i < MAX_HASH_SLOT; i++) {
        
        if (!table->table[i]->hashKey) {
            free(table->table[i]->hashKey);
            free(table->table[i]); // If nothing in it still need to free.
        }
        
        else {
        
            while (table->table[i]) {
                LL = table->table[i];
                table->table[i] = table->table[i]->next;
                free(LL->hashKey);
                free(LL);
            }
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
     
     char *str = calloc(1, 18);
     strcpy(str, "www.dartmouth.edu");
    
     if (hashLookUp(new, "www.dartmouth.edu")) {
         printf("hello");
         
     }
    
   
     
     char *str2 = calloc(1, 7);
     strcpy(str2, "whatup");
     
     hashInsert(new, str);
     
     if (hashLookUp(new, "www.dartmouth.edu")) {
         hashInsert(new, str2);
         printf("whadup");
     }
     
     
     
     int key;
     key = JenkinsHash("www.dartmouth.edu", MAX_HASH_SLOT);
     printf("%d\n", key);
    
     freeHash(new);
     return 0;
}

*/

