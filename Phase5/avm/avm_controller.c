#include "avm.h"
#include "../arrays/arrays.h"
#include "../command_impl/command_impl.h"
#include <stdlib.h>
#include <assert.h>

#define AVM_STACKENV_SIZE 4
#define AVM_STACK_SIZE 2048
#define AVM_MAX_INSTRUCTIONS 2048
#define AVM_ENDING_PC codeSize

avm_memcell stack[AVM_STACK_SIZE];
avm_memcell ax, bx, cx;
avm_memcell retval;

unsigned int top, topsp;
unsigned int executionFinished = 0;
unsigned pc = 0;
unsigned currLine = 0;
unsigned codeSize = 0; 

avm_memcell * avm_translate_operand(vmarg * arg, avm_memcell * reg){
    switch(arg->type){
        case global_a:  return &stack[AVM_STACK_SIZE - 1 - (unsigned int) arg->val];
        case local_a:   return &stack[topsp - (unsigned int) arg->val];
        case formal_a:  return &stack[topsp + AVM_STACKENV_SIZE + 1 + (unsigned int) arg->val];

        case retval_a:  return &retval;
        case number_a: {
            reg->type = number_m;
            reg->data.numVal = consts_getNumber(arg->val);
            return reg;
        }

        case string_a: {
            reg->type = string_m;
            reg->data.strVal = consts_getString(arg->val);
            return reg;
        }

        case bool_a: {
            reg->type = bool_m;
            reg->data.boolVal = arg->val;
            return reg;
        }

        case nil_a: reg->type = nil_m; return reg;

        case userfunc_a: {
            reg->type = userfunc_m;
            reg->data.funcVal = arg->val;
            return reg;
        }

        case libfunc_a: {
            reg->type = libfunc_m;
            reg->data.funcVal = arg->val;
            return reg;
        }
    }
}

void execute_cycle(void) {
    if(executionFinished)
        return;
    else if (pc == AVM_ENDING_PC) {
        executionFinished = 1;
        return;
    } else {
        assert(pc < AVM_ENDING_PC);
        avm_instruction * instr = code + pc;
        assert(instr->opcode >= 0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
        if(instr->srcLine)
            currLine = instr->srcLine;
        unsigned oldPC = pc;
        (*executeFuncs[instr->opcode])(instr);
        if(pc == oldPC)
            ++pc;
    }
        
}

double consts_getNumber (unsigned index){
    
}

char * consts_getString (unsigned index){

}

char * libfuncs_getUsed (unsigned index){

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

int main() {
	/* ASSIGN x 10 */
   	init_memcell_array(1, &const_nums);
	init_instruction_array(1, &code);
	const_nums[0].data.numVal = 10;
	codeSize = 1;
	code[0].opcode = assign_v;
	code[0].result.type = global_a;
	code[0].result.val = 0;
	code[0].arg1.type = number_a;
	code[0].arg1.val = 0;

	execute_cycle();
}


