#include "target.h"

incomplete_jump* ij_head = (incomplete_jump*) 0;
unsigned int ij_total = 0;

/* generate */
void emit_target_code(instruction i)
{
    printf("LINE %d | vmopcode : %d | result : %d | arg1 : %d | arg2 : %d\n",i.srcLine,i.opcode,i.result.val,i.arg1.val,i.arg2.val);
}


void generate_op(vmopcode op,quad q)
{
    instruction t;
    t.opcode = op;
    make_operand(q.arg1, &t.arg1);
    make_operand(q.arg2, &t.arg2);
    make_operand(q.result, &t.result);
    /*quad.taddress = nextinstructionlabel();*/
    emit_target_code(t); 
}

void generate_ADD(quad q)
{
    generate_op(add_v,q);
}

void generate_SUB(quad q)
{

}

void generate_MUL(quad q)
{

}

void generate_DIV(quad q)
{

}

void generate_MOD(quad q)
{

}

void generate_NEWTABLE(quad q)
{

}

void generate_TABLEGETELEM(quad q)
{
     
}

void generate_TABLESETELEM(quad q)
{

}

void generate_ASSIGN(quad q)
{

}

void generate_NOP(quad q)
{

}

void generate_JUMP(quad q)
{

}

void generate_IF_EQ(quad q)
{

}

void generate_IF_NOTEQ(quad q)
{

}

void generate_IF_GREATER(quad q)
{

}

void generate_IF_GREATEREQ(quad q)
{

}

void generate_IF_LESS(quad q)
{

}

void generate_IF_LESSEQ(quad q)
{

}

void generate_NOT(quad q)
{

}

void generate_OR(quad q)
{

}

void generate_PARAM(quad q)
{

}

void generate_CALL(quad q)
{

}

void generate_GETRETVAL(quad q)
{

}

void generate_FUNCSTART(quad q)
{

}

void generate_RETURN(quad q)
{

}

void generate_FUNCEND(quad q)
{

}

/* the order of the array must be equal to the enum that represents the i-code instruction*/
typedef void (*generator_func_t)(quad*);

generator_func_t generators[] = {  
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_ASSIGN,
    generate_NOP,
    generate_JUMP,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_GREATER,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_LESSEQ,
    generate_NOT,
    generate_OR,
    generate_PARAM,
    generate_CALL,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_RETURN,
    generate_FUNCEND
};

/* generate instructions from quads */
void generate(void) {
    unsigned int i;
    for(i = 0; i < total; ++i){
        (*generators[quads[i].op])(quads+i);
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

void make_operand(expr* e, vmarg* arg){

    if(e == NULL) return;

    switch(e->type) {
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
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

