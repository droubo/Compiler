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
#ifndef PARSER_H
#define PARSER_H

#include "../avm/avm.h"
#include "../memory/memory.h"
#include <stdio.h>
#include <string.h>

char * read_string(FILE * file){
    char in1, in2;
    char * string;
    int i;
    in1 = fgetc(file);
    if(in1 != '\"')
        return NULL;
    
    in1 = fgetc(file);
    in2 = fgetc(file);
    string = (char *) malloc(sizeof(char));
    i = 0;
    while((in1 != '\"' || in2 != '\n') && in1 != EOF && in2 != EOF){
        string[i] = in1;
        string[i + 1] = in2;
        string = (char *) realloc(string, sizeof(char) * i + 2);
        i += 2 ;
        in1 = fgetc(file);
        in2 = fgetc(file);
    }
    string[i] = '\0';
    fgetc(file);
    return string;
}

double read_double(FILE * file){
    char * buf;
    int i;
    buf = (char *) malloc(sizeof(char) * 11);

    fgets(buf, 11, file);
    return atof(buf);
}

unsigned read_unsigned(FILE * file){
    char * buf;
    int i;
    char in;
    buf = (char *) malloc(sizeof(char) * 11);

    buf = malloc(sizeof(char) * 11);
	i = 0;
	in = fgetc(file);
	while(in != ' ' && in != EOF && in != '\n'){
		buf[i] = in;
		i++;
		in = fgetc(file);
	}
	buf[i] = '\0';
    return atoi(buf);
}

int do_magic(FILE * file){
    char * buf;
    buf = (char *) malloc(sizeof(char) * MAGICNUMBERSIZE);
    int i;
    for(i = 0; i < MAGICNUMBERSIZE; i++){
        buf[i] = fgetc(file);
        if(buf[i] == EOF)
            return 0;
    }
    fgetc(file);
    if(strcmp(buf, MAGICNUMBER) == 0)
        return 1;
    return 0;
}

void read_const_strings(FILE * file){
    int i;

    int size = read_unsigned(file);

    const_strings = init_avm_memcell_array(size, string_m);
    for(i = 0; i < size; i++){
        const_strings.array[i].data.strVal = read_string(file);
    }
}

void read_const_nums(FILE * file){
    int i;
    int size = read_unsigned(file);

    const_nums = init_avm_memcell_array(size, number_m);
    for(i = 0; i < size; i++) {
        const_nums.array[i].data.numVal = read_double(file);
    }
}

void read_user_funcs(FILE * file){
    int i;
    int size = read_unsigned(file);

    user_funcs = init_avm_memcell_array(size, userfunc_m);
    for(i = 0; i < size; i++){
        user_funcs.array[i].data.funcVal.address = read_unsigned(file) - 1;
        user_funcs.array[i].data.funcVal.locals = read_unsigned(file);
    }
}

void read_lib_funcs(FILE * file){
    int i;
    int size = read_unsigned(file);
    char * name;
    lib_funcs = init_avm_memcell_array(size, libfunc_m);
    for(i = 0; i < size; i++){
        lib_funcs.array[i].data.libfuncVal = read_string(file);
    }
}

void read_globals(FILE * file, avm_memory * memory){
    int i;
    int size = read_unsigned(file);

    globals = init_avm_memcell_array(size, undef_m);
    for(i = 0; i < size; i++){
        push_stack(memory, &(globals.array[i]));
    }
}

