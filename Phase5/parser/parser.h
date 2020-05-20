#ifndef PARSER_H
#define PARSER_H

#include "../avm/avm.h"
#include <stdio.h>
#include <string.h>

char * read_string(FILE * file){
    char in;
    char * string;
    int i;
    in = fgetc(file);
    if(in != '\"')
        return NULL;
    
    in = fgetc(file);
    string = (char *) malloc(sizeof(char));
    i = 0;
    while(in != '\"' && in != EOF){
        string[i] = in;
        string = (char *) realloc(string, sizeof(char) * i + 2);
        i++;
        in = fgetc(file);
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
    buf = (char *) malloc(sizeof(char) * 11);

    fgets(buf, 11, file);
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

    const_strings = init_memcell_array(size, string_m);
    for(i = 0; i < size; i++){
        const_strings.array[i].data.strVal = read_string(file);
    }
}

void read_const_nums(FILE * file){
    int i;
    int size = read_unsigned(file);

    const_nums = init_memcell_array(size, number_m);
    for(i = 0; i < size; i++) {
        const_nums.array[i].data.numVal = read_double(file);
    }
}

void read_user_funcs(FILE * file){
    int i;
    int size = read_unsigned(file);

    user_funcs = init_memcell_array(size, userfunc_m);
    for(i = 0; i < size; i++){
        // Addr
        user_funcs.array[i].data.funcVal.address = read_unsigned(file);
        // Localsize
        user_funcs.array[i].data.funcVal.locals = read_unsigned(file);
        // ID
        user_funcs.array[i].data.funcVal.ID = read_string(file);
    }
}

void read_lib_funcs(FILE * file){
    int i;
    int size = read_unsigned(file);

    lib_funcs = init_memcell_array(size, libfunc_m);
    for(i = 0; i < size; i++){
        lib_funcs.array[i].data.libfuncVal = read_string(file);
    }
}

void read_globals(FILE * file){
    int i;
    int size = read_unsigned(file);

    globals = init_memcell_array(size, number_m);
    for(i = 0; i < size; i++){
        globals.array[i].data.strVal = read_string(file);
    }
}

#endif