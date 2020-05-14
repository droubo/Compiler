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
