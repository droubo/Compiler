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
#ifndef AVM_ASSIGN_H
#define AVM_ASSIGN_H

#include "../avm/avm.h"
#include "../memory/memory.h"
#include "avm_table.h"
#include <string.h>
#include <stdio.h>

void execute_assign (avm_instruction * instr, avm_memory * memory) {
    avm_memcell * lv = avm_translate_operand(instr->result, (avm_memcell *) 0);
    avm_memcell * rv = avm_translate_operand(instr->arg1, &(memory->ax)); 
    assert(lv && (&(memory->stack[AVM_STACK_SIZE - 1]) >= lv && lv > &(memory->stack[memory->top])) 
            || lv == &(memory->retval));
    assert(rv && (&(memory->stack[AVM_STACK_SIZE - 1]) >= rv && rv > &(memory->stack[memory->top])) 
            || rv == &(memory->retval) || rv == &(memory->ax));
    
    avm_assign(lv, rv);
}

#endif