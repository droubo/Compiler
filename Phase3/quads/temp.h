#ifndef _TEMP_H
#define _TEMP_H
#include "../symtab/symtable.h"

#define INT_MIN -2147483647
#define INT_MAX 2147483647

extern int tempcounter;

/* Used code from Lecture 9 Slide 45 */
char * newtempname();

void resettemp();

SymTabEntry * newtemp(SymTable * table, int currScope, int func_scope, int curr);

#endif