typedef struct booleanList{
    int label;
    struct booleanList * next;

} booleanList;

booleanList * makelist(int label);
booleanList * merge(booleanList * list1, booleanList * list2);