#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symtable.h"

SymTable new_SymTable(SymTabEntry ** table, ScopeList * list){
    SymTable s;
    s.table = table;
    s.size = TABLE_SIZE;
    s.list = list;
    return s;
}

SymTable * init_SymTable(){
    SymTable * s;
    s = (SymTable *) malloc(sizeof(SymTable));
    *s = new_SymTable((SymTabEntry **) malloc(sizeof(SymTabEntry *) * TABLE_SIZE), init_ScopeList());

    return s;
}

unsigned long hash(const char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % TABLE_SIZE;
}

SymTabEntry *  insert_SymTable(SymTable * table, SymTabEntry sym){
    SymTabEntry * node, *temp;
    int index;
    node = malloc(sizeof(SymTabEntry));
    *node = sym;

    insert_ScopeList(table->list, node);

    index = hash(sym.name);

    if((table->table)[index] == NULL)
        table->table[index] = node;
    else{
        temp = table->table[index];
        table->table[index] = node;
        table->table[index]->nextInHash = temp;
    }
    return node;
}

void print_SymTable(SymTable * table){
    int i;
    SymTabEntry * temp;

    printf("\n\033[1;32m>>> \033[01;33m Symbols in Table\n\n\033[0m");

    for(i = 0; i < TABLE_SIZE; i++){
        printf("\033[1;31m#%d:\n\033[0m", i);
        if(table->table[i] != NULL){
            temp = table->table[i];
            while(temp != NULL) {
                print_SymTabEntry(*temp);
                temp = temp->nextInHash;
            }
        } else
            printf("\033[1;32m(nil)\n\033[0m");
    }
    printf("\n\033[1;32m>>> \033[01;33mEND\n\033[0m");
}

SymTabEntry * lookup_SymTable(SymTable * table, const char * name){
    int index;
    SymTabEntry * currEntry;

    index = hash(name);

    currEntry = table->table[index];
    while(currEntry != NULL){
        if(!strcmp(currEntry->name, name))
            return currEntry;
        currEntry = currEntry->nextInHash;
    }
    
    return NULL;
}

SymTabEntry * lookup_SymTableScope(SymTable * table, int scope, const char * name){
    return lookup_ScopeList(table->list, scope, name);
}

void hide_Scope(SymTable * table, int scope){
    ScopeListEntry * list;
    SymTabEntry * currEntry;

    list = get_ScopeList(table->list, scope);

    if(list == NULL)
        return;

    currEntry = list->firstSymEntry;
    while(currEntry != NULL){
        currEntry->isActive = 0;
        currEntry = currEntry->nextInScope;
    }    
}

void print_Scopes(SymTable * table){
    print_ScopeList(table->list);
}
/*
int test() {
    SymTable * t;
    SymTabEntry entry1, entry2, entry3, entry4, entry5, entry6;
    
    t = init_SymTable();
    
    entry1 = new_SymTabEntry("entry1", 1, 1, NULL, NULL, 6, GLOBAL);
    entry2 = new_SymTabEntry("entry2", 1, 1, NULL, NULL, 1, GLOBAL);
    entry3 = new_SymTabEntry("entry3", 1, 1, NULL, NULL, 2, GLOBAL);
    entry4 = new_SymTabEntry("entry4", 1, 1, NULL, NULL, 0, GLOBAL);
    entry5 = new_SymTabEntry("entry5", 1, 1, NULL, NULL, 1, GLOBAL);
    entry6 = new_SymTabEntry("entry6", 1, 1, NULL, NULL, 2, GLOBAL);
    insert_SymTable(t, entry1);
    insert_SymTable(t, entry2);
    insert_SymTable(t, entry3);
    insert_SymTable(t, entry4);
    insert_SymTable(t, entry5);
    insert_SymTable(t, entry6);
    print_ScopeList(t->list);
    print_SymTable(t);
}
*/