/* Taken from lecture 9, Slides 37, 38 */
#define EXPAND_SIZE 1024
#define CURR_SIZE (total*sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE * sizeof(quad) + CURR_SIZE)

enum iopcopde {
    assign, add, sub, mul, div, mod, uminus, and, or, not, 
    if_eq, if_noteq, if_lesseq, if_greatereq, if_less, if_greater, 
    call, param, ret, getretval, funcstart, funcend, tablecreate, 
    tablegetelem, tablesetelem
};

struct expr;

typedef struct quad {
    iopcopde op;
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
    if(quad == total)
        expand();
    
    quad * p = quads + currQuad++;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = line;
}