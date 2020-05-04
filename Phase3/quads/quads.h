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
 * quads.h
 * Implementation of quad and expression data
 * structures, used to create intermediate code.
 */

#ifndef _QUADS_H
#define _QUADS_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../symtab/symtable.h"
#include "../booleanList/booleanList.h"
#include "../symtab/numPlaces.h"
#include "../statement/statement.h"

#define VAR_TRUE 1
#define VAR_FALSE 0

#define EXPAND_SIZE 1024
#define CURR_SIZE (total*sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE * sizeof(quad) + CURR_SIZE)

typedef enum expr_t {
    var_e,
    retval_e,
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
typedef struct call_struct{
	expr* elist;
	unsigned char method;
	char* name;
} call_struct;


expr * make_call(expr* lv, expr* elist, SymTable **table, int yyline, int currscope, int funcscope);

extern quad * quads;
extern unsigned total;
extern unsigned int currQuad;

void expand (void);

expr * newexpr(expr_t type, SymTabEntry* sym);

expr * newconstnumexpr(double num);

expr * newconstboolexpr(unsigned char bool);

expr * newconststringexpr(char * str);

expr* lvalue_expr(SymTabEntry* sym);

void emit (iopcode op, expr * arg1, expr * arg2, expr * result, unsigned line);

void emit_jump(iopcode op, expr * arg1, expr * arg2, unsigned int label, unsigned line);

void switch_quads(unsigned int label1, unsigned int label2);

struct expr *emit_iftableitem(expr* e, SymTable * table, int currScope, int func_scope, unsigned line);

expr * member_item(expr * lv, char* name, SymTable * table, int currScope, int func_scope, unsigned line);

void print_expr(expr * exp, int indent);

void print_quads(FILE * file);

void edit_quad(int index, expr * arg1, expr * arg2, expr * result, unsigned int label);


extern quad * quads;
#endif
