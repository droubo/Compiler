/* Some code Taken from lecture 9, Slides 37, 38 
   Frontisthrio 4 Slide 15 */
#include "quads.h"

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
    p->op = op;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = line;
}

void print_expr(expr * exp, int indent){
    int i;
    printf("EXPRESSION @%p {\n", exp);

    printf("type = %p\nindex (pointer) = %p\n", exp->type, exp->index);
    printf("sym = \n");
    print_SymTabEntry(*(exp->sym));
    printf("numConst = %d\n", exp->numConst);

    if(exp->strConst != NULL){
        printf("strConst = %s", exp->strConst);
    } else
        printf("strConst = (nil)\n");
    
    printf("boolConst = %d\n", exp->boolConst);
    printf("next (pointer) = %p\n", exp->next);

}

void print_quads(FILE * file){
    int i;
    quad * curr_quad;
    for(i = 0; i < currQuad; i++){
        curr_quad = quads + i;
        
        switch(curr_quad->op){
            case assign:    { fprintf(file, "ASSIGN"); break;}
            case add:       { fprintf(file, "ADD"); break; }
            case sub:       { fprintf(file, "SUB"); break; }

            case mul:       { fprintf(file, "MUL"); break; }
            case diva:      { fprintf(file, "DIV"); break; }
            case mod:       { fprintf(file, "MOD"); break; }

            case uminus:    { fprintf(file, "ASSIGN"); break; }
            case and:    	{ fprintf(file, "ASSIGN"); break; }
            case or:    	{ fprintf(file, "ASSIGN"); break; }

			case not:    { fprintf(file, "ASSIGN"); break; }
            case if_eq:    	{ fprintf(file, "ASSIGN"); break; }
            case if_noteq:    	{ fprintf(file, "ASSIGN"); break; }
			
			case if_lesseq:    { fprintf(file, "ASSIGN"); break; }
            case if_greatereq:    	{ fprintf(file, "ASSIGN"); break; }
            case :    	{ fprintf(file, "ASSIGN"); break; }
			
			case uminus:    { fprintf(file, "ASSIGN"); break; }
            case and:    	{ fprintf(file, "ASSIGN"); break; }
            case or:    	{ fprintf(file, "ASSIGN"); break; }
			
			case uminus:    { fprintf(file, "ASSIGN"); break; }
            case and:    	{ fprintf(file, "ASSIGN"); break; }
            case or:    	{ fprintf(file, "ASSIGN"); break; }

        
        }


    }
}

int main(){
    emit(assign, NULL, NULL, NULL, 1, 1);
    emit(add, NULL, NULL, NULL, 2, 1);
    emit(assign, NULL, NULL, NULL, 3, 1);
    emit(assign, NULL, NULL, NULL, 4, 1);
    print_quads(stdout);
}