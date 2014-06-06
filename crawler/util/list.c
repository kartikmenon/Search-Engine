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
#include "list.h"
#include "web.h"
#include "utils.h"


// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/*====================================================================*/
/*
 Check if null first; then follow. (Grab -> check -> run), for linked list.
 */

int append(List *list, WebPage *webpage) {
    ListNode *new = (ListNode*)calloc(1, sizeof(ListNode));
    
    if (new == NULL) {
        fprintf(stderr, "Memory allocation for new ListNode failed.\n");
        return 1;
    }
    
    // Slot it in between n (=tail->prev) and tail
    // [  head  ] <-> [  1  ] <-> ... <-> [  n  ] <-> [  tail  ]
    
    // Copy input page in case it is freed. Remember to free it in crawler.
    /* WebPage *newPage = (WebPage*)malloc(sizeof(WebPage));
     
     if (newPage == NULL) {
     fprintf(stderr, "Memory allocation for new WebPage copy failed.\n");
     return 1;
     }
     memcpy(newPage, webpage, sizeof(WebPage));*/
    
    new->page = webpage; // Assign webpage to newly created ListNode
    
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
    List *newList = (struct List*)malloc(sizeof(List));
    
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
        return; // If empty list is given nothing to free
    }
    
    // Otherwise loop over all nodes and free them
    while (!isEmpty(list)) {
        pop(list); // this automatically frees the first element in the list
        
    }
    
    if (list->head) {
        free(list->head);
    }
    
    list->head = NULL; // reset head.
    // CHANGE 2
    free(list);
}

int pop(List *list) {
    if (list == NULL) {
        exit (1);
    }
    
    ListNode *temp;
    temp = list->head->next;
    free(list->head->page->url);
    free(list->head->page->html);
    free(list->head->page);
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
 WebPage *newPage = (struct WebPage*)malloc(sizeof(WebPage));
 newPage->url = "www.dartmouth.edu";
 newPage->html = "this is the html";
 newPage->html_len = 10;
 newPage->depth = 2;
 
 
 append(new, newPage);
 append(new, newPage);
 
 pop(new);
 
 freeList(new);
 
 // Nothing should print
 
 }
 */


