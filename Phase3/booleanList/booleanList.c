/**
 * CS-340 Compilers
 * Spring 2020
 * 
 * Project Phase 3: Intermediate code Generation
 * 
 * Antonis Droubogiannis    csd4014@csd.uoc.gr
 * Georgios Marios Kokiadis csd3962@csd.uoc.gr
 * Nikolaos Gounakis        csd3932@csd.uoc.gr
 * 
 * booleanList.c
 * List implementation for partial boolean evaluation
 */ 

#include "booleanList.h"
#include <stdlib.h>
#include <stdio.h>
#include "../quads/quads.h"

booleanList * booleanList_makeList(int label){
    booleanList * list;
    list = malloc(sizeof(booleanList));
    list->label = label;
    list->next = NULL;

    return list;
}

booleanList * booleanList_merge(booleanList * list1, booleanList * list2){
    booleanList * list;
    booleanList * currNode;

    if(list1 == NULL || list2 == NULL)
        return NULL;

    list = list1;
    currNode = list;

    while(currNode->next != NULL){
        currNode = currNode->next;
    }

    currNode->next = list2;

    return list;
}

void backpatch(booleanList * list, unsigned int label){
    while(list != NULL){
        edit_quad(list->label, NULL, NULL, NULL, label);
        list = list->next;
    }
}

/*
int test(){
    booleanList * list1 = booleanList_makelist(1);
    booleanList * list2 = booleanList_makelist(2);
    booleanList * list3 = booleanList_makelist(3);
    booleanList * list4 = booleanList_makelist(4);
    booleanList * list = booleanList_merge(booleanList_merge(list1, list2), booleanList_merge(list3, list4));

    while(list != NULL){
        printf("%d\n", list->label);
        list = list->next;
    }
}
*/
