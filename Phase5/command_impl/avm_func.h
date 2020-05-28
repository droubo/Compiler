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


typedef void (*library_func_t)(avm_memory*);

unsigned int avm_get_envvalue(unsigned i, avm_memory * memory){
    assert(memory->stack[i].type == number_m);
    unsigned int val = (unsigned) memory->stack[i].data.numVal;
    assert(memory->stack[i].data.numVal == (double) val);
    return val;
}

unsigned int avm_totalactuals(avm_memory* memory)
{
    return avm_get_envvalue(memory->topsp + memory->totalActuals,memory);
}

avm_memcell* avm_getactual(unsigned int i,avm_memory* memory)
{
    assert(i < avm_totalactuals(memory));
    return &(memory->stack[memory->topsp + AVM_STACKENV_SIZE + 1 + i]);
}

char* avm_tostring(avm_memcell* cell)
{
    assert(cell);

    char* s;

    switch (cell->type)
    {
        case string_m :
        {
            s = strdup(cell->data.strVal);
            break;
        }
        case number_m :
        {
            s = malloc(50);
            sprintf(s,"%0.f",cell->data.numVal);
            break;
        }
        case bool_m :
        {
            s = malloc(6);
            if(cell->data.boolVal == 1) sprintf(s,"true");
            else if(cell->data.boolVal == 0) sprintf(s,"false");
            else assert(0);
            break;
        }
        default: assert(0);
    }
    return s;
}

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
    
    assert(instr->opcode == pusharg_v);
    avm_memcell * arg = malloc(sizeof(avm_memcell));
    arg = avm_translate_operand(instr->result,arg);
    avm_assign(&(memory->stack[memory->top]),arg);
    memory->totalActuals++;
    avm_dec_top(memory);
}

void execute_funcenter (avm_instruction * instr, avm_memory * memory) {
    avm_memcell * func = avm_translate_operand(instr->result, &(memory->ax));
    assert(func);
    printf("%d %d\n", memory->pc, func->data.funcVal.address);
    assert(memory->pc == func->data.funcVal.address);

    memory->totalActuals = 0;
    avm_user_func funcInfo = avm_getfuncinfo(memory->pc, memory);
    memory->topsp = memory->top;
    memory->top = memory->top - funcInfo.locals;
    printf("\n\nENTERED USER FUNCTION WITH ADDRESS %d AND LOCALS %d\n\n", funcInfo.address, funcInfo.locals);
}

void execute_funcexit (avm_instruction * instr, avm_memory * memory) {
    unsigned oldTop = memory->top;

}


void libfunc_print(avm_memory * memory) {
   printf("CALLED PRINT\n");
   unsigned int n = memory->totalActuals;
   unsigned int i;
   for(i=0;i < n;i++)
   {
       char* s = avm_tostring(avm_getactual(i,memory));
       puts(s);
       free(s);
   }

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
    (*f)(memory);
    memory->totalActuals = 0;
    if(!memory->executionFinished)
        execute_funcexit((avm_instruction *) 0, memory);
}

#endif