/**
 * scope_list.h
 * Scope List
 * 
 * One main list (ScopeList) contains a head for every scope list.
 * The index for said list is actually the scope of the list.
 */

#ifndef SCOPE_LIST_H
#define SCOPE_LIST_H

#include "symtable_types.h"

/* ***** */
/* Types */
/* ***** */

typedef struct ScopeListEntry {
	SymTabEntry * firstSymEntry;
	struct ScopeListEntry * next;
} ScopeListEntry;

typedef struct ScopeList {
	int maxScope;
	ScopeListEntry * firstScopeEntry;
} ScopeList;

/* ************ */
/* Constructors */
/* ************ */

ScopeListEntry new_ScopeListEntry(SymTabEntry * first, ScopeListEntry * next);

ScopeList new_ScopeList(int maxScope, ScopeListEntry * first);

/* ********* */
/* Functions */
/* ********* */

ScopeList * init_ScopeList();

ScopeListEntry * get_ScopeList(ScopeList * list, int scope);

SymTabEntry * lookup_ScopeList(ScopeList * list, int scope, const char * name);

void print_ScopeList(ScopeList * list);

int insert_ScopeList(ScopeList * list, SymTabEntry * node);

#endif