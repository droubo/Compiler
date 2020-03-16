/**
 * Scope List implementation
 * 
 * One main list (ScopeList) contains a head for every scope list.
 * The index for said list is actually the scope of the list.
 */
#include <stdlib.h>
#include <stdio.h>
#include "scope_list.h"
#include "symtable_types.h"
#include <string.h>

/* ************ */
/* Constructors */
/* ************ */

ScopeListEntry new_ScopeListEntry(SymTabEntry * first, ScopeListEntry * next){
	ScopeListEntry s;
	s.firstSymEntry = first;
	s.next = next;

	return s;  
}

ScopeList new_ScopeList(int maxScope, ScopeListEntry * first){
	ScopeList s;
	s.maxScope = maxScope;
	s.firstScopeEntry = first;
	
	return s;
}

/* ********* */
/* Functions */
/* ********* */

ScopeList * init_ScopeList(){
	ScopeList * list;
	list = malloc(sizeof(ScopeList));
	*list = new_ScopeList(0, malloc(sizeof(ScopeListEntry)));
	*(list->firstScopeEntry) = new_ScopeListEntry(NULL, NULL);
	return list;
}

ScopeListEntry * get_ScopeList(ScopeList * list, int scope) {
	ScopeListEntry * currScope;
	int i;
	
	if(list->maxScope < scope)
		return NULL;

	currScope = list->firstScopeEntry;
	for(i = 0; i < scope; i++)
		currScope = currScope->next;

	return currScope;
}


SymTabEntry * lookup_ScopeListExclusive(ScopeList * list, int scope, const char * name){
	ScopeListEntry * currScope;
	SymTabEntry * currEntry;
	currScope = get_ScopeList(list, scope);
	currEntry = currScope->firstSymEntry;
	
	while(currEntry != NULL) {
		if(!strcmp(currEntry->name, name))
			if(currEntry->isActive)
				return currEntry;
		currEntry = currEntry->nextInScope;
	}

	return NULL;
}

SymTabEntry * lookup_ScopeListDeclare(ScopeList * list, int scope, const char * name){
	
	int i;
	SymTabEntry * currEntry;

	currEntry = NULL;
	for(i = scope; i >= 0; i--) {
		currEntry = lookup_ScopeListExclusive(list, i, name);
		if(currEntry != NULL)
			return currEntry;
	}

	return NULL;
}

SymTabEntry * lookup_ScopeListAccess(ScopeList * list, int scope, const char * name){
	
	int i;
	SymTabEntry * currEntry;

	currEntry = NULL;
	currEntry = lookup_ScopeListExclusive(list, 0, name);
	if(currEntry != NULL)
		return currEntry;

	currEntry = lookup_ScopeListExclusive(list, i, name);
	if(currEntry != NULL)
		return currEntry;
	

	return NULL;
}

void print_ScopeList(ScopeList * list){
	int i;
	ScopeListEntry * currScope;
	SymTabEntry * currEntry;

	currScope = list->firstScopeEntry;
	printf(">>> Symbols by scope\n");
	for(i = 0; i < list->maxScope + 1; i++) {
		printf(">> Scope %d :\n", i);
		currEntry = currScope->firstSymEntry;
		while(currEntry != NULL){
			print_SymTabEntry(*currEntry);
			currEntry = currEntry->nextInScope;
		}
		printf("\n");
		currScope = currScope->next;
	}
	printf(">>> End\n");
}

int insert_ScopeList(ScopeList * list, SymTabEntry * node) {
	ScopeListEntry * currScope;
	SymTabEntry * currEntry, * temp;
	int i;
	int createdScopeNodes;

	if(lookup_ScopeList(list, list->maxScope, node->name) != NULL)
		return 1;

	/* List already has the correct size */
	if(list->maxScope >= node->scope) {
		currScope = get_ScopeList(list, node->scope);

		/* Base case where there are no entries */
		if(currScope->firstSymEntry == NULL){
			currScope->firstSymEntry = node;
			return 0;
		}

		temp = currScope->firstSymEntry;
		currScope->firstSymEntry = node;
		currScope->firstSymEntry->nextInScope = temp;
		return 0;
	/* List does not have the correct size and we need to make room */
	} else {
		/* We need to create maxScope - node->scope nodes*/
		createdScopeNodes = node->scope - list->maxScope;

		/* Get to the bottom of the list */
		currScope = list->firstScopeEntry;
		for(i = 0; i < list->maxScope; i++){
			currScope = currScope->next;
		}
		
		for(i = 0; i < createdScopeNodes; i++){
			currScope->next = malloc(sizeof(ScopeListEntry));
			*(currScope->next) = new_ScopeListEntry(NULL, NULL);
			currScope = currScope->next;
		}
		list->maxScope = node->scope;
		currScope->firstSymEntry = node;
		return 0;
	}
	return 1;
}
