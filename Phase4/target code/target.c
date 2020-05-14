#include "target.h"

void make_operand(expr* e, vmarg* arg){

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
            arg->val = const_newstring(e->strConst);
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

