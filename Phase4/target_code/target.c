#include "target.h"
#include "target_emit.h"

#include <string.h>
#include <stdio.h>
#define true 1
#define false 0

StringStack *StringHead=NULL, *StringTail=NULL;
NumStack *NumHead=NULL, *NumTail=NULL;
FunctionStack *FunHead=NULL, *FunTail=NULL;
StringStack *LibHead=NULL, *LibTail=NULL;
StringStack *GlobalHead = NULL, *GlobalTail = NULL;
int globals = 0;

instruction* make_new_instruction()
{
    instruction *t = (instruction*) malloc(sizeof(instruction));
    t->arg1 = malloc(sizeof(vmarg));
    t->arg2 = malloc(sizeof(vmarg));
    t->result = malloc(sizeof(vmarg));

    return t;
}

char* consts_newstring(char *s)
{
    char *tmp = malloc(strlen(s));
    strcat(tmp, s);
    //free(s);
    return tmp;
}

double consts_newnumber(double n)
{
    return n;
}

const char* libfuncs_newused(const char *s)
{
    char *tmp = malloc(strlen(s));
    strcat(tmp, s);
    //free(s);
    return tmp;
}


void make_operand(expr* e, vmarg* arg){

    if(e == NULL) return;

    switch(e->type) {
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
	    case assignexpr_e:
        case retval_e:
        case newtable_e: {
            arg->num_val = e->sym->offset;
            arg->str_val = strdup(e->sym->name);
            switch(e->sym->space){
                case programvar:
                {   
                    arg->type = global_a;
                    if(e->sym->offset > globals)
                    {
                        globals = e->sym->offset;
                    }  
                    break;
                }
                case functionlocal: arg->type = local_a; break;
                case formalarg: arg->type = formal_a; break;
                default: assert(0);
            }
            break;
        }
        
        case constbool_e: {
            arg->bool_val = e->boolConst;
            arg->str_val = NULL;
            arg->type = bool_a; break;
        }


        case conststring_e: {
            arg->str_val = consts_newstring(e->strConst);
            pushStringStack(&StringHead, &StringTail, arg->str_val);
            arg->type = string_a; break;
        }

        case constnum_e: {
            arg->num_val = consts_newnumber(e->numConst);
            pushNumStack(&NumHead, &NumTail, arg->num_val);
            arg->str_val = NULL;
            arg->type = number_a; break;
        }

        case nil_e: arg->type = nil_a; break;

        case programfunc_e: {
            arg->type = userfunc_a;
            arg->num_val = e->sym->value.funcVal->iaddress;
            arg->str_val = strdup(e->sym->name);
            pushFunctionStack(&FunHead, &FunTail, arg->str_val, e->sym->value.funcVal->iaddress, e->sym->value.funcVal->num_of_locals);
            break;
        }

        case libraryfunc_e: {
            arg->type = libfunc_a;
            arg->str_val = strdup(libfuncs_newused(e->sym->name));
            pushStringStack(&LibHead, &LibTail, arg->str_val);
            break;
        }
        default: assert(0);
    }
}
void make_numberoperand(vmarg *arg, double val){
    arg->num_val = val;
    arg->type = number_a;
}
void make_booloperand(vmarg* arg, unsigned int val){
    arg->bool_val = val;
    arg->type = bool_a;
}
void make_retvaloperand(vmarg *arg){
    arg->type = retval_a;
}

void generate_op(avm_opcode op,quad *q, int flag)
{
    instruction *t = make_new_instruction();
    t->opcode = op;
    make_operand(q->arg1, t->arg1);
    if(flag == 0) make_operand(q->arg2, t->arg2);
    make_operand(q->result, t->result);
    t->srcLine = q->line;
    /*quad->taddress = nextinstructionlabel();*/
    emit_instruction(t); 
}

