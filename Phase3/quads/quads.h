/* Some code Taken from lecture 9, Slides 37, 38 
   Frontisthrio 4 Slide 15 */
#ifndef _QUADS_H
#define _QUADS_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../symtab/symtable.h"
#include "../booleanList/booleanList.h"
#include "../symtab/numPlaces.h"

#define VAR_TRUE 1
#define VAR_FALSE 0

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

    constnum_e, 
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

typedef struct M {
    int quad;
} M;

typedef struct expr {
    expr_t type;
    SymTabEntry * sym;
    struct expr * index;
    double numConst;
    char * strConst;
    unsigned char boolConst;
    booleanList * truelist;
    booleanList * falselist;
    struct expr * next;
} expr;

typedef struct quad {
    iopcode op;
    expr * result;
    expr * arg1;
    expr * arg2;
    unsigned int label;
    unsigned int line;
} quad;

extern quad * quads;
extern unsigned total;
extern unsigned int currQuad;

void expand (void);

expr * newexpr(expr_t type, SymTabEntry* sym);

expr * newconstnumexpr(double num);

expr * newconstboolexpr(unsigned char bool);

expr * newconststringexpr(char * str);

void emit (iopcode op, expr * arg1, expr * arg2, expr * result, unsigned line);

void emit_jump(iopcode op, expr * arg1, expr * arg2, unsigned int label, unsigned line);

struct expr *emit_iftableitem(expr* e, SymTable * table, int currScope, int func_scope, int curr, unsigned line);

void print_expr(expr * exp, int indent);

void print_quads(FILE * file);

void edit_quad(int index, expr * arg1, expr * arg2, expr * result, unsigned int label);

extern quad * quads;
#endif
