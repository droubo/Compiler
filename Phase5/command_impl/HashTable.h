#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string.h>
#include "../avm/avm.h"


avm_memcell *avm_tablegetelem(
	avm_table* table,
	avm_memcell* index
) {
	if (table->table == NULL) {
		return NULL;
	}
	else if (index->type != number_m && index->type != string_m) return NULL;
	else {
		avm_hashtable *tmp = (avm_hashtable*) table->table;
		while (tmp != NULL) {
			if (index->type == number_m) {
				if (tmp->index->data.numVal == index->data.numVal) {
					return tmp->content;
				}
			}
			else {
				if (!strcmp(tmp->index->data.strVal,index->data.strVal)) {
					return tmp->content;
				}
			}
			tmp = tmp->next;
		}
		return NULL;
	}
}

void avm_tablesetelem(
	avm_table* table,
	avm_memcell *index,
	avm_memcell *content) {
	
	avm_hashtable *tmp = table->table;
	if (tmp == NULL) {
		avm_hashtable *newelem = (avm_hashtable *)malloc(sizeof(avm_hashtable));
		newelem->index = index;
		newelem->content = content;
		newelem->next = NULL;
		table->table = (avm_hashtable*) newelem;
		printf("NEWTABLE %d\n", (int)((avm_hashtable*)table->table)->content->data.numVal);
		return;
	}
	printf("HEAD: %d\n", (int)((avm_hashtable*)table->table)->content->data.numVal);
	while (tmp != NULL) {
		if (index->type == number_m) {
			if ((int)tmp->index->data.numVal == (int)index->data.numVal) {
				printf("PREV: %d NEXT %d\n", (int)tmp->index->data.numVal, (int)index->data.numVal);
				tmp->content = content;
				return;
			}
		}
		else {
			if (!strcmp(tmp->index->data.strVal, index->data.strVal)) {
				tmp->content = content;
				return;
			}
		}
		if (tmp->next == NULL) {
			avm_hashtable *newelem = (avm_hashtable *)malloc(sizeof(avm_hashtable));
			newelem->index = index;
			newelem->content = content;
			newelem->next = NULL;
			tmp->next = newelem;
			//printf("PREV: %d NEXT %d\n", (int)tmp->content->data.numVal, (int)tmp->next->content->data.numVal);
			return;
		}
		tmp = tmp->next;
	}
}

print_table(avm_table *table) {
	avm_hashtable *tbl = table->table;
	while (tbl != NULL) {
		printf("TABLEELEM: %d\n", (int)tbl->index->data.numVal);
		tbl = tbl->next;
	}
}

#endif