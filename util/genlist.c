/*	list.c List Operations
 
 Project name: crawler.c
 Component name: list.c
 
 This file contains all list functions
 
 Primary Author: Kartik Menon
 Date Created: April 2014
 
 Special considerations:
 
 ======================================================================*/
// do not remove any of these sections, even if they are empty
//
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------- Local includes  e.g., "file.h"
#include "genlist.h"
#include "header.h"


// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/*====================================================================*/
/*
 Check if null first; then follow. (Grab -> check -> run), for linked list.
 */

int append(List *list, ListNode *new) {
    
    // If first insertion
    if ((list->head == NULL) && (list->tail == NULL)) {
        list->head = list->tail = new;
        
    }
    
    else {
        list->tail->next = new;
        new->prev = list->tail;
        list->tail = new;
        new->next = NULL;
    }
    
    return 0;
}

List* initList() {
    
    // Make space for list, set head and tail to null, and connect the head and tail.
    List *newList = (struct List*)calloc(1, sizeof(List));
    
    if (newList == NULL) {
        fprintf(stderr, "Memory allocation for list initialization failed.");
        exit (1);
    }
    
    newList->head = NULL;
    newList->tail = NULL;
    
    return newList; // Return a reference to the new list.
}

void freeList(List *list) {
    
    if (isEmpty(list)) {
        free(list);
        return; // If empty list is given nothing to free but actual list
    }
    
    // Otherwise loop over all nodes and free them
    while (!isEmpty(list)) {
        pop(list); // this automatically frees the first element in the list
        
    }
    
    if (list->head) {
        free(list->head);
    }
    
    list->head = NULL;

    // CHANGE 2
    free(list);
}

int pop(List *list) {
    if (list == NULL) {
        exit (1);
    }
    
    if (isEmpty(list)) {
        return 1;
    }
    
    ListNode *temp;
    temp = list->head->next;
    free(list->head->data);
    free(list->head);
    list->head = temp;
    if (list->head) {
        list->head->prev = NULL;
    }
    return 0;
}

int isEmpty(List *list) {
    if (list->head == NULL){
        return 1;
    }
    return 0;
}

// LINKED LIST TESTING
/*int main(void) {
    List *new;
    new = initList();
    
    char *str = calloc(1, 18);
    strcpy(str, "www.dartmouth.edu");
    
    char *str2 = calloc(1, 7);
    strcpy(str2, "whatup");
    
    append(new, str);
    append(new, str2);
 
 
    freeList(new);
 
 // Nothing should print
 
 }
*/


