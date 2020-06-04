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
#ifndef AVM_JUMP_H
#define AVM_JUMP_H

#include "../avm/avm.h"
#include <assert.h>

unsigned char number_tobool(avm_memcell * m) { return m->data.numVal != 0; }
unsigned char string_tobool(avm_memcell * m) { return m->data.strVal[0] != 0; }
unsigned char bool_tobool(avm_memcell * m) { return m->data.boolVal; }
unsigned char table_tobool(avm_memcell * m) { return 1; }
unsigned char userfunc_tobool(avm_memcell * m) { return 1; }
unsigned char libfunc_tobool(avm_memcell * m) { return 1; }
unsigned char nil_tobool(avm_memcell * m) { return 0; }
unsigned char undef_tobool(avm_memcell * m) { assert(0); return 0; }

typedef unsigned char (*type_tobool_t) (avm_memcell *);

type_tobool_t type_tobool[] = {
    number_tobool,
    string_tobool,
    bool_tobool,
    table_tobool,
    userfunc_tobool,
    libfunc_tobool,
    nil_tobool,
    undef_tobool
};

unsigned char avm_tobool (avm_memcell * m){
    assert(m->type >= 0 && m->type < undef_m);
    return (*type_tobool[m->type])(m);
}

void execute_jeq (avm_instruction * instr, avm_memory * memory){
    assert(instr->result.type == label_a);

    avm_memcell * rv1 = avm_translate_operand(instr->arg1, &(memory->ax));
    avm_memcell * rv2 = avm_translate_operand(instr->arg2, &(memory->bx));
    unsigned char result = 0;

    if(rv1->type == undef_m || rv2->type == undef_m)
        avm_error("UNDEFINED VALUE IN INEQUALITY. I HAVE TO KNOW WHAT THEY ARE TO COMPARE THEM.");
    else if (rv1->type == nil_m || rv2->type == nil_m)
        result = rv1->type == nil_m && rv2->type == nil_m;
    else if (rv1->type == bool_m || rv2->type == bool_m)
        result = (avm_tobool(rv1) == avm_tobool(rv2));
    else if (rv1->type != rv2->type){
        avm_error("ILLEGAL COMPARISON OF DIFFERENT TYPED VALUES (%d comp %d).\n" 
        "YOUR MATH TEACHER TOLD YOU TO COMPARE APPLES WITH APPLES.\n", rv1->type, rv2->type);
        printf("%d %d\n", rv1->data.numVal, rv2->data.strVal);
    } else {
        switch(rv1->type){
            case number_m:  { result = rv1->data.numVal == rv2->data.numVal; break; }
            case string_m:  { result = strcmp(rv1->data.strVal, rv2->data.strVal) == 0; break; }
            case table_m:
            case userfunc_m:
            case libfunc_m:   { result = 1; break; }
            default: { avm_error("UNKNOWN ERROR WHEN COMPARING VALUES. MY BAD."); }
        }
    }

    if(!memory->executionFinished && result)
        memory->pc = instr->result.val;
}

void execute_jne (avm_instruction * instr, avm_memory * memory){
    assert(instr->result.type == label_a);

    avm_memcell * rv1 = avm_translate_operand(instr->arg1, &(memory->ax));
    avm_memcell * rv2 = avm_translate_operand(instr->arg2, &(memory->bx));

    unsigned char result = 0;

    if(rv1->type == undef_m || rv2->type == undef_m)
        avm_error("UNDEFINED VALUE IN INEQUALITY. I HAVE TO KNOW WHAT THEY ARE TO COMPARE THEM.");
    else if (rv1->type == nil_m || rv2->type == nil_m)
        result = rv1->type != nil_m && rv2->type != nil_m;
    else if (rv1->type == bool_m || rv2->type == bool_m)
        result = (avm_tobool(rv1) != avm_tobool(rv2));
    else if (rv1->type != rv2->type)
        avm_error("ILLEGAL COMPARISON OF DIFFERENT TYPED VALUES (%d comp %d)." 
        "YOUR MATH TEACHER TOLD YOU TO COMPARE APPLES WITH APPLES.\n", rv1->type, rv2->type);
    else {
        switch(rv1->type){
            case number_m:  { result = rv1->data.numVal != rv2->data.numVal; break; }
            case string_m:  { result = strcmp(rv1->data.strVal, rv2->data.strVal) != 0; break; }
            case table_m:
            case userfunc_m:
            case libfunc_m:   { result = 0; break; }
            default: { avm_error("UNKNOWN ERROR WHEN COMPARING VALUES. MY BAD."); }
        }
    }

    if(!memory->executionFinished && result)
        memory->pc = instr->result.val;
}

void execute_jle (avm_instruction * instr, avm_memory * memory){
    assert(instr->result.type == label_a);
    
    avm_memcell * rv1 = avm_translate_operand(instr->arg1, &(memory->ax));
    avm_memcell * rv2 = avm_translate_operand(instr->arg2, &(memory->bx));

    if(rv1->type != number_m || rv2->type != number_m)
        avm_error("COMPARISON OPERATION WITHOUT THE USAGE OF NUMBER VALUES. I NEED NUMBERS PLS");

    unsigned char result = 0;

    result = rv1->data.numVal <= rv2->data.numVal;

    if(!memory->executionFinished && result)
        memory->pc = instr->result.val;
}

void execute_jge (avm_instruction * instr, avm_memory * memory){
    assert(instr->result.type == label_a);    
    avm_memcell * rv1 = avm_translate_operand(instr->arg1, &(memory->ax));
    avm_memcell * rv2 = avm_translate_operand(instr->arg2, &(memory->bx));

    if(rv1->type != number_m || rv2->type != number_m)
        avm_error("COMPARISON OPERATION WITHOUT THE USAGE OF NUMBER VALUES. I NEED NUMBERS PLS");
    
    unsigned char result = 0;

    result = rv1->data.numVal >= rv2->data.numVal;

    if(!memory->executionFinished && result)
        memory->pc = instr->result.val;
}

void execute_jlt (avm_instruction * instr, avm_memory * memory){
    assert(instr->result.type == label_a);
    
    avm_memcell * rv1 = avm_translate_operand(instr->arg1, &(memory->ax));
    avm_memcell * rv2 = avm_translate_operand(instr->arg2, &(memory->bx));

    if(rv1->type != number_m || rv2->type != number_m)
        avm_error("COMPARISON OPERATION WITHOUT THE USAGE OF NUMBER VALUES. I NEED NUMBERS PLS");

    unsigned char result = 0;

    result = rv1->data.numVal < rv2->data.numVal;

    if(!memory->executionFinished && result)
        memory->pc = instr->result.val;
}

void execute_jgt (avm_instruction * instr, avm_memory * memory){
    assert(instr->result.type == label_a);
    
    avm_memcell * rv1 = avm_translate_operand(instr->arg1, &(memory->ax));
    avm_memcell * rv2 = avm_translate_operand(instr->arg2, &(memory->bx));

    if(rv1->type != number_m || rv2->type != number_m)
        avm_error("COMPARISON OPERATION WITHOUT THE USAGE OF NUMBER VALUES. I NEED NUMBERS PLS");

    unsigned char result = 0;

    result = rv1->data.numVal > rv2->data.numVal;

    if(!memory->executionFinished && result)
        memory->pc = instr->result.val;
}

void execute_jump (avm_instruction * instr, avm_memory * memory){
    assert(instr->result.type == label_a);
    assert(instr->result.val >= 0 && instr->result.val <= memory->codeSize);
    if(!memory->executionFinished)
        memory->pc = instr->result.val;
}

#endif