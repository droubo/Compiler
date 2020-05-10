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
 * quads.c
 * Implementation of quad and expression data
 * structures, used to create intermediate code.
 */

/* Some code Taken from lecture 9, Slides 37, 38 
   Frontisthrio 4 Slide 15 */
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

void switch_quads(unsigned int label1, unsigned int label2){
    quad temp;

    temp = quads[label1];
    quads[label1] = quads[label2];
    quads[label2] = temp;
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

expr * newconstnullexpr(){
    expr * temp;
    temp = (expr*) malloc(sizeof(expr));
    temp->type = nil_e;
    temp->sym = NULL;
    temp->numConst = 0;
    temp->strConst = NULL;
    temp->boolConst = (unsigned char) 0;
    
    return temp;
}

expr* lvalue_expr(SymTabEntry* sym){
	assert(sym);
	expr* e = (expr*) malloc(sizeof(expr));
	memset(e,0,sizeof(expr));
	e->next = (expr*)0;
	e->sym = sym;
	switch(sym->type){
		case GLOBAL	: e->type = var_e; break;
		case LOCAL	: e->type = var_e; break;
		case USERFUNC 	: e->type = programfunc_e; break;
		case LIBFUNC 	: e->type = libraryfunc_e; break;
        case FORMAL : e->type = var_e; break;
		default: assert(0);
	}
	return e;
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

struct expr *emit_iftableitem(expr* e, SymTable * table, int currScope, int func_scope, unsigned line){
    if(e->type != tableitem_e) return e;
    else{
        expr* result = newexpr(var_e, newtemp(table,currScope,func_scope));
        emit(tablegetelem, e, e->index, result, line);
        return result;
    }
}

expr * member_item(expr * lv, char* name, SymTable * table, int currScope, int func_scope, unsigned line){
	lv = emit_iftableitem(lv, table, currScope, func_scope, line);
	expr* ti = newexpr(tableitem_e, lv->sym);
	ti->index = newconststringexpr(name);
	return ti;
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

expr * reverse_elist(expr * elist){
    expr * curr, * next, *prev;
    curr = elist;
    next = NULL;
    prev = NULL;

    while(curr != NULL){
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

expr * make_call(expr* lv, expr* elist, SymTable **table, int yyline, int currscope, int funcscope){
	expr* func = emit_iftableitem(lv, (*table), currscope, funcscope, yyline);
	expr * reversed_elist;
    reversed_elist = reverse_elist(elist);
    while(reversed_elist){
		emit(param, reversed_elist, NULL, NULL, yyline);
		reversed_elist = reversed_elist->next;
	}
	emit(call, func, NULL, NULL, yyline);
	expr* result = newexpr(retval_e, newtemp((*table), currscope, funcscope));
	emit(getretval, NULL, NULL, result, yyline);
	return result;

}

hashTableElement* newHashTableElement(expr* key, expr* value){
    hashTableElement* temp;
    temp = (hashTableElement*) malloc(sizeof(hashTableElement));
    temp->key = key;
    temp->value = value;
    temp->next = NULL;
    return temp;

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
            case retval_e:
            case tableitem_e:
            
            case programfunc_e:
            case libraryfunc_e:
             
            case arithexpr_e: 
            case newtable_e:    { fprintf(file, "%s ", arg->sym->name); break; }
            
            case boolexpr_e:    { fprintf(file, "boolexpr_e "); break; }
            case assignexpr_e:  { fprintf(file, "%s ", arg->sym->name); break; }


            case constnum_e:    { print_double(arg->numConst, file); break; }
            case constbool_e:   { if(arg->boolConst == VAR_TRUE) fprintf(file, "'true' "); else fprintf(file, "'false' "); break; }
            case conststring_e: { fprintf(file, "\"%s\" ", arg->strConst); break; }

            case nil_e:         { fprintf(file, "nil "); break; }

            default:            { fprintf(file, "*error* "); break; }
        }
    
}

void print_quads(FILE * file, int max_lines){
    int i, j;
    printf("\n\033[1;32m>> \033[01;33mI CODE\n\n\033[0m");
    quad curr_quad;
    for(i = 0; i < currQuad; i++){
        curr_quad = quads[i];
    
        if(file == stdout){
            fprintf(file, "\033[0;36m[line %d]\033[0m", quads[i].line, i + 1);

            for(j = 0; j < numPlaces(max_lines) - numPlaces(quads[i].line) + 2 ; j++){
                fprintf(file, " ");
            }

            fprintf(file, "\033[0;32m%d\033[0m", i + 1);
        
            for(j = 0; j < numPlaces(currQuad) - numPlaces(i + 1) + 2 ; j++){
                fprintf(file, " ");
            }
        }
        printf("\033[0;33m");
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
        printf("\033[0m");
        switch(curr_quad.op){
            case if_eq:
            case if_noteq:
            case if_lesseq:
            case if_greatereq:
            case if_less:
            case if_greater: {
                    print_quad_arg(curr_quad.arg1, file);
                    print_quad_arg(curr_quad.arg2, file);
                    printf("\033[0;32m");
                    fprintf(file, "%d", curr_quad.label);
                    printf("\033[0m");
                    break;
            }
            case jump: {    
                    printf("\033[0;32m"); 
                    fprintf(file, "%d", curr_quad.label);
                    printf("\033[0m");
                    break; }
            default: {
                print_quad_arg(curr_quad.result, file);
                printf("\033[0;35m");
		        print_quad_arg(curr_quad.arg1, file);
                print_quad_arg(curr_quad.arg2, file);
                printf("\033[0m");
                break;
            }
        }
        fprintf(file, "\n");
    }
    printf("\n\033[1;32m>>> \033[01;33mEND\n\033[0m");
}