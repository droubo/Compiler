#ifndef AVM_ASSIGN_H
#define AVM_ASSIGN_H

#include "../avm/avm.h"
#include <stdio.h>

void execute_assign (avm_instruction * instr){
    avm_memcell * cell_res, * cell_arg1;
    cell_res = malloc(sizeof(avm_memcell));
    cell_arg1 = malloc(sizeof(avm_memcell));
    cell_res = avm_translate_operand(instr->result, cell_res);
    cell_arg1 = avm_translate_operand(instr->arg1, cell_arg1);
    cell_res->data = cell_arg1->data;
}

#endif