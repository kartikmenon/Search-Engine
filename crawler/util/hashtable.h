/* ========================================================================== */
/* File: hashtable.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * This file contains the definitions for a hashtable of urls.
 *
 */
/* ========================================================================== */
#ifndef HASHTABLE_H
#define HASHTABLE_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "common.h"                          // common functionality

// ---------------- Constants
#define MAX_HASH_SLOT 10000                  // number of "buckets"

// ---------------- Structures/Types

typedef struct HashTableNode {
    char *url;                               // url previously seen
    struct HashTableNode *next;              // pointer to next node
} HashTableNode;

typedef struct HashTable {
    HashTableNode *table[MAX_HASH_SLOT];     // actual hashtable
} HashTable;

// ---------------- Public Variables

// ---------------- Prototypes/Macros



/* ==================
 Hash Table Functions
 ==================== */

/*
 Initialize a new HashList. Go through each of 10,000 = MAX_HASH_SLOT buckets and set them all to null
 Input: None
 Output: A pointer to the new HashList. Re-assign malloc()-returned pointer
 */

HashTable* initHash();

/*
 Lookup an element in the HashTable
 Input: A url to lookup in the table and the HashTable you want to search for the Key in.
 Output: The HashTableNode containing the found char. If not found, the function will return a NULL.
 
 */

HashTableNode* hashLookUp(HashTable *table, char *url);

/*
 Insert an element into a Hash Table
 Input: The HashTable in question, and the url (character string) to insert
 Output: An int (> 0 if inserted, = 0 if not)
 */

int hashInsert(HashTable *table, char *url);

/* Free all elements malloc'd
 Input: Used HashTable
 Output: None
 */

void freeHash(HashTable *table);

/*
 * jenkins_hash - Bob Jenkins' one_at_a_time hash function
 * @str: char buffer to hash
 * @mod: desired hash modulus
 *
 * Returns hash(str) % mod. Depends on str being null terminated.
 * Implementation details can be found at:
 *     http://www.burtleburtle.net/bob/hash/doobs.html
 */
unsigned long JenkinsHash(const char *str, unsigned long mod);

#endif // HASHTABLE_H
