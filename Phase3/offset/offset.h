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

unsigned currscopeoffset();

void inccurrscopeoffset();

#endif