avm_opcode read_op(FILE * file){
    char * buf;
    char in;
    int i;
    buf = malloc(sizeof(char) * 20);

    in = fgetc(file);
    i = 0;
    while(in != ' ' && in != EOF){
        buf[i] = in;
        i++;
        in = fgetc(file);
    }
    buf[i] = '\0';
    if(strcmp(buf, "ASSIGN") == 0)      		return assign_v;

    else if(strcmp(buf, "ADD") == 0)    		return add_v;
    else if(strcmp(buf, "SUB") == 0)    		return sub_v;
    else if(strcmp(buf, "MUL") == 0)    		return mul_v;
    else if(strcmp(buf, "DIV") == 0)    		return div_v;
    else if(strcmp(buf, "MOD") == 0)    		return mod_v;

    else if(strcmp(buf, "JEQ") == 0)    		return jeq_v;
    else if(strcmp(buf, "JNE") == 0)    		return jne_v;
    else if(strcmp(buf, "JLE") == 0)    		return jle_v;
    else if(strcmp(buf, "JGE") == 0)    		return jge_v;
    else if(strcmp(buf, "JLT") == 0)    		return jlt_v;
    else if(strcmp(buf, "JGT") == 0)    		return jgt_v;
    else if(strcmp(buf, "JUMP") == 0)   		return jump_v;

    else if(strcmp(buf, "CALL") == 0)       	return call_v;
    else if(strcmp(buf, "PUSHARG") == 0)    	return pusharg_v;
    else if(strcmp(buf, "FUNCENTER") == 0)  	return funcenter_v;
    else if(strcmp(buf, "FUNCEXIT") == 0)    	return funcexit_v;

    else if(strcmp(buf, "NEWTABLE") == 0)    	return newtable_v;
    else if(strcmp(buf, "TABLEGETELEM") == 0)	return tablegetelem_v;
    else if(strcmp(buf, "TABLESETELEM") == 0)	return tablesetelem_v;
	else  		 								return nop_v;
	
}

vmarg read_vmarg(FILE * file){
	char * buf;
	int type;
	int i;
	char in;
	vmarg res;

	buf = malloc(sizeof(char) * 3);
	buf[0] = fgetc(file);
	buf[1] = fgetc(file);
	buf[2] = '\0';

	type = atoi(buf);
	assert(type <= 10);
	res.type = type;
	buf[0] = fgetc(file);

    assert(buf[0] == ':');
    if(type != retval_a)
	    res.val = read_unsigned(file);
    else {
        res.val = 0;
        fgetc(file);
    }
	return res;
}

void read_code(FILE * file, int * codeSize){
    int i;
    int size = read_unsigned(file);

    init_avm_instruction_array(size, codeSize);
    for(i = 0; i < size; i++){
        code[i].srcLine = read_unsigned(file);
        code[i].opcode = read_op(file);
        printf("    %d [%d] %d ", i,  code[i].srcLine, code[i].opcode);
        switch(code[i].opcode){
            case assign_v: {
                code[i].result = read_vmarg(file);
                code[i].arg1 = read_vmarg(file);
                printf("%d:%d %d:%d\n", code[i].result.type, code[i].result.type != retval_a? code[i].result.val : 0,
                                        code[i].arg1.type, code[i].arg1.val);
                break;
            }
            case add_v:
            case sub_v:
            case mul_v:
            case div_v:
            case mod_v: 
            case tablegetelem_v:
            case tablesetelem_v: {
                code[i].result = read_vmarg(file);
                code[i].arg1 = read_vmarg(file);
                code[i].arg2 = read_vmarg(file);
                printf("%d:%d %d:%d %d:%d\n", code[i].result.type, code[i].result.type != retval_a? code[i].result.val : 0,
                                        code[i].arg1.type, code[i].arg1.val,
                                        code[i].arg2.type, code[i].arg2.val);
                break;
            }
            case jeq_v:
            case jne_v:
            case jle_v:
            case jge_v:
            case jlt_v:
            case jgt_v: {
                code[i].arg1 = read_vmarg(file);
                code[i].arg2 = read_vmarg(file);
                code[i].result.val = read_unsigned(file) - 1;
                code[i].result.type = label_a;
            printf("%d:%d %d:%d %d\n", code[i].arg1.type, code[i].arg1.type != retval_a? code[i].arg1.val : 0,
                                        code[i].arg2.type, code[i].arg2.type != retval_a? code[i].arg2.val : 0,
                                        code[i].result.val);
                break;
            }

            case jump_v: {
                code[i].result.val = read_unsigned(file) - 1;
                code[i].result.type = label_a;
                printf("%d\n", code[i].result.val);
                break;
            }
            case call_v: 
            case pusharg_v: 
            case funcenter_v:
            case funcexit_v: 
            case newtable_v: {
                code[i].result = read_vmarg(file);
                printf("%d:%d\n", code[i].result.type, code[i].result.type != retval_a? code[i].result.val : 0);
                break;
            }
            case nop_v:{ break; }
        }
    }
}

#endif