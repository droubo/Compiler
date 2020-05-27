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
#ifndef AVM_FUNC_H
#define AVM_FUNC_H

#include "../avm/avm.h"
#include "../memory/memory.h"
#include <assert.h>

extern char * avm_tostring(avm_memcell *);

void avm_dec_top(avm_memory * memory){
    if(!memory->top)
        avm_error("STACK OVERFLOW. NOT THE WEBSITE.");
    else
        --memory->top; 
}

void avm_push_envvalue(unsigned val, avm_memory * memory){
    memory->stack[memory->top].type = number_m;
    memory->stack[memory->top].data.numVal = val;
    avm_dec_top(memory);
}

void avm_callsaveenvironment(avm_memory * memory){    
    avm_push_envvalue(memory->totalActuals, memory);
    avm_push_envvalue(memory->pc + 1, memory);
    avm_push_envvalue(memory->top + memory->totalActuals + 2, memory);
    avm_push_envvalue(memory->topsp, memory);
}

void execute_call (avm_instruction * instr, avm_memory * memory) {
    avm_memcell * func = avm_translate_operand(instr->result, &(memory->ax));
    assert(func);
    avm_callsaveenvironment(memory);

    switch(func->type) {
        case userfunc_m: {
            memory->pc = func->data.funcVal.address;
            assert(memory->pc < memory->codeSize);
            assert(code[memory->pc].opcode == funcenter_v);
            break;
        }

        case string_m: avm_calllibfunc(func->data.strVal, memory); break;
        case libfunc_m: avm_calllibfunc(func->data.libfuncVal, memory); break;

        default: {
            avm_error("CALLED INVALID FUNCTION");
        }
    }
}

void execute_pusharg (avm_instruction * instr, avm_memory * memory) {

}

void execute_funcenter (avm_instruction * instr, avm_memory * memory) {

}

void execute_funcexit (avm_instruction * instr, avm_memory * memory) {

}

typedef void (*library_func_t)(void);

void libfunc_print(void) {

}

void libfunc_cos(void) {

}

void libfunc_sin(void) {

}


library_func_t library_funcs[] = {
    libfunc_print,
    libfunc_cos,
    libfunc_sin
};

library_func_t avm_getlibraryfunc (char * id){
    if(strcmp(id, "print") == 0)        return library_funcs[0];
    else if (strcmp(id, "cos") == 0)    return library_funcs[1];
    else if (strcmp(id, "sin") == 0)    return library_funcs[1];
    else avm_error("CALLED INVALID LIBRARY FUNCTION");
    return NULL;
}

void avm_calllibfunc(char * id, avm_memory * memory){
    library_func_t f = avm_getlibraryfunc(id);
    if(!f) return;

    memory->topsp = memory->top;
    memory->totalActuals = 0;
    (*f)();
    if(!memory->executionFinished)
        execute_funcexit((avm_instruction *) 0, memory);
}

#endif