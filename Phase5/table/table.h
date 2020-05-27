/**
 * CS-340 Compilers
 * Spring 2020
 * 
 * Project Phase 5: AVM Implementation
 * 
 * Antonis Droubogiannis    csd4014@csd.uoc.gr
 * Georgios Marios Kokiadis csd3962@csd.uoc.gr
 * Nikolaos Gounakis        csd3932@csd.uoc.gr
 */ 
#ifndef TABLE_H
#define TABLE_H

#include "../avm/avm.h"

void avm_refcounter_incr(avm_table * t){
    t->refcounter++;
}

void avm_refcounter_decr(avm_table * t){
    t->refcounter--;
}
void execute_newtable(avm_instruction *instr) {
	avm_memcell *lv = avm_translate_operand(&instr->result, (avm_memcell *)0);
	assert(lv && (&stack[N - 1] >= lv && lv > &stack[top] || lv == &retval));

	avm_memcellclear(lv);

	lv->type = table_m;
	lv->data.tableVal = avm_tablenew();
	avm_refcounter_incr(lv->data.tableVal);
}

avm_memcell *avm_tablegetelem{
	avm_table* table,
	avm_memcell* index
};


void avm_tablesetelem{
	avm_table* table,
	avm_memcell *index,
	avm_memcell *content
};

void execute_tablegetelem(avm_instruction *instr) {
	avm_memcell *lv = avm_translate_operand(&instr->result, (avm_memcell *)0);
	avm_memcell *t = avm_translate_operand(&instr->arg1, (avm_memcell *)0);
	avm_memcell *i = avm_translate_operand(&instr->arg2, &ax);

	avm_memcellclear(lv);
	lv->type = nil_m;

	if (t->type != table_m) {
		avm_error("illegal use of type %s as table!", t->type);
	}
	else {
		avm_memcell *content = avm_tablegetelem(t->data.tableVal, i);
		if (content) avm_assign(lv, content);
		else {
			char *ts = avm_tostring(t);
			char *is = avm_tostring(i);
			avm_warning("%s[%s] not found!", ts, is);
			free(ts);
			free(is);
		}
	}
}

#endif