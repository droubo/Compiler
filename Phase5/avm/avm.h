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
#ifndef AVM_H
#define AVM_H

#define MAGICNUMBER "42069"
#define MAGICNUMBERSIZE 5
#define AVM_STACK_SIZE 2048

#define AVM_STACKENV_SIZE 4
#define AVM_MAX_INSTRUCTIONS 2048

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

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

typedef enum avm_opcode
{
    assign_v, 
    
    add_v, sub_v, mul_v, 
    div_v, mod_v,

    jeq_v, jne_v, jle_v, 
    jge_v, jlt_v, jgt_v, 
    jump_v,

    call_v, pusharg_v, funcenter_v, 
    funcexit_v, 

    newtable_v, tablegetelem_v, tablesetelem_v,
    nop_v
}avm_opcode;

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

typedef struct avm_user_func{
    unsigned address;
    unsigned locals;
} avm_user_func;

typedef struct avm_table {
	unsigned refcounter;
	struct avm_hashtable *table;
} avm_table;

typedef struct avm_memcell {
	avm_memcell_t type;
	union {
		double numVal;
		char * strVal;
		unsigned char boolVal;
		avm_table * tableVal;
		avm_user_func funcVal;
		char * libfuncVal;
	} data;
} avm_memcell;

typedef struct avm_hashtable {
	avm_memcell* index;
	avm_memcell* content;
	struct avm_hashtable*next;
}avm_hashtable;

typedef struct avm_memory {
    unsigned top;
    unsigned topsp;
    unsigned pc;
    unsigned executionFinished;
    unsigned currLine;
    unsigned codeSize;
    unsigned executionStarted;
    unsigned totalActuals;
    avm_memcell stack[AVM_STACK_SIZE];
    avm_memcell ax, bx, cx;
    avm_memcell retval;
} avm_memory;

typedef struct avm_memcell_array {
    avm_memcell * array;
    int size;
} avm_memcell_array;

typedef struct avm_instruction{
	avm_opcode opcode;
	unsigned int srcLine;
	vmarg arg1;
	vmarg arg2;
	vmarg result;
} avm_instruction;

/** Controller Functions **/
// avm_controller.c
avm_memcell * avm_translate_operand(vmarg arg, avm_memcell * reg);
void avm_warning(char * format,...);
void avm_error(char * format,...);
void avm_execute_cycle(void);

/** Command execution functions **/
typedef void (*execute_func_t)(avm_instruction *, avm_memory *);

// TODO: These in the appropriate files

// avm_assign.h
void execute_assign (avm_instruction * instr, avm_memory * memory);
void avm_assign(avm_memcell * lv, avm_memcell * rv);

// avm_math.h
void execute_add (avm_instruction * instr, avm_memory * memory);
void execute_sub (avm_instruction * instr, avm_memory * memory);
void execute_mul (avm_instruction * instr, avm_memory * memory);
void execute_div (avm_instruction * instr, avm_memory * memory);
void execute_mod (avm_instruction * instr, avm_memory * memory);

// avm_jump.h
void execute_jeq (avm_instruction * instr, avm_memory * memory);
void execute_jne (avm_instruction * instr, avm_memory * memory);
void execute_jle (avm_instruction * instr, avm_memory * memory);
void execute_jge (avm_instruction * instr, avm_memory * memory);
void execute_jlt (avm_instruction * instr, avm_memory * memory);
void execute_jgt (avm_instruction * instr, avm_memory * memory);
void execute_jump (avm_instruction * instr, avm_memory * memory);

// avm_func.h
void execute_call (avm_instruction * instr, avm_memory * memory);
void execute_pusharg (avm_instruction * instr, avm_memory * memory);
void execute_funcenter (avm_instruction * instr, avm_memory * memory);
void execute_funcexit (avm_instruction * instr, avm_memory * memory);

// avm_table.h
void execute_newtable (avm_instruction * instr, avm_memory * memory);
void execute_tablegetelem (avm_instruction * instr, avm_memory * memory);
void execute_tablesetelem (avm_instruction * instr, avm_memory * memory);
void execute_nop (avm_instruction * instr, avm_memory * memory);

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

/** Constants fetch functions **/
// memory.h
double consts_getNumber (unsigned index);
char * consts_getString (unsigned index);
char * libfuncs_getUsed (unsigned index);

/** Mem clear functions **/
typedef void (*memclear_func_t) (avm_memcell*);

// memory.h
void avm_memclear_string(avm_memcell * m);
void avm_memclear_table(avm_memcell * m);
void avm_memcellclear(avm_memcell * m);

memclear_func_t memclearFuncs[] = {
    0,
    avm_memclear_string,
    0,
    avm_memclear_table,
    0,
    0,
    0,
    0
};

/* please call free() after you call this function */
char* avm_tostring(avm_memcell* cell)
{
    assert(cell);

    char* s;

    switch (cell->type)
    {
        case string_m :
        {
            s = strdup(cell->data.strVal);
            break;
        }
        case number_m :
        {
            s = malloc(50);
            sprintf(s,"%.3f",cell->data.numVal);
            break;
        }
        case bool_m :
        {
            s = malloc(6);
            if(cell->data.boolVal == 1) sprintf(s,"true");
            else if(cell->data.boolVal == 0) sprintf(s,"false");
            else assert(0);
            break;
        }
        case nil_m:
        {
            s = malloc(4);
            sprintf(s,"nil");
            break;
        }
        case userfunc_m :
        {
            s = malloc(50);
            int adr = (int) cell->data.funcVal.address;
            sprintf(s,"userfunc : %d",adr);
            break;
        }
        case libfunc_m :
        {
            s = malloc(50);
            sprintf(s, cell->data.libfuncVal);
            break;
        }
        case undef_m :
        {
            s = malloc(20);
            sprintf(s,"(undefined var)");
            break;
        }
        case table_m :
        {
            avm_hashtable *tbl = cell->data.tableVal->table;
	        if (tbl != NULL) printf("{");
	        else printf("nil table\n");
	        while (tbl != NULL) {
	        	if (tbl->index->type != table_m) {
	        		char* s = avm_tostring(tbl->index);
	        		if(s != NULL){
	        	   		printf("%s", s);
                   		free(s);
               		}
	        	} else avm_tostring(tbl->index);
	        	printf(" : ");
	        	if (tbl->content->type != table_m) {
	        		char* s = avm_tostring(tbl->content);
	        		if(s != NULL){
	        	   		printf("%s", s);
                   		free(s);
               		}
	        	}
                else {
                    avm_tostring(tbl->content);
                }

	        	if (tbl->next != NULL) printf(", ");
	        	else printf("}");
	        	tbl = tbl->next;
	        }
            s = NULL;
            break;
        }

        default: printf("cell->type : %d",cell->type); assert(0);
    }
    return s;
}



/* Stack functions */
// memory.h
void avm_dec_top(avm_memory * memory);

#endif