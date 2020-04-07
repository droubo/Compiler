#include "booleanList.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

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
