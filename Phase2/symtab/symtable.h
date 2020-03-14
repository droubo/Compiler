#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "scope_list.h"

#define TABLE_SIZE 32

typedef struct SymTable {
    SymTabEntry ** table;
    int size;
} SymTable;


SymTable new_SymTable(SymTabEntry ** table, int size);

SymTable * init_SymTable();

int insert_SymTable(SymTable * table, ScopeList * list, SymTabEntry sym);

SymTabEntry lookup_SymTable(SymTable * table, const char * name);

#endif
