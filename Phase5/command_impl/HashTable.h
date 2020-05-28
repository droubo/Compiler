#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string.h>
#include "../avm/avm.h"

avm_hashtable *global_tmp;

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
			if (tmp->index->type == number_m && index->type == number_m) {
				if (tmp->index->data.numVal == index->data.numVal) {
					return tmp->content;
				}
			}
			else if (tmp->index->type == string_m && index->type == string_m) {
				if (!strcmp(tmp->index->data.strVal,index->data.strVal)) {
					return tmp->content;
				}
			}
			tmp = tmp->next;
		}
		return NULL;
	}
}

avm_hashtable *avm_tablegetHash(
	avm_table* table,
	avm_memcell* index
) {
	if (table->table == NULL) {
		return NULL;
	}
	else if (index->type != number_m && index->type != string_m) return NULL;
	else {
		avm_hashtable *tmp = (avm_hashtable*)table->table;
		while (tmp != NULL) {
			if (tmp->index->type == number_m && index->type == number_m) {
				if (tmp->index->data.numVal == index->data.numVal) {
					return tmp;
				}
			}
			else if (tmp->index->type == string_m && index->type == string_m) {
				if (!strcmp(tmp->index->data.strVal, index->data.strVal)) {
					return tmp;
				}
			}
			tmp = tmp->next;
		}
		return NULL;
	}
}

void avm_tablesetelem(
	avm_table* table,
	avm_memcell *old_index,
	avm_memcell *old_content) {
	
	avm_hashtable *tmp = table->table;
	avm_memcell *index = (avm_memcell *)malloc(sizeof(avm_memcell *));
	index->type = old_index->type;
	index->data = old_index->data;

	avm_memcell *content = (avm_memcell *)malloc(sizeof(avm_memcell *));
	content->type = old_content->type;
	content->data = old_content->data;

	if (tmp == NULL) {
		avm_hashtable *newelem = (avm_hashtable *)malloc(sizeof(avm_hashtable));
		newelem->index = index;
		newelem->content = content;
		newelem->next = NULL;
		table->table = (avm_hashtable*) newelem;
		global_tmp = newelem;
		return;
	}
	tmp = avm_tablegetHash(table, index);
	if (tmp != NULL) {
		tmp->content = content;
	}
	//printf("HEAD: %d\n", (int)((avm_hashtable*)table->table)->content->data.numVal);
	tmp = table->table;
	while (tmp->next != NULL) tmp = tmp->next;
	avm_hashtable *newelem = (avm_hashtable *)malloc(sizeof(avm_hashtable));
	newelem->index = index;
	newelem->content = content;
	newelem->next = NULL;
	tmp->next = newelem;
}

print_table(avm_table *table) {
	avm_hashtable *tbl = table->table;
	while (tbl != NULL) {
		printf("TABLEELEM: %d\n", (int)tbl->content->data.numVal);
		tbl = tbl->next;
	}
}

#endif