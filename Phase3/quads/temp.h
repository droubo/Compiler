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
 * temp.h
 * Temp expression manager
 */

#ifndef _TEMP_H
#define _TEMP_H
#include "../symtab/symtable.h"

#define INT_MIN -2147483647
#define INT_MAX 2147483647

extern int tempcounter;

/* Used code from Lecture 9 Slide 45 */
char * newtempname();

void resettemp();

SymTabEntry * newtemp(SymTable * table, int currScope, int func_scope);

#endif