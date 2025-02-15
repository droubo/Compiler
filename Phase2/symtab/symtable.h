#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "scope_list.h"
#include "symtable_types.h"

#define TABLE_SIZE 32

typedef struct SymTable {
    SymTabEntry ** table;
    ScopeList * list;
    int size;
} SymTable;

/**
 * @brief Initialize the symtable
 * 
 * @return initialized pointer to symtable
 */
SymTable * init_SymTable();

/**
 * @brief Insert a node to the symtable
 * 
 * @param table pointer to table to insert sym to
 * @param sym entry item
 */
void insert_SymTable(SymTable * table, SymTabEntry sym);

/**
 * @brief Lookup in all of symtable
 * 
 * @param table pointer to table to lookup in
 * @param name name of entry to lookup
 * 
 * @return pointer to entry item
 */
SymTabEntry * lookup_SymTable(SymTable * table, const char * name);

/**
 * @brief Lookup in a specific scOp
 * 
 * @param table pointer to table to lookup in
 * @param scope scope to lookup in
 * @param name name of entry to lookup
 * 
 * @return pointer to entry item
 */
SymTabEntry * lookup_SymTableScope(SymTable * table, int scope, const char * name);

/**
 * @brief table Symtable
 * 
 * @param table Symtable
 * @param scope scope to hide
 */
void hide_Scope(SymTable * table, int scope);

/**
 * @brief Print symtable
 * 
 * @param table table to prin
 */
void print_SymTable(SymTable * table);


/**
 * @brief Print scopes
 * 
 * @param table table to prin
 */
void print_Scopes(SymTable * table);

#endif
