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
 * offset.c
 * Scope offset tracking implementation
 */ 

#include "offset.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* some code taken from Lecture 9 , HY340 */

void printNumOfVars()
{
    printf("\nprogramVarOffset : %d\n",programVarOffset);
}

unsigned int getscopeSpaceCounter()
{
    return scopeSpaceCounter;
}

SymbolType  currscopespace()
{
    if(scopeSpaceCounter == 1)
    return programvar;
    else if(scopeSpaceCounter % 2 == 0)
    return formalarg;
    else 
    return functionlocal;
}

unsigned int currscopeoffset()
{
    switch(currscopespace())
    {
        case programvar : return programVarOffset;
        case functionlocal : return functionLocalOffset;
        case formalarg : return formalArgOffset;
        default : assert(0);
    }
}

void inccurrscopeoffset ()
{
    switch(currscopespace())
    {
        case programvar : programVarOffset++; break;
        case functionlocal : functionLocalOffset++; break;
        case formalarg : formalArgOffset++; break;
        default : assert(0);
    }
}

void enterscopespace()
{
    scopeSpaceCounter++;
}

void exitscopespace()
{
    assert(scopeSpaceCounter>1);
    scopeSpaceCounter--;
}

void resetformalargsoffset()
{
    formalArgOffset = 0;
}

void resetfunctionlocaloffset()
{
    functionLocalOffset = 0;
}

void restorecurroffset(unsigned int n)
{
    switch(currscopespace())
    {
        case programvar : programVarOffset = n; break;
        case functionlocal : functionLocalOffset = n; break;
        case formalarg : formalArgOffset = n; break;
        default : assert(0);
    }   
}

/*-------- stack---------- */

void push(int data)
{
    if(top == -1)
    {
        stack = malloc(sizeof(int));
    }
    else
    {
        /* top + 1 = current size */
        stack = realloc(stack,sizeof(int)* ( (top + 1)+1) );
    }
    stack[++top] = data;
}

/* need fix when pop , realloc fewer memory */
int pop()
{
    if(top != -1)
    { 
        int tmp = stack[top];
        /* realloc , -1 int */
        stack = realloc(stack,sizeof(int)* top--);
        return tmp;
    }
    printf("Can't pop() : STACK EMPTY\n");
    assert(0);
}

