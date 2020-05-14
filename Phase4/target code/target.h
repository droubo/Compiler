#ifndef _TARGET_H
#define _TARGET_H

#include "../quads/quads.h"

typedef enum vmopcode
{
    assign_v, add_v, sub_v, 
    mul_v, div_v, mod_v, 
    uminus_v, and_v, or_v, 
    not_v, jeq_v, jne_v, 
    jle_v, jge_v, jlt_v, 
    jgt_v, call_v, pusharg_v,
    funcenter_v, funcexit_v, newtable_v,
    tablegetelem_v, tablesetelem_v, nop_v

}vmopcode;

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
    unsigned int val;
}vmarg;

typedef struct instruction 
{
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned int srcLine;
};

struct userfunc
{
    unsigned int address;
    unsigned int localSize;
    char* id;
}

typedef struct incomplete_jump
{
    unsigned int instrNo; /* jump instruction number*/
    unsigned int iaddress;/*i-code jump target address*/
    incomplete_jump* next;/*linked list*/
}incomplete_jump;

incomplete_jump* ij_head = (incomplete_jump*) 0;
unsigned int ij_total = 0;

void add_incomplete_jump(unsigned int instrNo, unsigned iaddress);


/* generate */
extern void generate_ADD,
extern void generate_SUB(*quads);
extern void generate_MUL(*quads);
extern void generate_DIV(*quads);
extern void generate_MOD(*quads);
extern void generate_NEWTABLE(*quads);
extern void generate_TABLEGETELEM(*quads);
extern void generate_TABLESETELEM(*quads);
extern void generate_ASSIGN(*quads);
extern void generate_NOP(*quads);
extern void generate_JUMP(*quads);
extern void generate_IF_EQ(*quads);
extern void generate_IF_NOTEQ(*quads);
extern void generate_IF_GREATER(*quads);
extern void generate_IF_GREATEREQ(*quads);
extern void generate_IF_LESS(*quads);
extern void generate_IF_LESSEQ(*quads);
extern void generate_NOT(*quads);
extern void generate_OR(*quads);
extern void generate_PARAM(*quads);
extern void generate_CALL(*quads);
extern void generate_GETRETVAL(*quads);
extern void generate_FUNCSTART(*quads);
extern void generate_RETURN(*quads);
extern void generate_FUNCEND

typedef void (*generator_func_t)(quad*);

generator_func_t generators[] = {  
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_ASSIGN,
    generate_NOP,
    generate_JUMP,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_GREATER,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_LESSEQ,
    generate_NOT,
    generate_OR,
    generate_PARAM,
    generate_CALL,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_RETURN,
    generate_FUNCEND
};

void generate(void) {
    unsigned int i;
    for(i = 0; i < total; ++i){
        (*generators[quads[i].op])(quads+i);
    }
}

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

unsigned consts_newstring(char *s);
unsigned consts_newnumber(double n);
unsigned libfuncs_newused(char *s);




#endif
