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
	else if (index->type != number_m && index->type != string_m && index->type != table_m) return NULL;
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
			else if (tmp->index->type == table_m && index->type == table_m) {
				if (tmp->index->data.tableVal == index->data.tableVal) {
					return tmp->content;
				}
			}
			tmp = tmp->next;
		}
		return NULL;
	}
}

avm_hashtable *avm_tablegetPreviousHash(
	avm_table* table,
	avm_memcell* index
) {
	if (table->table == NULL) {
		return NULL;
	}
	else if (index->type != number_m && index->type != string_m) return NULL;
	else {

		avm_hashtable *tmp = (avm_hashtable*)table->table;
		while (tmp->next != NULL) {
			if (tmp->next->index->type == number_m && index->type == number_m) {
				if (tmp->next->index->data.numVal == index->data.numVal) {
					return tmp;
				}
			}
			else if (tmp->next->index->type == string_m && index->type == string_m) {
				if (!strcmp(tmp->next->index->data.strVal, index->data.strVal)) {
					return tmp;
				}
			}
			else if (tmp->next->index->type == table_m && index->type == table_m) {
				if (tmp->next->index->data.tableVal == index->data.tableVal) {
					return tmp;
				}
			}
			tmp = tmp->next;
		}
		return NULL;
	}
}

avm_memcell * avm_tablesetelem(
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
		return NULL;
	}
	if (tmp->index->type == number_m && index->type == number_m) {
		if (tmp->index->data.numVal == index->data.numVal) {
			tmp->content = content;
			return NULL;
		}
	}
	else if (tmp->index->type == string_m && index->type == string_m) {
		if (!strcmp(tmp->index->data.strVal, index->data.strVal)) {
			tmp->content = content;
			return NULL;
		}
	}
	else if (tmp->index->type == table_m && index->type == table_m) {
		if (tmp->index->data.tableVal == index->data.tableVal) {
			return tmp->content;
		}
	}
	tmp = avm_tablegetPreviousHash(table, index);
	if (tmp != NULL) {
		if (content->type == nil_m) {
			avm_hashtable *to_remove = tmp->next;
			tmp->next = to_remove->next;
			free(to_remove);
		}
		else tmp->next->content = content;
	}
	tmp = table->table;
	if (content->type == nil_m) return NULL;
	while (tmp->next != NULL) tmp = tmp->next;
	avm_hashtable *newelem = (avm_hashtable *)malloc(sizeof(avm_hashtable));
	newelem->index = index;
	newelem->content = content;
	newelem->next = NULL;
	tmp->next = newelem;
}

void print_table(avm_table *table) {
	avm_hashtable *tbl = table->table;
	if (tbl != NULL) printf("{");
	else printf("nil table\n");
	while (tbl != NULL) {
		if(tbl->index->type == string_m) printf("\"%s\":", (char*)tbl->index->data.strVal);
		else {
			printf("%.3f:", tbl->index->data.numVal);
		}
		if (tbl->content->type == table_m) printf("table");
		//if (tbl->content->data.tableVal != NULL && tbl->content->data.tableVal != table) print_table(tbl->content->data.tableVal);
		else if (tbl->content->type == string_m) printf("\"%s\"", (char*)tbl->content->data.strVal);
		else {
			printf("%.3f", tbl->content->data.numVal);
		}
		if (tbl->next != NULL) printf(", ");
		else printf("}");
		tbl = tbl->next;
	}
}

#endif