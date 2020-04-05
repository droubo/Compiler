/* Some code Taken from lecture 9, Slides 37, 38 
   Frontisthrio 4 Slide 15 */
#include "quads.h"

void expand (void){
    assert(total == currQuad);
    quad * p = (quad *) malloc(NEW_SIZE);
    if (quads) {
        memcpy(p, quads, CURR_SIZE);
        free(quads);
    }
    quads = p;
    total += EXPAND_SIZE;
}

expr * newexpr(expr_t type, SymTabEntry* sym){
    expr * temp;
    temp = (expr*) malloc(sizeof(expr));
    temp->type = type;
    temp->sym = sym;
    return temp;
}

void emit (iopcode op, expr * arg1, expr * arg2, expr * result, unsigned label, unsigned line) {
    if(currQuad == total)
        expand();
    
    quad * p = quads + currQuad++;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = line;
}