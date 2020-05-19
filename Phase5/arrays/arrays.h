#ifndef ARRAYS_H
#define ARRAYS_H

#include <stdlib.h>
#include "../avm/avm.h"

avm_memcell * const_strings;
avm_memcell * const_nums;
avm_memcell * user_funcs;
avm_memcell * lib_funcs;
avm_instruction * code;

void init_memcell_array(int size, avm_memcell ** array){
    (*array) = (avm_memcell*) malloc(sizeof(avm_memcell) * size);  
}

void init_instruction_array(int size, avm_instruction ** array){
    (*array) = (avm_instruction*) malloc(sizeof(avm_instruction) * size);  
}

#endif