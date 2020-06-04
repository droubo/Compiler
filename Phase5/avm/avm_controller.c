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
#include "avm.h"
#include "../command_impl/command_impl.h"
#include "../parser/parser.h"
#include "../memory/memory.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

avm_memory memory;

#define AVM_ENDING_PC memory.codeSize

avm_memcell * avm_translate_operand(vmarg arg, avm_memcell * reg){
    switch(arg.type){
        case global_a:  return &(memory.stack[AVM_STACK_SIZE - 1 - (unsigned int) arg.val]);
        case local_a:   return &(memory.stack[memory.topsp - (unsigned int) arg.val]);
        case formal_a:  return &(memory.stack[memory.topsp + AVM_STACKENV_SIZE + 1 + (unsigned int) arg.val]);

        case retval_a:  return &(memory.retval);
        case number_a: {
            reg->type = number_m;
            reg->data.numVal = consts_getNumber(arg.val);
            return reg;
        }

        case string_a: {
            reg->type = string_m;
            reg->data.strVal = consts_getString(arg.val);
            return reg;
        }

        case bool_a: {
            reg->type = bool_m;
            reg->data.boolVal = arg.val;
            return reg;
        }

        case nil_a: reg->type = nil_m; return reg;

        case userfunc_a: {
            reg->type = userfunc_m;
            reg->data.funcVal.address = user_funcs_getAddress(arg.val);
            return reg;
        }

        case libfunc_a: {
            reg->type = libfunc_m;
            reg->data.libfuncVal = lib_funcs_getUsed(arg.val);
            return reg;
        }
    }
}

void avm_execute_cycle(void) {
    if(memory.executionFinished)
        return;
    else if (memory.pc == AVM_ENDING_PC) {
        memory.executionFinished = 1;
        return;
    } else {
        assert(memory.pc < AVM_ENDING_PC);
        avm_instruction * instr = code + memory.pc;
        assert(instr->opcode >= 0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
        if(instr->srcLine)
            memory.currLine = instr->srcLine;
        unsigned oldPC = memory.pc;
        (*executeFuncs[instr->opcode])(instr, &memory);
        if(memory.pc == oldPC)
            ++memory.pc;
    }
        
}

void avm_assign(avm_memcell * lv, avm_memcell * rv){
    if(lv == rv)
        return;
    if(lv->type == table_m && rv->type == table_m &&
        lv->data.tableVal == rv->data.tableVal) return;
    if(rv->type == undef_m && memory.executionStarted)
        avm_warning("ASSIGNMENT FROM UNDEFINED CONTENT");
    avm_memcellclear(lv);
    memcpy(lv, rv, sizeof(avm_memcell));
    if(lv->type == string_m)
        lv->data.strVal = strdup(rv->data.strVal);
    else if (lv->type == table_m)
        avm_refcounter_incr(lv->data.tableVal);
}

void avm_parse_error(char * format,...){
	printf("\033[0;31mERROR WHEN LOADING MACHINE CODE: %s\n", format);
}

void avm_warning(char * format,...){
    printf("\033[01;33m[%d] @LINE %d RUNTIME WARNING: %s\033[0m\n", 
    memory.pc, code[memory.pc].srcLine, format);
}

void avm_error(char * format,...){
    printf("\033[1;31m[%d] @LINE %d RUNTIME ERROR: %s\033[0m\n", 
    memory.pc, code[memory.pc].srcLine, format);
    memory.executionFinished = 1;
    //exit(0);
}

void avm_init(char * filename){
	memory = avm_init_memory();

	FILE * file;
    file = fopen(filename, "r");
    if(!do_magic(file))
        avm_parse_error("INVALID FILE PROVIDED");
    read_const_strings(file);
    int i;
    printf("\033[01;33m>> LOADING ALPHA FILE\033[0m");
    printf("\n  \033[0;33m> CONST_STRINGS\033[0m\n");
    for(i = 0; i < const_strings.size; i++){
        printf("    [%d] %s\n", i, const_strings.array[i].data.strVal);
    }

    read_const_nums(file);
    printf("\n  \033[0;33m> CONST_NUMS\033[0m\n");
    for(i = 0; i < const_nums.size; i++){
        printf("    [%d] %f\n", i, const_nums.array[i].data.numVal);
    }

    read_user_funcs(file);
    printf("\n  \033[0;33m> USER_FUNCS\033[0m\n");
    for(i = 0; i < user_funcs.size; i++){
        printf("    [%d] %d %d\n", i, user_funcs.array[i].data.funcVal.address, 
                                    user_funcs.array[i].data.funcVal.locals);
    }

    read_lib_funcs(file);
    printf("\n  \033[0;33m> LIB_FUNCS\033[0m\n");
    for(i = 0; i < lib_funcs.size; i++){
        printf("    [%d] %s\n", i, lib_funcs.array[i].data.libfuncVal);
    }

    read_globals(file, &memory);
    printf("\n  \033[0;33m> GLOBALS\033[0m\n");
    for(i = 0; i < globals.size; i++){
        printf("    [%d]\n", i);
    }

    printf("\n  \033[0;33m> CODE\033[0m\n");
    read_code(file, &(memory.codeSize));

    printf("\n\033[01;33m>> FILE LOADED\n\033[0m");
}

int main(int argv, char * argc[]) {
    int i;
    if(argv != 2) {
        printf("\033[1;31mINVALID PARAMETERS PROVIDED.\n\033[01;33mUSAGE: ALPHA <FILENAME>\033[0m\n");
        return 1;
    }
    avm_init(argc[1]);
    
    memory.executionStarted = 1;
    printf("\n\033[01;33m>> EXECUTING CODE...\033[0m\n");
    
    while(!memory.executionFinished) avm_execute_cycle();

    printf("\n\033[01;33m>> DONE\033[0m\n");
}