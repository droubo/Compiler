/**
 * Scope List implementation
 * 
 * One main list (ScopeList) contains a head for every scope list.
 * The index for said list is actually the scope of the list.
 */
#include <stdlib.h>
#include <stdio.h>
#include "symtable_types.c"


/* ***** */
/* Types */
/* ***** */

typedef struct ScopeListEntry {
	SymTabEntry * first;
	struct ScopeListEntry * next;
} ScopeListEntry;

typedef struct ScopeList {
	int maxScope;
	ScopeListEntry * first;
} ScopeList;

/* ************ */
/* Constructors */
/* ************ */

ScopeListEntry new_ScopeListEntry(SymTabEntry * first, ScopeListEntry * next){
	ScopeListEntry s;
	s.first = first;
	s.next = next;

	return s;  
}

ScopeList new_ScopeList(int maxScope, ScopeListEntry * first){
	ScopeList s;
	s.maxScope = maxScope;
	s.first = first;
	
	return s;
}

/* ********* */
/* Functions */
/* ********* */

ScopeList * init_ScopeList(){
	ScopeList * list;
	list = malloc(sizeof(ScopeList));
	*list = new_ScopeList(0, malloc(sizeof(ScopeListEntry)));
	*(list->first) = new_ScopeListEntry(NULL, NULL);
	return list;
}

ScopeListEntry * get_ScopeList(ScopeList * list, int index) {
	ScopeListEntry * currScope;
	int i;
	
	if(list->maxScope < index)
		return NULL;
	
	currScope = list->first;
	for(i = 0; i < index; i++)
		currScope = currScope->next;
	
	return currScope;
}

void insert_ScopeList(ScopeList * list, SymTabEntry * node) {
	ScopeListEntry * currScope;
	SymTabEntry * currEntry;
	int i;
	int createdScopeNodes;

	/* List already has the correct size */
	if(list->maxScope >= node->scope) {
		
		currEntry = get_ScopeList(list, node->scope)->first;
		/* Base case where there are no entries */
		if(currEntry == NULL){
			currEntry = node;
			return;
		}

		while(currEntry->nextInScope != NULL){
			currEntry = currEntry->nextInScope;
		}
		currEntry->nextInScope = node;
		return;
	/* List does not have the correct size and we need to make room */
	} else {
		/* We need to create maxScope - node->scope nodes*/
		createdScopeNodes = node->scope - list->maxScope;

		/* Get to the bottom of the list */
		currScope = list->first;
		for(i = 0; i < list->maxScope; i++){
			currScope = currScope->next;
		}
		
		for(i = 0; i < createdScopeNodes; i++){
			currScope->next = malloc(sizeof(ScopeListEntry));
			*(currScope->next) = new_ScopeListEntry(NULL, NULL);
			currScope = currScope->next;
		}

		currScope->first = node;
	}
}

int main() {
	ScopeList * list;
	SymTabEntry * entry1, * entry2, * entry3;
	ScopeListEntry * scope0, * scope1;
	list = init_ScopeList();
	
	entry1 = malloc(sizeof(SymTabEntry));
	*entry1 = new_SymTabEntry(1, NULL, NULL, 0, USERFUNC, NULL, NULL);

	entry2 = malloc(sizeof(SymTabEntry));
	*entry2 = new_SymTabEntry(1, NULL, NULL, 1, LIBFUNC, NULL, NULL);
	
	entry3 = malloc(sizeof(SymTabEntry));
	*entry3 = new_SymTabEntry(1, NULL, NULL, 4, USERFUNC, NULL, NULL);

	printf("Done creating\n");
	
	insert_ScopeList(list, entry1);
	printf("entry 1 done\n");
	insert_ScopeList(list, entry2);
	printf("entry 2 done\n");
	insert_ScopeList(list, entry3);
	printf("entry 3 done\n");
	printf("%p %p\n", entry3, list->first->next->next->next->next->first);

}
