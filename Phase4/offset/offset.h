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
extern int top;

void push(int data);
int pop();
/*--------------------------*/

/* scopespace_t : programvar */
extern unsigned int programVarOffset;
/* scopespace_t : functionlocal */
extern unsigned int functionLocalOffset;
/* scopespace_t : formalarg */
extern unsigned int  formalArgOffset;

extern unsigned int scopeSpaceCounter;

unsigned int currscopeoffset();

unsigned int getscopeSpaceCounter();

void inccurrscopeoffset();

void resetformalargsoffset();

void resetfunctionlocaloffset();

void restorecurroffset(unsigned int n);

void printNumOfVars();

SymbolType currscopespace();

void enterscopespace();

void exitscopespace();


#endif