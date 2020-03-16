#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

SymTable new_SymTable(SymTabEntry ** table){
    SymTable s;
    s.table = table;
    s.size = TABLE_SIZE;

    return s;
}

SymTable * init_SymTable(){
    SymTable * s;
    s = (SymTable *) malloc(sizeof(SymTable));
    *s = new_SymTable((SymTabEntry **) malloc(sizeof(SymTabEntry *) * TABLE_SIZE));
    return s;
}

unsigned long hash(const char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % TABLE_SIZE;
}

int insert_SymTable(SymTable * table, ScopeList * list, SymTabEntry sym){
    SymTabEntry * node, *temp;
    int index;
    node = malloc(sizeof(SymTabEntry));
    *node = sym;

    /* If we try to insert it in the scope list and it fails, it
    the symbol has already been defined in the scope (or greater scope) */
    if(insert_ScopeList(list, node) == 1)
        return 1;

    index = hash(sym.name);

    if((table->table)[index] == NULL)
        table->table[index] = node;
    else{
        temp = table->table[index];
        table->table[index] = node;
        table->table[index]->nextInHash = temp;
    }

    return 0;
}

void print_SymTable(SymTable * table){
    int i;
    SymTabEntry * temp;
    for(i = 0; i < TABLE_SIZE; i++){
        printf("#%d:\n", i);
        if(table->table[i] != NULL){
            temp = table->table[i];
            while(temp != NULL){
                print_SymTabEntry(*temp);
                temp = temp->nextInHash;
            }
            printf("\n");
        } else
            printf("(nil)\n");
    }
}

SymTabEntry * lookup_SymTable(SymTable * table, const char * name){
    SymTabEntry a;
    return a;
}

int main() {
    SymTable * t;
    ScopeList * l;
    SymTabEntry entry1, entry2, entry3, entry4, entry5, entry6;
    
    t = init_SymTable();
    l = init_ScopeList();
    
    entry1 = new_SymTabEntry("entry1", 1, 1, NULL, NULL, 6, GLOBAL, NULL, NULL);
    entry2 = new_SymTabEntry("entry2", 1, 1, NULL, NULL, 1, GLOBAL, NULL, NULL);
    entry3 = new_SymTabEntry("entry3", 1, 1, NULL, NULL, 2, GLOBAL, NULL, NULL);
    entry4 = new_SymTabEntry("entry4", 1, 1, NULL, NULL, 0, GLOBAL, NULL, NULL);
    entry5 = new_SymTabEntry("entry5", 1, 1, NULL, NULL, 1, GLOBAL, NULL, NULL);
    entry6 = new_SymTabEntry("entry6", 1, 1, NULL, NULL, 2, GLOBAL, NULL, NULL);
    insert_SymTable(t, l, entry1);
    insert_SymTable(t, l, entry2);
    insert_SymTable(t, l, entry3);
    insert_SymTable(t, l, entry4);
    insert_SymTable(t, l, entry5);
    insert_SymTable(t, l, entry6);
    print_ScopeList(l);
    print_SymTable(t);
}