void generate_relational(avm_opcode op, quad* q) {
    instruction *t = make_new_instruction();
    t->opcode = op;
    make_operand(q->arg1, t->arg1);
    make_operand(q->arg2, t->arg2);
    t->result->type = label_a;
    t->srcLine = q->line;
    t->label = q->label;
    /*
    if quad->label jump target < currprocessedquad() then
    t.result.value = quads[quad->label]->taddress;
    else
    add_incomplete_jump(nextinstructionlabel(), quad->label);
    //quad->taddress = nextinstructionlabel();
    */
    emit_instruction(t);
} 

void generate_ADD(quad *q)
{
    generate_op(add_v,q, 0);
}

void generate_SUB(quad *q)
{
    generate_op(sub_v,q, 0);
}

void generate_MUL(quad *q)
{
    generate_op(mul_v,q, 0);
}

void generate_DIV(quad *q)
{
    generate_op(div_v,q, 0);
}

void generate_MOD(quad *q)
{
    generate_op(mod_v,q, 0);
}

void generate_NEWTABLE(quad *q)
{
    generate_op(newtable_v,q, 0);
}

void generate_TABLEGETELEM(quad *q)
{
     generate_op(tablegetelem_v,q, 0);
}

void generate_TABLESETELEM(quad *q)
{
    generate_op(tablesetelem_v,q, 0);
}

void generate_ASSIGN(quad *q)
{
    generate_op(assign_v,q, 1);
}

void generate_NOP(quad *q)
{
    /*
    instruction t;
    t.opcode=nop_v;
    t.srcLine = q->line;
    emit_instruction(&t);
    */ 
}

void generate_JUMP(quad *q)
{
    generate_relational(jump_v, q);
}

void generate_IF_EQ(quad *q)
{
    generate_relational(jeq_v, q);
}

void generate_IF_NOTEQ(quad *q)
{
    generate_relational(jne_v, q);
}

void generate_IF_GREATER(quad *q)
{
    generate_relational(jgt_v, q);
}

void generate_IF_GREATEREQ(quad *q)
{
    generate_relational(jge_v, q);
}

void generate_IF_LESS(quad *q)
{
    generate_relational(jlt_v, q);
}

void generate_IF_LESSEQ(quad *q)
{
    generate_relational(jle_v, q);
}

void generate_NOT (quad *q) {
    /*
    //quad->taddress = nextinstructionlabel();
    instruction t;
    t.opcode = jeq_v;
    make_operand(q->arg1, &t.arg1);
    make_booloperand(&t.arg2, false);
    t.result.type = label_a;
    //t.result.value = nextinstructionlabel()+3;
    emit_instruction(&t);
    t.opcode = assign;
    make_booloperand(&t.arg1, false);
    //reset_operand(&t.arg2);
    make_operand(q->result, &t.result);
    emit_instruction(&t);
    t.opcode = jump;
    //reset_operand (&t.arg1);
    //reset_operand(&t.arg2);
    t.result.type = label_a;
    //t.result.value = nextinstructionlabel()+2;
    emit_instruction(&t);
    t.opcode = assign_v;
    make_booloperand(&t.arg1, true);
    //reset_operand(&t.arg2);
    make_operand(q->result, &t.result);
    emit_instruction(&t);
    */
} 

void generate_OR (quad *q) {
    /*
    //quad->taddress = nextinstructionlabel();
    instruction t;
    t.opcode = jeq_v;
    make_operand(q->arg1, &t.arg1);
    make_booloperand(&t.arg2, true);
    t.result.type = label_a;
    //t.result.value = nextinstructionlabel()+4;
    emit_instruction(&t);
    make_operand(q->arg2, &t.arg1);
    //t.result.value = nextinstructionlabel()+3;
    emit_instruction(&t); 
    t.opcode = assign;
    make_booloperand(&t.arg1, false);
    //reset_operand(&t.arg2);
    make_operand(q->result, &t.result);
    emit_instruction(&t);
    t.opcode = jump;
    //reset_operand (&t.arg1);
    //reset_operand(&t.arg2);
    t.result.type = label_a;
    //t.result.value = nextinstructionlabel()+2;
    emit_instruction(&t);
    t.opcode = assign;
    make_booloperand(&t.arg1, true);
    //reset_operand(&t.arg2);
    make_operand(q->result, &t.result);
    emit_instruction(&t);
    */
} 

