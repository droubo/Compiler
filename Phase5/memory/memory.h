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
#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "../avm/avm.h"

avm_memcell_array const_strings;
avm_memcell_array const_nums;
avm_memcell_array user_funcs;
avm_memcell_array lib_funcs;
avm_memcell_array globals;
avm_instruction * code;

avm_memory avm_init_memory(){
    avm_memory ret;
    int i;
    ret.top = AVM_STACK_SIZE - 1;
    ret.topsp = AVM_STACK_SIZE - 1;
    ret.pc = 0;
    ret.executionFinished = 0;
    ret.currLine = 0;
    ret.codeSize = 0; 
    ret.executionStarted = 0;
    for(i = 0; i < AVM_STACK_SIZE; i++)
        ret.stack[i].type = undef_m;
    return ret;
}

avm_memcell_array init_avm_memcell_array(int size, avm_memcell_t type){
    avm_memcell_array temp;
    int i;
    temp.array = (avm_memcell*) malloc(sizeof(avm_memcell) * size);  
    temp.size = size;
    for(i = 0; i < temp.size; i++){
        temp.array[i].type = type;
    }
    return temp;
}

void init_avm_instruction_array(int size, int * codeSize){
    code = (avm_instruction*) malloc(sizeof(avm_instruction) * size);
    (*codeSize) = size;
}

double consts_getNumber (unsigned index){
    return const_nums.array[index].data.numVal;
}

char * consts_getString (unsigned index){
    assert(index <= const_strings.size);
	return const_strings.array[index].data.strVal;
}

char * lib_funcs_getUsed (unsigned index){
    return lib_funcs.array[index].data.strVal;
}

unsigned user_funcs_getAddress(unsigned index){
    return user_funcs.array[index].data.funcVal.address;
}

unsigned user_funcs_getLocals(unsigned index){
    return user_funcs.array[index].data.funcVal.locals;
}

void avm_memclear_string(avm_memcell* m){
    assert(m->data.strVal);
    free(m->data.strVal);
}

void avm_memclear_table(avm_memcell* m){
    assert(m->data.tableVal);
    avm_refcounter_decr(m->data.tableVal);
}

void avm_memcellclear(avm_memcell * m){
    if(m->type != undef_m) {
        memclear_func_t f = memclearFuncs[m->type];
              
        if(f)
            (*f)(m);
        m->type = undef_m;
    }
}

void push_stack(avm_memory * memory, avm_memcell * cell){
    avm_assign(&(memory->stack[memory->top]), cell);
    memory->top--;
}

#endif