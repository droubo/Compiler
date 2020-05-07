/**
 * CS-340 Compilers
 * Spring 2020
 * 
 * Project Phase 3: Intermediate code Generation
 * 
 * Antonis Droubogiannis    csd4014@csd.uoc.gr
 * Georgios Marios Kokiadis csd3962@csd.uoc.gr
 * Nikolaos Gounakis        csd3932@csd.uoc.gr
 * 
 * temp.c
 * Temp expression manager
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "temp.h"
#include "../symtab/symtable.h"
#include "../symtab/numPlaces.h"
#define INT_MIN -2147483647
#define INT_MAX 2147483647

/* Used code from Lecture 9 Slide 45 */
int tempcounter = 0;
char * newtempname() {
    char * name = malloc(sizeof(char) * (numPlaces(tempcounter) + 1));
    sprintf(name, "^%d", tempcounter);
    //printf("tempname = %s\n", name);
    tempcounter++;
    return name;
}

void resettemp() { tempcounter = 0; }

SymTabEntry * newtemp(SymTable * table, int currScope, int func_scope) {
    char * name;
    SymTabEntry * sym;

    name = newtempname();
    sym = lookup_SymTableScope(table, currScope, name);
    if(sym == NULL)
        return insert_SymTable(table, new_SymTabEntry(name, -1, 1, NULL, NULL, currScope, func_scope, LOCAL));

    else
        return sym;
}