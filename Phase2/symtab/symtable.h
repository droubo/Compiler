#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "symtable_types.h"
#include <stdlib.h>

#define TABLE_SIZE 1024

typedef struct SymTable {
    SymTabEntry * table;
    int size;
} SymTable;


SymTable new_SymTable(SymTabEntry * table, int size);

SymTable * init_SymTable();

int insert_SymTable(SymTable table, SymTabEntry entry);

SymTabEntry lookup_SymTable(SymTable table, const char * name);

#endif
