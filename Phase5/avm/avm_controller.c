#include "avm.h"
#include "../arrays/arrays.h"
#include "../command_impl/command_impl.h"
#include "../parser/parser.h"
#include "../memory/memory.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define AVM_STACKENV_SIZE 4
#define AVM_MAX_INSTRUCTIONS 2048
#define AVM_ENDING_PC codeSize

avm_memory memory;


unsigned int executionFinished = 0;
unsigned currLine = 0;
unsigned codeSize = 0; 

avm_memcell * avm_translate_operand(vmarg arg, avm_memcell * reg){
    switch(arg.type){
        case global_a:  return &(memory.stack)[AVM_STACK_SIZE - 1 - (unsigned int) arg.val];
        case local_a:   return &(memory.stack[memory.topsp - (unsigned int) arg.val]);
        case formal_a:  return &(memory.stack[memory.topsp + AVM_STACKENV_SIZE + 1 + (unsigned int) arg.val]);

        case retval_a:  return &(memory.retval);
        case number_a: {
            reg->type = number_m;
            reg->data.numVal = 5;
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
            reg->data.funcVal.address = arg.val;
            return reg;
        }

        case libfunc_a: {
            reg->type = libfunc_m;
            reg->data.funcVal.address = arg.val;
            return reg;
        }
    }
}

void execute_cycle(void) {
    if(executionFinished)
        return;
    else if (memory.pc == AVM_ENDING_PC) {
        executionFinished = 1;
        return;
    } else {
        assert(memory.pc < AVM_ENDING_PC);
        avm_instruction * instr = code + memory.pc;
        assert(instr->opcode >= 0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
        if(instr->srcLine)
            currLine = instr->srcLine;
        unsigned oldPC = memory.pc;
        (*executeFuncs[instr->opcode])(instr);
        if(memory.pc == oldPC)
            ++memory.pc;
    }
        
}

void memclear_string(avm_memcell* m){
    assert(m->data.strVal);
    free(m->data.strVal);
}

void memclear_table(avm_memcell* m){
    assert(m->data.tableVal);
    // Decrease refrence counter of the table
    //avm_tablecrefcounter(m->data.tableVal);
}

void avm_memcellclear(avm_memcell * m){
    if(m->type != undef_m) {
        memclear_func_t f = memclearFuncs[m->type];
        if(f)
            (*f)(m);
        m->type = undef_m;
    }
}

void parse_error(char * message){
	printf("\033[0;31mERROR WHEN LOADING MACHINE CODE: %s\n", message);
}

void initialize_VM(char * filename){
	memory = init_memory();
}

int main() {
    memory = init_memory();

	FILE * file;
    file = fopen("code.txt", "r");
    do_magic(file);
    read_const_strings(file);
    int i;
    printf("\nCONST_STRINGS\n");
    for(i = 0; i < const_strings.size; i++){
        printf("%s\n", const_strings.array[i].data.strVal);
    }

    read_const_nums(file);
    printf("\nCONST_NUMS\n");
    for(i = 0; i < const_nums.size; i++){
        printf("%f\n", const_nums.array[i].data.numVal);
    }

    read_user_funcs(file);
    printf("\nUSER_FUNCS\n");
    for(i = 0; i < user_funcs.size; i++){
        printf("%d %d\n",    user_funcs.array[i].data.funcVal.address, 
                                user_funcs.array[i].data.funcVal.locals);
    }

    read_lib_funcs(file);
    printf("\nLIB_FUNCS\n");
    for(i = 0; i < lib_funcs.size; i++){
        printf("%s\n", lib_funcs.array[i].data.libfuncVal);
    }

    read_globals(file, &memory);
    printf("\nGLOBALS\n");
    for(i = 0; i < globals.size; i++){
        printf("%s\n", globals.array[i].data.strVal);
    }

    printf("\nCODE\n");
    read_code(file, &codeSize);
    printf("\n\nFILE LOADED.\n");
    execute_cycle();
}