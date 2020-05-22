#ifndef ARRAYS_H
#define ARRAYS_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "../avm/avm.h"

typedef struct memcell_array {
    avm_memcell * array;
    int size;
} memcell_array;

memcell_array const_strings;
memcell_array const_nums;
memcell_array user_funcs;
memcell_array lib_funcs;
memcell_array globals;
avm_instruction * code;

memcell_array init_memcell_array(int size, avm_memcell_t type){
    memcell_array temp;
    int i;
    temp.array = (avm_memcell*) malloc(sizeof(avm_memcell) * size);  
    temp.size = size;
    for(i = 0; i < const_strings.size; i++){
        temp.array[i].type = string_m;
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

#endif