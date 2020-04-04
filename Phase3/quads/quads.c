/* Some code Taken from lecture 9, Slides 37, 38 
   Frontisthrio 4 Slide 15 */
#include "symtab/symtable.h"

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
    mul, div, mod, 
    uminus, and, or, 
    not, if_eq, if_noteq, 
    if_lesseq, if_greatereq, if_less, 
    if_greater, call, param, ret, 
    getretval, funcstart, funcend, 
    tablecreate, tablegetelem, tablesetelem
} iopcode;

typedef struct expr {
    expr_t type;
    SymTabEntry * sym;
    struct expr * index;
    double numConst;
    char * strConst;
    unsigned char boolConst;
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

quad * quads = (quad *) 0;
unsigned total = 0;
unsigned int currQuad = 0;

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