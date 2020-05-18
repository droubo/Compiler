#include "avm.h"
#include "vmarg.h"
#include "stacks.h"

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
instruction * code = (instruction *) 0;

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
            reg->data.strVal = arg->val;
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

void execute_cycle(void){
    if(executionFinished)
        return;
    else if (pc == AVM_ENDING_PC) {
        executionFinished = 1;
        return;
    } else {
        assert(pc < AVM_ENDING_PC);
        instruction * instr = code + pc;
        assert(instr->opcode >= 0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
        if(instr->srcLine)
            currLine = instr->srcLine;
        unsigned oldPC = pc;
        (*executeFuncs[instr->opcode])(instr);
        if(pc == oldPC)
            ++pc;
    }
        
}


