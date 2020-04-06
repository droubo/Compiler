/* Some code Taken from lecture 9, Slides 37, 38 
   Frontisthrio 4 Slide 15 */
#ifndef _QUADS_H
#define _QUADS_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../symtab/symtable.h"

#define EXPAND_SIZE 1024
#define CURR_SIZE (total*sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE * sizeof(quad) + CURR_SIZE)

typedef enum expr_t {
    var_e,
    tableitem_e,

    programfunc_e,
    libraryfunc_e,

    arithexpr_e,
    boolexpr_e,
    assignexpr_e,
    newtable_e,

    costnum_e, 
    constbool_e,
    conststring_e,

    nil_e
} expr_t;

typedef enum iopcode {
    assign, add, sub, 
    mul, diva, mod, 
    uminus, and, or, 
    not, if_eq, if_noteq, 
    if_lesseq, if_greatereq, if_less, 
    if_greater, call, param,
    ret, getretval, funcstart,
    funcend, tablecreate, tablegetelem, tablesetelem, jump
} iopcode;

typedef struct expr {
    expr_t type;
    SymTabEntry * sym;
    struct expr * index;
    double numConst;
    char * strConst;
    unsigned char boolConst;
    int const_type; /* -1 = symtab 0 = num 1 = str 2 = bool */
    struct expr * next;
} expr;

typedef struct quad {
    iopcode op;
    expr * result;
    expr * arg1;
    expr * arg2;
    unsigned label;
    unsigned line;
} quad;

void expand (void);

expr * newexpr(expr_t type, SymTabEntry* sym);

void emit (iopcode op, expr * arg1, expr * arg2, expr * result, unsigned label, unsigned line);
struct expr *emit_iftableitem(expr* e, SymTable * table, int currScope, int func_scope, int curr, unsigned label, unsigned line);
/* Always call with indent = 0 */
void print_expr(expr * exp, int indent);

void print_quads(FILE * file);

extern quad * quads;
#endif
