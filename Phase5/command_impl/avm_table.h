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
#ifndef AVM_TABLE_H
#define AVM_TABLE_H

#include "avm_table_impl.h"
#include "../avm/avm.h"
#include "avm_func.h"

void avm_refcounter_incr(avm_table * t) {
	t->refcounter++;
}

void avm_refcounter_decr(avm_table * t) {
	t->refcounter--;
}

void execute_newtable(avm_instruction *instr, avm_memory *memory) {
	avm_memcell *lv = avm_translate_operand(instr->result, (avm_memcell *)0);
	assert(lv && (&memory->stack[AVM_STACK_SIZE - 1] >= lv && lv > &memory->stack[memory->top] || lv == &memory->retval));
	avm_memcellclear(lv);

	lv->type = table_m;
	lv->data.tableVal = avm_newTable();
	avm_refcounter_incr(lv->data.tableVal);
}

void execute_tablegetelem(avm_instruction *instr, avm_memory *memory) {
	avm_memcell *lv = avm_translate_operand(instr->result, (avm_memcell *)0);
	avm_memcell *t = avm_translate_operand(instr->arg1, (avm_memcell *)0);
	avm_memcell *i = avm_translate_operand(instr->arg2, &memory->ax);
	assert(lv && &memory->stack[AVM_STACK_SIZE - 1] >= lv && lv > &memory->stack[memory->top]);
	assert(t && &memory->stack[AVM_STACK_SIZE - 1] >= t && t > &memory->stack[memory->top]);
	assert(i);
	avm_memcellclear(lv);
	lv->type = nil_m;

	if (t->type != table_m) {
		avm_error("illegal use of type %s as table!", t->type);
	}
	else {
		avm_memcell *content = avm_table_getElement(t->data.tableVal, i);
		if (content) avm_assign(lv, content);
		else {
			char *ts = avm_tostring(t);
			char *is = avm_tostring(i);
			avm_warning("GETELEM not found!");
			free(ts);
			free(is);
		}
	}
}
void execute_tablesetelem(avm_instruction *instr, avm_memory *memory) {
	avm_memcell *t = avm_translate_operand(instr->result, (avm_memcell *)0);
	avm_memcell *i = avm_translate_operand(instr->arg1, &memory->ax);
	avm_memcell *c = avm_translate_operand(instr->arg2, &memory->bx);
	
	assert(t && &memory->stack[AVM_STACK_SIZE - 1] >= t && t > &memory->stack[memory->top]);
	assert(i && c);

	if (t->type != table_m)
		avm_error("illegal use of type %s as table!", t->type);
	else {
		avm_table_setElement(t->data.tableVal, i, c);
		avm_table * tab = t->data.tableVal;
	}
}

void execute_nop(avm_instruction * instr, avm_memory * memory) {

}

#endif