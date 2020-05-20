#ifndef AVM_H
#define AVM_H

#define MAGICNUMBER "42069"
#define MAGICNUMBERSIZE 5

#include <stdlib.h>

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
} vmarg;

typedef enum vmopcode
{
    assign_v, 
    
    add_v, sub_v, mul_v, 
    div_v, mod_v,

    jeq_v, jne_v, jle_v, 
    jge_v, jlt_v, jgt_v, 
    jump_v,

    call_v, pusharg_v, funcenter_v, 
    uncexit_v, 

    newtable_v, tablegetelem_v, tablesetelem_v,
    nop_v
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

typedef struct avm_user_func{
    unsigned address;
    unsigned locals;
    char * ID;
} avm_user_func;

typedef struct avm_memcell {
	avm_memcell_t type;
	union {
		double numVal;
		char * strVal;
		unsigned char boolVal;
		avm_table* tableVal;
		avm_user_func funcVal;
		char * libfuncVal;
	} data;
} avm_memcell;

typedef struct avm_instruction{
	vmopcode opcode;
	unsigned int srcLine;
	vmarg arg1;
	vmarg arg2;
	vmarg result;
} avm_instruction;

avm_memcell * avm_translate_operand(vmarg * arg, avm_memcell * reg);
void execute_cycle(void);

/** Command execution functions **/

typedef void (*execute_func_t)(avm_instruction *);

// TODO: These in the appropriate files

// avm_assign.h
void execute_assign (avm_instruction * instr);

// avm_math.h
void execute_add (avm_instruction * instr);
void execute_sub (avm_instruction * instr);
void execute_mul (avm_instruction * instr);
void execute_div (avm_instruction * instr);
void execute_mod (avm_instruction * instr);

// avm_jump.h
void execute_jeq (avm_instruction * instr);
void execute_jne (avm_instruction * instr);
void execute_jle (avm_instruction * instr);
void execute_jge (avm_instruction * instr);
void execute_jlt (avm_instruction * instr);
void execute_jgt (avm_instruction * instr);
void execute_jump (avm_instruction * instr);

// avm_func.h
void execute_call (avm_instruction * instr);
void execute_pusharg (avm_instruction * instr);
void execute_funcenter (avm_instruction * instr);
void execute_funcexit (avm_instruction * instr);

// avm_table.h
void execute_newtable (avm_instruction * instr);
void execute_tablegetelem (avm_instruction * instr);
void execute_tablesetelem (avm_instruction * instr);
void execute_nop (avm_instruction * instr);

execute_func_t executeFuncs[] = {
	execute_assign,

    execute_add, execute_sub, execute_mul, 
    execute_div, execute_mod,

	execute_jeq, execute_jne, execute_jle,
    execute_jge, execute_jlt, execute_jgt,
    execute_jump,

    execute_call, execute_pusharg, execute_funcenter, 
    execute_funcexit,

    execute_newtable, execute_tablegetelem, execute_tablesetelem,
    execute_nop
};


// TODO: This
double consts_getNumber (unsigned index);
char * consts_getString (unsigned index);
char * libfuncs_getUsed (unsigned index);

/** Mem clear functions **/
typedef void (*memclear_func_t) (avm_memcell*);

void memclear_string(avm_memcell * m);
void memclear_table(avm_memcell * m);
void avm_memcellclear(avm_memcell * m);

memclear_func_t memclearFuncs[] = {
    0,
    memclear_string,
    0,
    memclear_table,
    0,
    0,
    0,
    0
};

#endif