#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../symtab/symtable.h"

#define INT_MIN -2147483647
#define INT_MAX 2147483647

int numPlaces (int n) {
    if (n < 0) return numPlaces ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
}

/* Used code from Lecture 9 Slide 45 */
int tempcounter = 0;
char * newtempname() {
    char * name = malloc(sizeof(char) * (numPlaces(tempcounter) + 2));
    sprintf("_t%d", name);
    return name;
}

void resettemp() { tempcounter = 0; }

SymTabEntry * newtemp(SymTable * table, int currScope, int func_scope, int curr) {
    char * name;
    SymTabEntry * sym;

    name = newtempname();
    sym = lookup_SymTableScope(table, currScope, name);
    if(sym == NULL){
        insert_SymTable(table, new_SymTabEntry(name, -1, 1, NULL, NULL, currScope, func_scope, LOCAL));
        return lookup_SymTable(table, name);
    }
    else
        return sym;
    
}