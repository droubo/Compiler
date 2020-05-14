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
 * booleanList.h
 * List implementation for partial boolean evaluation
 */ 

#ifndef _BOOLEANLIST_H
#define _BOOLEANLIST_H

typedef struct booleanList{
    int label;
    struct booleanList * next;

} booleanList;

booleanList * booleanList_makeList(int label);
booleanList * booleanList_merge(booleanList * list1, booleanList * list2);
void booleanList_print(booleanList * l);
void backpatch(booleanList * list, unsigned int label);

#endif
