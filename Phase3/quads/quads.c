#include "quads.h"
#include "temp.h"
#include <math.h>
#include "../symtab/numPlaces.h"

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
    temp->numConst = 0;
    temp->strConst = NULL;
    temp->boolConst = (unsigned char) 0;
    temp->index = NULL;
    return temp;
}

expr * newconstnumexpr(double num) {
    expr * temp;
    temp = (expr*) malloc(sizeof(expr));
    temp->type = constnum_e;
    temp->sym = NULL;
    temp->numConst = num;
    temp->strConst = NULL;
    temp->boolConst = (unsigned char) 0;
    
    return temp;
}

expr * newconstboolexpr(unsigned char bool){
    expr * temp;
    temp = (expr*) malloc(sizeof(expr));
    temp->type = constbool_e;
    temp->sym = NULL;
    temp->numConst = 0;
    temp->strConst = NULL;
    temp->boolConst = bool;
    
    return temp;
}

expr * newconststringexpr(char * str){
    expr * temp;
    temp = (expr*) malloc(sizeof(expr));
    temp->type = conststring_e;
    temp->sym = NULL;
    temp->numConst = 0;
    temp->strConst = str;
    temp->boolConst = (unsigned char) 0;
    
    return temp;
}

void emit (iopcode op, expr * arg1, expr * arg2, expr * result, unsigned line) {
    if(currQuad == total)
        expand();
    
    quad * p = quads + currQuad++;
    p->op = op;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->line = line;
}

void emit_jump (iopcode op, expr * arg1, expr * arg2, unsigned int label, unsigned int line) {
    if(currQuad == total)
        expand();
    
    quad * p = quads + currQuad++;
    p->op = op;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = NULL;
    p->label = label;
    p->line = line;
}

struct expr *emit_iftableitem(expr* e, SymTable * table, int currScope, int func_scope, int curr, unsigned line){
    if(e->type != tableitem_e) return e;
    else{
        expr* result = newexpr(var_e, newtemp(table,currScope,func_scope,curr));
        emit(tablegetelem, e, e->index, result, line);
        return result;
    }
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

void edit_quad(int index, expr * arg1, expr * arg2, expr * result, unsigned int label){
    quad * curr_quad;
    
    curr_quad = quads + index;

    if(arg1 != NULL)
        curr_quad->arg1 = arg1;
    
    if(arg2 != NULL)
        curr_quad->arg2 = arg2;
    
    if(result != NULL)
        curr_quad->result = result;

    if(label != -1)
        curr_quad->label = label;

}

void print_double(double d, FILE * file){
    if((ceilf(d) == d && floor(d) == d) || d == 0.0)
        fprintf(file, "%.0f ", d);
    else
        fprintf(file, "%f ", (double) d);
}

void print_quad_arg(expr * arg, FILE * file){
    if(arg != NULL)
        switch(arg->type){
            case var_e:
            case tableitem_e:
            
            case programfunc_e:
            case libraryfunc_e:
             
            case arithexpr_e: 
            case newtable_e:    { fprintf(file, "%s ", arg->sym->name); break; }
            
            case boolexpr_e:    { fprintf(file, "boolexpr_e "); break; }
            case assignexpr_e:  { fprintf(file, "assinexpr_e "); break; }


            case constnum_e:    { print_double(arg->numConst, file); break; }
            case constbool_e:   { if(arg->boolConst == VAR_TRUE) fprintf(file, "TRUE "); else fprintf(file, "FALSE "); break; }
            case conststring_e: { fprintf(file, "%s ", arg->strConst); break; }

            case nil_e:         { fprintf(file, "(nil) "); break; }

            default:            { fprintf(file, "*error* "); break; }
        }
    
}

void print_quads(FILE * file){
    int i, j;
    printf("\n>> I CODE\n\n");
    quad curr_quad;
    for(i = 0; i < currQuad; i++){
        curr_quad = quads[i];
    
        if(file == stdout){
            fprintf(file, "%d", i + 1);
        
            for(j = 0; j < numPlaces(currQuad) - numPlaces(i + 1) + 2; j++){
                fprintf(file, " ");
            }
        }

        switch(curr_quad.op){
            case assign:    	{ fprintf(file, "ASSIGN "); break;}
            case add:       	{ fprintf(file, "ADD "); break; }
            case sub:       	{ fprintf(file, "SUB "); break; }

            case mul:       	{ fprintf(file, "MUL "); break; }
            case diva:      	{ fprintf(file, "DIV "); break; }
            case mod:       	{ fprintf(file, "MOD "); break; }

            case uminus:    	{ fprintf(file, "UMINUS "); break; }
            case and:    		{ fprintf(file, "AND "); break; }
            case or:    		{ fprintf(file, "OR "); break; }

			case not:    		{ fprintf(file, "NOT "); break; }
            case if_eq:    		{ fprintf(file, "IF_EQ "); break; }
            case if_noteq:  	{ fprintf(file, "IF_NOTEQ "); break; }

			case if_lesseq: 	{ fprintf(file, "IF_LESSEQ "); break; }
            case if_greatereq:	{ fprintf(file, "IF_GREATEREQ "); break; }
            case if_less:    	{ fprintf(file, "IF_LESS "); break; }
			
			case if_greater:    { fprintf(file, "IF_GREATER "); break; }
            case call:    		{ fprintf(file, "CALL "); break; }
            case param:    		{ fprintf(file, "PARAM "); break; }
			
			case ret:    		{ fprintf(file, "RET "); break; }
            case getretval:    	{ fprintf(file, "GETRETVAL "); break; }
            case funcstart:    	{ fprintf(file, "FUNCSTART "); break; }

			case funcend:    	{ fprintf(file, "FUNCEND "); break; }
            case tablecreate:   { fprintf(file, "TABLECREATE "); break; }
            case tablegetelem:  { fprintf(file, "TABLEGETELEM "); break; }
			case tablesetelem:  { fprintf(file, "TABLESETELEM "); break; }
            case jump:          { fprintf(file, "JUMP "); break; }

            default: { fprintf(file, "*ERROR* "); break; }
        }
        switch(curr_quad.op){
            case if_eq:
            case if_noteq:
            case if_lesseq:
            case if_greatereq:
            case if_less:
            case if_greater: {
                    print_quad_arg(curr_quad.arg1, file);
                    print_quad_arg(curr_quad.arg2, file);
                    fprintf(file, "%d", curr_quad.label);
                    break;
            }
            case jump: { fprintf(file, "%d", curr_quad.label); break; }
            default: {
                print_quad_arg(curr_quad.result, file);
		        print_quad_arg(curr_quad.arg1, file);
                print_quad_arg(curr_quad.arg2, file);
                break;
            }
        }
        fprintf(file, "\n");
    }
    printf("\n>> END\n");
}