#include "symtable.h"
#include <stdlib.h>
#include <stdio.h>

SymTable new_SymTable(SymTabEntry * table, int size){
    SymTable s;
    s.table = table;
    s.size = size;

    return s;
}

SymTable * init_SymTable(){
    SymTable * s;
    s = (SymTable *) malloc(sizeof(SymTable));
    *s = new_SymTable((SymTabEntry *) malloc(sizeof(SymTabEntry) * TABLE_SIZE), TABLE_SIZE);
    return s;
}

int insert_SymTable(SymTable table, SymTabEntry entry){
    return NULL;
}

SymTabEntry lookup_SymTable(SymTable table, const char * name){
    SymTabEntry a;
    return a;
}

int main() {
    printf("poop\n");
}
