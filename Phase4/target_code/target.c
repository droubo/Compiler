#include "target.h"

incomplete_jump* ij_head = (incomplete_jump*) 0;
unsigned int ij_total = 0;


char* print_vm_opcode(vmopcode opcode)
{
    switch(opcode)
    {
        case assign_v : return "assign";
        case add_v : return "add";
        case sub_v : return "sub";
        case mul_v : return "mul";
        case div_v : return "div";
        case mod_v : return "mod";
        case uminus_v :  return "uminus";
        case and_v : return "and";
        case or_v : return "or";
        case not_v : return "not";
        case jeq_v : return "jeq";
        case jne_v : return "jne";
        case jle_v : return "jle";
        case jge_v : return "jge";
        case jlt_v : return "jlt";
        case jgt_v : return "jgt";
        case call_v: return "call";
        case pusharg_v : return "pusharg";
        case funcenter_v : return "funcenter";
        case funcexit_v : return "funcexit";
        case newtable_v : "newtable";
        case tablegetelem_v : return "tablegetelem";
        case tablesetelem_v : return "table";
        case nop_v : return "nop";
        default : printf("opcode : %d\n");
    };
}
void make_operand(expr* e, vmarg* arg){

    if(e == NULL) return;

    switch(e->type) {
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
	case assignexpr_e:
        case newtable_e: {
            arg->val = e->sym->offset;
            switch(e->sym->space){
                case programvar: arg->type = global_a; break;
                case functionlocal: arg->type = local_a; break;
                case formalarg: arg->type = formal_a; break;
                default: assert(0);
            }
        }
        
        case constbool_e: {
            arg->val = e->boolConst;
            arg->type = bool_a; break;
        }


        case conststring_e: {
            arg->val = consts_newstring(e->strConst);
            arg->type = string_a; break;
        }

        case constnum_e: {
            arg->val = consts_newnumber(e->numConst);
            arg->type = number_a; break;
        }

        case nil_e: arg->type = nil_a; break;

        case programfunc_e: {
            arg->type = userfunc_a;
            arg->val = e->sym->taddress;
            break;
        }

        case libraryfunc_e: {
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(e->sym->name);
            break;
        }
        default: assert(0);
    }
}

/* generate */
void emit_target_code(instruction i)
{
    printf("LINE %d | vmopcode : %s | result : %d | arg1 : %d | arg2 : %d\n",i.srcLine,print_vm_opcode(i.opcode),i.result.val,i.arg1.val,i.arg2.val);
}


void generate_op(vmopcode op,quad *q, int flag)
{
    instruction t;
    t.opcode = op;
    make_operand(q->arg1, &t.arg1);
    if(flag == 0) make_operand(q->arg2, &t.arg2);
    make_operand(q->result, &t.result);
    t.srcLine = q->line;
    /*quad.taddress = nextinstructionlabel();*/
    emit_target_code(t); 
}

generate_relational(op, *quad) {
    instruction t;
    t.opcode = op;
    make_operand(quad->arg1, &t.arg1);
    make_operand(quad->arg2, &t.arg2);
    t.result.type = label_a;
    if quad.label jump target < currprocessedquad() then
    t.result.value = quads[quad->label]->taddress;
    else
    add_incomplete_jump(nextinstructionlabel(), quad->label);
    quad->taddress = nextinstructionlabel();
    emit(t);
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
    instruction t;
    t.opcode=nop;
    emit(t); 
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

generate_NOT (quad) {
    quad.taddress = nextinstructionlabel();
    instruction t;
    t.opcode = jeq;
    make_operand(quad.arg1, &t.arg1);
    make_booloperand(&t.arg2, false);
    t.result.type = label_a;
    t.result.value = nextinstructionlabel()+3;
    emit(t);
    t.opcode = assign;
    make_booloperand(&t.arg1, false);
    reset_operand(&t.arg2);
    make_operand(quad.result, &t.result);
    emit(t);
    t.opcode = jump;
    reset_operand (&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    t.result.value = nextinstructionlabel()+2;
    emit(t);
    t.opcode = assign;
    make_booloperand(&t.arg1, true);
    reset_operand(&t.arg2);
    make_operand(quad.result, &t.result);
    emit(t);
} 

generate_OR (quad) {
    quad.taddress = nextinstructionlabel();
    instruction t;
    t.opcode = jeq;
    make_operand(quad.arg1, &t.arg1);
    make_booloperand(&t.arg2, true);
    t.result.type = label_a;
    t.result.value = nextinstructionlabel()+4;
    emit(t);
    make_operand(quad.arg2, &t.arg1);
    t.result.value = nextinstructionlabel()+3;
    emit(t); 
    t.opcode = assign;
    make_booloperand(&t.arg1, false);
    reset_operand(&t.arg2);
    make_operand(quad.result, &t.result);
    emit(t);
    t.opcode = jump;
    reset_operand (&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    t.result.value = nextinstructionlabel()+2;
    emit(t);
    t.opcode = assign;
    make_booloperand(&t.arg1, true);
    reset_operand(&t.arg2);
    make_operand(quad.result, &t.result);
    emit(t);
} 

generate_PARAM(quad) {
    quad.taddress = nextinstructionlabel();
    instruction t;
    t.opcode = pusharg;
    make_operand(quad.arg1, &t.arg1);
    emit(t);
}
generate_CALL(quad) {
    quad.taddress = nextinstructionlabel();
    instruction t;
    t.opcode = callfunc;
    make_operand(quad.arg1, &t.arg1);
    emit(t);
}
generate_GETRETVAL(quad) {
    quad.taddress = nextinstructionlabel();
    instruction t;
    t.opcode = assign;
    make_operand(quad.result, &t.result);
    make_retvaloperand(&t.arg1);
    emit(t);
} 

void generate_FUNCSTART(quad *q)
{
    f = quad->result->sym;
    userfunctions.add(f->id, f->taddress, f->totallocals);
    push(funcstack, f);
    instruction t;
    t.opcode = enterfunc;
    make_operand(quad->result, &t.result);
    emit(t);
}

void generate_RETURN(quad *q)
{
    instruction t;
    t.opcode = assign_v;
    make_retvaloperand(&t.result);
    make_operand(quad->arg1, &t.arg1);
    emit(t);
    f = top(funcstack);
    append(f.retunList, nexrinstructionlabel());
    t.opcode = jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg1);
    t.result.type = label_a;
    emit(t);
}

void generate_FUNCEND(quad *q)
{
    f = pop(funcstack);
    backpatch(f.returnList, nexrinstructionlabel());
    quad->taddress = nextinstructionlabel();
    instruction t;
    t.opcode = exitfunc;
    make_operand(quad->result, &t.result);
    emit(t);
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
    generate_AND,
    generate_NOT,
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
};

/* generate instructions from quads */
void generate(void) {
    unsigned int i;
    for(i = 0; i < currQuad; ++i){
	//printf("I: %d\n\n\n", quads[i].op);
        (*generators[quads[i].op])((quads+i));
    }
}

unsigned consts_newstring(char *s)
{
    return 0;
}

unsigned consts_newnumber(double n)
{
    return (unsigned)n;
}

unsigned libfuncs_newused(const char *s)
{
    return 0;
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

