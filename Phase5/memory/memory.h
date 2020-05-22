#ifndef MEMORY_H
#define MEMORY_H

#define AVM_STACK_SIZE 2048
#include "../avm/avm.h"

typedef struct avm_memory {
    unsigned top;
    unsigned topsp;
    unsigned pc;
    avm_memcell stack[AVM_STACK_SIZE];
    avm_memcell ax, bx, cx;
    avm_memcell retval;
} avm_memory;

avm_memory init_memory(){
    avm_memory ret;
    ret.top = AVM_STACK_SIZE - 1;
    ret.topsp = AVM_STACK_SIZE - 1;
    ret.pc = 0;
    return ret;
}

void push_stack(avm_memory * memory, avm_memcell cell){
    memory->stack[memory->top] = cell;
    memory->top--;
}

#endif