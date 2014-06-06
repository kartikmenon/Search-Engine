/* ========================================================================== */
/* File: list.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * This file contains the definitions for a doubly-linked list of WebPages.
 *
 */
/* ========================================================================== */
#ifndef LIST_H
#define LIST_H

// ---------------- Prerequisites e.g., Requires "math.h"
                                // common functionality

// ---------------- Constants

// ---------------- Structures/Types

typedef struct ListNode {
    void **data;                              // the data for a given page
    struct ListNode *prev;                   // pointer to previous node
    struct ListNode *next;                   // pointer to next node
} ListNode;

typedef struct List {
    ListNode *head;                          // "beginning" of the list
    ListNode *tail;                          // "end" of the list
} List;

// ---------------- Public Variables

// ---------------- Prototypes/Macros


/* ============================
 Linked List-related functions 
 ============================== */

/*
 Append puts a new element (i.e., a WebPage struct) onto the end (i.e., before the ListNode tail) of a doubly linked list. 
 Input: A pointer to the linked list in question, and a pointer to a WebPage struct
 Output: An int (> 0 if successful, 0 if unsuccessful). Chose this over a pointer to the newly appended WebPage because will already have that pointer in order to use this function)
 Usage Example: 
 List* name = initList();
 append(&name, WebPage);
 */

int append(List *list, ListNode *new);

/*
 Initialize a new linked list. Allocates memory with malloc() for the initial list, which is just a NULL head and NULL tail with nothing in between.
 Input: None
 Output: A pointer to the list object. This is found by re-assigning the void* pointer outputted by malloc()
 */

List* initList();

/* Free the actual list. Free stuff stored inside each listnode (i.e., each WebPage)
 Input: A List
 Output: Nothing
 */

void freeList(List *list);

/* Remove the first element (i.e., the head) from the linked list
 Input: The list
 Output: An int indicating success (= 0)
 */

int pop(List *list);

/* Figures out if list is empty (if both head and tail are NULL)
 Input: list
 Output: Int indicating empty (0) or not (1)
 */

int isEmpty(List *list);

#endif // LIST_H