void generate_PARAM(quad *q) {
    //quad->taddress = nextinstructionlabel();
    instruction *t = make_new_instruction();
    t->opcode = pusharg_v;
    make_operand(q->arg1, t->arg1);
    t->srcLine = q->line;
    emit_instruction(t);
}
void generate_CALL(quad *q) {
    //quad->taddress = nextinstructionlabel();
    instruction *t = make_new_instruction();
    t->opcode = call_v;
    make_operand(q->arg1, t->arg1);
    t->srcLine = q->line;
    emit_instruction(t);
}
void generate_GETRETVAL(quad *q) {
    //quad->taddress = nextinstructionlabel();
    instruction *t = make_new_instruction();
    t->opcode = assign_v;
    make_operand(q->result, t->arg1);
    make_retvaloperand(t->result);
    t->result->is_getretval = 1;
    t->srcLine = q->line;
    emit_instruction(t);
} 

void generate_FUNCSTART(quad *q)
{
    //SymTabEntry *f = quad->result->sym;
    //userfunctions.add(f->id, f->taddress, f->totallocals);
    //push(funcstack, f);
    instruction *t = make_new_instruction();
    t->opcode = funcenter_v;
    make_operand(q->result, t->result);
    t->srcLine = q->line;
    emit_instruction(t);
}

void generate_RETURN(quad *q)
{
    instruction *t = make_new_instruction();
    instruction *t2 = make_new_instruction();
    t->opcode = assign_v;
    make_retvaloperand(t->result);
    make_operand(q->result, t->arg1);
    emit_instruction(t);
    //SymTabEntry f = top(funcstack);
    //append(f.retunList, nexrinstructionlabel());
    //t2->opcode = jump_v;
    //reset_operand(&t.arg1);
    //reset_operand(&t.arg1);
    //t2->result->type = label_a;
    //t2->srcLine = q->line;
    //emit_instruction(t2);
}

void generate_FUNCEND(quad *q)
{
    //SymTabEntry f = pop(funcstack);
    //backpatch(f.returnList, nexrinstructionlabel());
    //quad->taddress = nextinstructionlabel();
    instruction* t = make_new_instruction();
    t->opcode = funcexit_v;
    make_operand(q->result, t->result);
    t->srcLine = q->line;
    emit_instruction(t);
}
void generate_UMINUS(quad *q){
    expr *tmp = (expr *)malloc(sizeof(expr));
    tmp->type = constnum_e;
    tmp->numConst = -1;
    q->arg2 = tmp;
    generate_MUL(q);
}

void generate_AND(quad *q)
{

}
/* the order of the array must be equal to the enum that represents the i-code instruction*/
typedef void (*generator_func_t)(quad*);

generator_func_t generators[] = {
    generate_ASSIGN,
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_UMINUS,
    generate_AND,
    generate_OR,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_LESSEQ,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_GREATER,
    generate_CALL,
    generate_PARAM,
    generate_GETRETVAL,
    generate_RETURN,
    generate_FUNCSTART,
    generate_FUNCEND,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_JUMP,
    generate_NOT
};

/* generate instructions from quads */
void generate(void) {
    unsigned int i;
    for(i = 0; i < currQuad; ++i){
        (*generators[quads[i].op])((quads+i));
    }/*
    print_LibraryStack(LibHead);
    print_StringStack(StringHead);
    print_NumStack(NumHead);
    print_FuncStack(FunHead);*/
}



/*

patch_incomplete_jumps() 
{
    for each incomplete jump x do 
    {
        if x.iaddress = intermediate code size then
            instructions[x.instrNo].result = target code size;
        else
            instructions[x.instrNo].result = quads[x.iaddress].taddress;
    }
}

*/

