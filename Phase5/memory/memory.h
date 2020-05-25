#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "../avm/avm.h"

memcell_array const_strings;
memcell_array const_nums;
memcell_array user_funcs;
memcell_array lib_funcs;
memcell_array globals;
avm_instruction * code;

avm_memory init_memory(){
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

memcell_array init_memcell_array(int size, avm_memcell_t type){
    memcell_array temp;
    int i;
    temp.array = (avm_memcell*) malloc(sizeof(avm_memcell) * size);  
    temp.size = size;
    for(i = 0; i < temp.size; i++){
        temp.array[i].type = type;
    }
    return temp;
}

void init_instruction_array(int size, int * codeSize){
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

unsigned user_funcs_getUsed_addr (unsigned index){
   return user_funcs.array[index].data.numVal;
}

char * user_funcs_getUsed_name (unsigned index){
   return user_funcs.array[index].data.strVal;
}

void push_stack(avm_memory * memory, avm_memcell * cell){
    avm_assign(&(memory->stack[memory->top]), cell);
    memory->top--;
}

#endif