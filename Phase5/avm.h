#ifndef AVM_H
#define AVM_H

#include "vmarg.h"

typedef enum vmopcode
{
    assign_v, add_v, sub_v, 
    mul_v, div_v, mod_v,
    jeq_v, jne_v, 
    jle_v, jge_v, jlt_v, 
    jgt_v, call_v, pusharg_v,
    funcenter_v, funcexit_v, newtable_v,
    tablegetelem_v, tablesetelem_v, jump_v

}vmopcode;

typedef enum avm_memcell_t {
    number_m 	= 0, 
    string_m    = 1,
    bool_m      = 2,
    table_m     = 3,
    userfunc_m  = 4,
	libfunc_m	= 5,
	nil_m		= 6,
	undef_m		= 7
} avm_memcell_t;

typedef struct avm_table {
	int a;
} avm_table;

typedef struct avm_memcell {
	avm_memcell_t type;
	union {
		double numVal;
		char * strVal;
		unsigned char boolVal;
		avm_table* tableVal;
		unsigned funcVal;
		char * libfuncVal;
	} data;
} avm_memcell;

typedef struct instruction{
	vmopcode opcode;
	unsigned int srcLine;
	vmarg arg1;
	vmarg arg2;
	vmarg result;
} instruction;

typedef void (*execute_func_t)(instruction *);

// TODO: This
void execute_assign (instruction * instr);
void execute_add (instruction * instr);
void execute_sub (instruction * instr);
void execute_mul (instruction * instr);
void execute_div (instruction * instr);
void execute_mod (instruction * instr);
void execute_jeq (instruction * instr);
void execute_jne (instruction * instr);
void execute_jle (instruction * instr);
void execute_jge (instruction * instr);
void execute_jlt (instruction * instr);
void execute_jgt (instruction * instr);
void execute_call (instruction * instr);
void execute_pusharg (instruction * instr);
void execute_funcenter (instruction * instr);
void execute_funcexit (instruction * instr);
void execute_newtable (instruction * instr);
void execute_tablegetelem (instruction * instr);
void execute_tablesetelem (instruction * instr);
void execute_nop (instruction * instr);

execute_func_t executeFuncs[] = {
	execute_assign, execute_add, execute_sub,
	execute_mul, execute_div, execute_mod,
	execute_jeq, execute_jne,
	execute_jle, execute_jge, execute_jlt,
	execute_jgt, execute_call, execute_pusharg,
	execute_funcenter, execute_funcexit, execute_newtable,
	execute_tablegetelem, execute_tablesetelem, execute_nop
};

// TODO: This
double consts_getnumber (unsigned index);
char * consts_getstring (unsigned index);
char * libfuncs_getused (unsigned index);

#endif