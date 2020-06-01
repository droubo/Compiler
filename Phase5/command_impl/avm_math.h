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
#ifndef AVM_MATH_H
#define AVM_MATH_H

#include "../avm/avm.h"
#include <math.h>

typedef double (*arithmetic_func_t)(double x, double y);

double add_imp(double x, double y) { return x + y; }
double sub_imp(double x, double y) { return x - y; }
double mul_imp(double x, double y) { return x * y; }
double div_imp(double x, double y) { 
    if(y == 0)
        avm_error("DIVISION BY ZERO. ARE YOU TRYING TO BREAK MATH?");
    return x / y; 
}
double mod_imp(double x, double y) { 
    if(ceil(x) != x || ceil(y) != y)
        avm_warning("PERFORMING MODULO ON NON-INTEGRAL VALUES. POSSIBLY UNPLEASANT RESULTS.");
    if(x < 0 || y < 0)
        avm_warning("PERFORMING MODULO ON NEGATIVE VALUES. POSSIBLY UNPLEASANT RESULTS.");
    
    return (unsigned) x % (unsigned) y;   
}

arithmetic_func_t arithmetic_funcs[] = {
    add_imp,
    sub_imp,
    mul_imp,
    div_imp,
    mod_imp
};

void execute_arithmetic(avm_instruction * instr, avm_memory * memory){
    avm_memcell * lv = avm_translate_operand(instr->result, (avm_memcell* ) 0);
    avm_memcell * rv1 = avm_translate_operand(instr->arg1, &(memory->ax));
    avm_memcell * rv2 = avm_translate_operand(instr->arg2, &(memory->bx));
    assert(lv && (&(memory->stack[AVM_STACK_SIZE - 1]) >= lv && lv > &(memory->stack[memory->top])) 
            || lv == &(memory->retval));
    if(instr->arg1.type != number_a)        
        assert(rv1 && (&(memory->stack[AVM_STACK_SIZE - 1]) >= rv1 && rv1 > &(memory->stack[memory->top])) 
                || rv1 == &(memory->retval) || rv1 == &(memory->ax));
    if(instr->arg2.type != number_a)    
        assert(rv2 && (&(memory->stack[AVM_STACK_SIZE - 1]) >= rv2 && rv2 > &(memory->stack[memory->top])) 
                || rv2 == &(memory->retval) || rv2 == &(memory->ax));

    if(rv1->type != number_m || rv2->type != number_m )
        avm_error("PERFORMING MATHEMATICAL OPERATION ON NON-ARITHMETIC VALUES. SEE THE PROBLEM WITH THAT?");
    else {
        arithmetic_func_t op = arithmetic_funcs[instr->opcode - add_v];
        avm_memcellclear(lv);
        lv->type = number_m;
        lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
    }
}

void execute_add(avm_instruction * instr, avm_memory * memory){ 
    execute_arithmetic(instr, memory);
}

void execute_sub(avm_instruction * instr, avm_memory * memory){ 
    execute_arithmetic(instr, memory);
}

void execute_mul(avm_instruction * instr, avm_memory * memory){ 
    execute_arithmetic(instr, memory);
}

void execute_div(avm_instruction * instr, avm_memory * memory){ 
    execute_arithmetic(instr, memory);
}

void execute_mod(avm_instruction * instr, avm_memory * memory){ 
    execute_arithmetic(instr, memory);
}

#endif