#include "booleanList.h"

#ifndef NULL
#define NULL 0
#endif

booleanList * booleanList_makelist(int label){
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

    while(currNode != NULL){
        currNode = currNode->next;
    }

    currNode->next = list2;

    return list;
}