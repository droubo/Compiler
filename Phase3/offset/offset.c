#include "offset.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* some code taken from Lecture 9 , HY340 */

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

int pop()
{
    if(top != -1) return stack[top--];
    printf("Can't pop() : STACK EMPTY\n");
    assert(0);
}

