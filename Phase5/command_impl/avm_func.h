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

#define ACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1

#include "../avm/avm.h"
#include "../memory/memory.h"
#include <assert.h>
#include "../command_impl/HashTable.h"

typedef void (*library_func_t)(avm_memory*);

unsigned int avm_get_envvalue(unsigned i, avm_memory * memory){
    //assert(memory->stack[i].type == number_m);
    unsigned int val = (unsigned) memory->stack[i].data.numVal;
    assert(memory->stack[i].data.numVal == (double) val);
    return val;
}

unsigned int avm_totalactuals(avm_memory* memory)
{
    return avm_get_envvalue(memory->topsp + ACTUALS_OFFSET,memory);
}

avm_memcell* avm_getactual(unsigned int i,avm_memory* memory)
{
    assert(i < avm_totalactuals(memory));
    return &(memory->stack[memory->topsp + AVM_STACKENV_SIZE + 1 + i]);
}

/* please call free() after you call this function */
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
            sprintf(s,"%.2f",cell->data.numVal);
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
        case nil_m:
        {
            s = malloc(4);
            sprintf(s,"nil");
            break;
        }
        case userfunc_m :
        {
            s = malloc(50);
            sprintf(s,"userfunc : %.0f",cell->data.funcVal.address);
            break;
        }
        case libfunc_m :
        {
            s = malloc(50);
            sprintf(s,"libfunc (not implemented fully yet)");
            break;
        }
        case undef_m :
        {
            s = malloc(20);
            sprintf(s,"(undefined var)");
            break;
        }
        case table_m :
        {
            printf("printint table (not fully implemented)\n");
            print_table(cell->data.tableVal);
            s = NULL;
            break;
        }

        default: printf("cell->type : %d",cell->type); assert(0);
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
    avm_push_envvalue(memory->top + memory->totalActuals + 2 ,memory);
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
    assert(memory->pc == func->data.funcVal.address);

    memory->totalActuals = 0;
    avm_user_func funcInfo = avm_getfuncinfo(memory->pc, memory);
    memory->topsp = memory->top;
    memory->top = memory->top - funcInfo.locals;
    //printf("\n\nENTERED USER FUNCTION WITH ADDRESS %d AND LOCALS %d\n\n", funcInfo.address, funcInfo.locals);
}

void execute_funcexit (avm_instruction * instr, avm_memory * memory) {
    unsigned oldTop = memory->top;
    memory->top = avm_get_envvalue(memory->topsp + AVM_SAVEDTOP_OFFSET,memory);
    memory->pc = avm_get_envvalue(memory->topsp + AVM_SAVEDPC_OFFSET,memory);
    memory->topsp = avm_get_envvalue(memory->topsp + AVM_SAVEDTOPSP_OFFSET,memory);

    while(++oldTop <= memory->top)
        avm_memcellclear(&(memory->stack[oldTop]));

}


void libfunc_print(avm_memory * memory) {
   //printf("CALLED PRINT\n");
   unsigned int n = avm_totalactuals(memory);
   //printf("print : total actuals %d\n",n);
   if(n == 0) 
   {
       avm_error("libfunc print : CALLED WITH NO ARGUMENTS");
       return;
   }
   unsigned int i;
   for(i=0;i < n;i++)
   {
       char* s = avm_tostring(avm_getactual(i,memory));
       if(s != NULL)
       {
		   printf("%s", s);
           free(s);
       }
   }
}

void libfunc_cos(avm_memory* memory) {
    avm_warning("libfunc cos : has not been inmplemented yet");
    avm_memcellclear(&(memory->retval));

}

void libfunc_sin(avm_memory* memory) {
    avm_warning("libfunc sin : has not been inmplemented yet");
    avm_memcellclear(&(memory->retval));

}

char* typeStrings(avm_memcell_t type)
{
    char* s;
    switch (type)
    {
        case number_m :
        {
            s = "number";
            break;
        }
        case string_m :
        {
            s = "string";
            break;
        }
        case bool_m :
        {
            s = "bool";
            break;
        }
        case table_m :
        {
            s = "table";
        }
        case userfunc_m :
        {
            s = "userfunc";
            break;
        }
        case libfunc_m :
        {
            s = "libfunc";
            break;
        }
        case nil_m :
        {
            s = "nil";
            break;
        }
        case undef_m :
        {
            s = "undef";
            break;
        }
        default: printf("type was %d",type); assert(0);
    }
    return s;
}

void libfunc_typeof(avm_memory* memory)
{
    unsigned int n = avm_totalactuals(memory);

    if(n != 1)
        avm_error("libfunc typeof : ONE ARGUMENT (NOT %d) EXPECTED IN typeof",n);
    else
    {
        avm_memcellclear(&(memory->retval));
        memory->retval.type = string_m;
        memory->retval.data.strVal = strdup(typeStrings(avm_getactual(0,memory)->type));
    }
    
}

void libfunc_totalarguments(avm_memory* memory)
{
    /* topsp of previous activation record */
    unsigned prev_topsp = avm_get_envvalue(memory->topsp + AVM_SAVEDTOPSP_OFFSET ,memory);
    avm_memcellclear(&(memory->retval));

    /* case of no previous activation record */
    if(prev_topsp == AVM_STACK_SIZE - 1)
    {
        avm_error("libfunc totalarguments : CALLED OUTSIDE OF A FUNCTION");
        memory->retval.type = nil_m;
    }
    else
    {
        /* extract number of actual arguments for the previous activation record */
        memory->retval.type = number_m;
        memory->retval.data.numVal = avm_get_envvalue(prev_topsp + ACTUALS_OFFSET,memory);
    }
    
}

void input()
{
    avm_warning("libfunc input : has not been inmplemented yet");
}

library_func_t library_funcs[] = {
    libfunc_print,
    libfunc_cos,
    libfunc_sin,
    libfunc_typeof,
    libfunc_totalarguments,
    input
};

library_func_t avm_getlibraryfunc (char * id){
    if(strcmp(id, "print") == 0)        return library_funcs[0];
    else if (strcmp(id, "cos") == 0)    return library_funcs[1];
    else if (strcmp(id, "sin") == 0)    return library_funcs[2];
    else if (strcmp(id,"typeof") == 0) return library_funcs[3];
    else if(strcmp(id,"totalarguments") == 0) return library_funcs[4];
    else if(strcmp(id,"input") == 0) return library_funcs[5];
    else avm_error("CALLED INVALID LIBRARY FUNCTION");
    return NULL;
}

void avm_calllibfunc(char * id, avm_memory * memory){
    library_func_t f = avm_getlibraryfunc(id);
    if(!f) return;

    memory->topsp = memory->top;
    memory->totalActuals = 0;
    (*f)(memory);
    
    if(!memory->executionFinished)
        execute_funcexit((avm_instruction *) 0, memory);
}

#endif