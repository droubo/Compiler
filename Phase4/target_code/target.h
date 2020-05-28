#ifndef _TARGET_H
#define _TARGET_H

#include "../quads/quads.h"
#include "stacks.h"

extern StringStack *StringHead, *StringTail;
extern NumStack *NumHead, *NumTail;
extern FunctionStack *FunHead, *FunTail;
extern StringStack *LibHead, *LibTail;
extern StringStack *GlobalHead, *GlobalTail;

typedef enum avm_opcode
{
    assign_v, add_v, sub_v, 
    mul_v, div_v, mod_v, 
    uminus_v, and_v, or_v, 
    not_v, jeq_v, jne_v, 
    jle_v, jge_v, jlt_v, 
    jgt_v, call_v, pusharg_v,
    funcenter_v, funcexit_v, newtable_v,
    tablegetelem_v, tablesetelem_v, jump_v

}avm_opcode;

typedef enum vmarg_t 
{
   label_a =0,
   global_a=1,
   formal_a =2,
   local_a =3,
   number_a =4,
   string_a =5,
   bool_a =6,
   nil_a =7,
   userfunc_a =8,
   libfunc_a =9,
   retval_a=10
} vmarg_t;

typedef struct vmarg
{
    vmarg_t type;
    unsigned int bool_val;
    double num_val;
    char *str_val;
    int is_getretval;
}vmarg;

typedef struct instruction 
{
    avm_opcode opcode;
    vmarg* result;
    vmarg* arg1;
    vmarg* arg2;
    unsigned int srcLine;
    unsigned int label;
}instruction;

typedef struct userfunc
{
    unsigned int address;
    unsigned int localSize;
    char* id;
}userfunc;


/* generate target code , put this in parser*/
void generate();


/* for vm */
/*
double* numConsts;
unsigned int totalNumConsts;

char** stringConsts;
unsigned int totalStringConsts;

char** namedLibFuncs;
unsigned int totalNamedLibFuncs;

userfunc* userFuncs;
unsinged int totalUserFuncs;
*/

char* consts_newstring(char *s);
double consts_newnumber(double n);
const char* libfuncs_newused(const char *s);


void produce_target_code();
extern int globals;

#endif
