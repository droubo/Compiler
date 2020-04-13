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
 * offset.h
 * Scope offset tracking implementation
 */ 

#ifndef _OFFSET_H
#define _OFFSET_H

#include "../symtab/symtable_types.h"

/* some stack implementation to save offsets */
int* stack;
int top = -1;

void push(int data);
int pop();
/*--------------------------*/

/* scopespace_t : programvar */
unsigned int programVarOffset = 0;
/* scopespace_t : functionlocal */
unsigned int functionLocalOffset = 0;
/* scopespace_t : formalarg */
unsigned int  formalArgOffset = 0;

unsigned int scopeSpaceCounter = 1;

unsigned int currscopeoffset();

unsigned int getscopeSpaceCounter();

void inccurrscopeoffset();

void resetformalargsoffset();

void resetfunctionlocaloffset();

void restorecurroffset(unsigned int n);

void printNumOfVars();


#endif