#ifndef _BOOLEANLIST_H
#define _BOOLEANLIST_H

typedef struct booleanList{
    int label;
    struct booleanList * next;

} booleanList;

booleanList * booleanList_makeList(int label);
booleanList * booleanList_merge(booleanList * list1, booleanList * list2);
void backpatch(booleanList * list, unsigned int label);

#endif
