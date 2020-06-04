/**
 * CS-340 Compilers
 * Spring 2020
 * 
 * Project Phase 5: AVM Implementation
 * 
 * Antonis Droubogiannis    csd4014@csd.uoc.gr
 * Georgios Marios Kokiadis csd3962@csd.uoc.gr
 * Nikolaos Gounakis        csd3932@csd.uoc.gr
 */ 
#ifndef AVM_FUNC_H
#define AVM_FUNC_H

#define ACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1
#define INPUT_BUFFER_SIZE 512

#include "../avm/avm.h"
#include "../memory/memory.h"
#include <assert.h>
#include "../command_impl/HashTable.h"
#include <math.h>

typedef void (*library_func_t)(avm_memory*);
void avm_calllibfunc(char * id, avm_memory * memory);

unsigned int avm_get_envvalue(unsigned i, avm_memory * memory){
    //assert(memory->stack[i].type == number_m);
    unsigned int val = (unsigned) memory->stack[i].data.numVal;
    assert(memory->stack[i].data.numVal == (double) val);
    return val;
}

unsigned int avm_totalactuals(avm_memory* memory)
{
    return avm_get_envvalue(memory->topsp + ACTUALS_OFFSET,memory);
}

avm_memcell* avm_getactual(unsigned int i,avm_memory* memory)
{
    assert(i < avm_totalactuals(memory));
    return &(memory->stack[memory->topsp + AVM_STACKENV_SIZE + 1 + i]);
}

/* please call free() after you call this function */
char* avm_tostring(avm_memcell* cell)
{
    assert(cell);

    char* s;

    switch (cell->type)
    {
        case string_m :
        {
            s = strdup(cell->data.strVal);
            break;
        }
        case number_m :
        {
            s = malloc(50);
            sprintf(s,"%.3f",cell->data.numVal);
            break;
        }
        case bool_m :
        {
            s = malloc(6);
            if(cell->data.boolVal == 1) sprintf(s,"true");
            else if(cell->data.boolVal == 0) sprintf(s,"false");
            else assert(0);
            break;
        }
        case nil_m:
        {
            s = malloc(4);
            sprintf(s,"nil");
            break;
        }
        case userfunc_m :
        {
            s = malloc(50);
            int adr = (int) cell->data.funcVal.address;
            sprintf(s,"userfunc : %d",adr);
            break;
        }
        case libfunc_m :
        {
            s = malloc(50);
            sprintf(s,"libfunc (not implemented fully yet)");
            break;
        }
        case undef_m :
        {
            s = malloc(20);
            sprintf(s,"(undefined var)");
            break;
        }
        case table_m :
        {
            //printf("printint table (not fully implemented)\n");
            print_table(cell->data.tableVal);
            s = NULL;
            break;
        }

        default: printf("cell->type : %d",cell->type); assert(0);
    }
    return s;
}

void avm_dec_top(avm_memory * memory){
    if(!memory->top)
        avm_error("STACK OVERFLOW. NOT THE WEBSITE.");
    else
        --memory->top; 
}

void avm_push_envvalue(unsigned val, avm_memory * memory){
    memory->stack[memory->top].type = number_m;
    memory->stack[memory->top].data.numVal = val;
    avm_dec_top(memory);
}

void avm_callsaveenvironment(avm_memory * memory){    
    avm_push_envvalue(memory->totalActuals, memory);
    avm_push_envvalue(memory->pc + 1, memory);
    avm_push_envvalue(memory->top + memory->totalActuals + 2 ,memory);
    avm_push_envvalue(memory->topsp, memory);
}

void execute_call (avm_instruction * instr, avm_memory * memory) {
    avm_memcell * func = avm_translate_operand(instr->result, &(memory->ax));
	
    assert(func);
	if (func->type == table_m) {
		avm_instruction tmp;
		tmp.result = instr->result;
		tmp.srcLine = instr->srcLine;
		tmp.opcode = pusharg_v;
		execute_pusharg(&tmp, memory);
	}
    avm_callsaveenvironment(memory);

    switch(func->type) {
	case table_m: {
			avm_hashtable *tbl = func->data.tableVal->table;
			func = NULL;
			while (tbl != NULL) {
				if (tbl->index->type == string_m) {
					if (!strcmp(tbl->index->data.strVal, "()")) {
						func = tbl->content;
					}
				}
				tbl = tbl->next;
			}
			if (func == NULL) {
				avm_error("TABLE HAS NO ELEMENT ()");
			}
		}
        case userfunc_m: {
            memory->pc = func->data.funcVal.address;
            assert(memory->pc < memory->codeSize);         
            assert(code[memory->pc].opcode == funcenter_v);
            break;
        }

        case string_m: avm_calllibfunc(func->data.strVal, memory); break;
        case libfunc_m: avm_calllibfunc(func->data.libfuncVal, memory); break;

        default: {
            avm_error("CALLED INVALID FUNCTION");
        }
    }
}

void execute_pusharg (avm_instruction * instr, avm_memory * memory) {
    
    assert(instr->opcode == pusharg_v);

    avm_memcell * arg = avm_translate_operand(instr->result,&(memory->ax));
    avm_assign(&(memory->stack[memory->top]),arg);
    memory->totalActuals++;
    avm_dec_top(memory);
}

void execute_funcenter (avm_instruction * instr, avm_memory * memory) {
    avm_memcell * func = avm_translate_operand(instr->result, &(memory->ax));
    assert(func);
    assert(memory->pc == func->data.funcVal.address);

    memory->totalActuals = 0;
    avm_user_func funcInfo = avm_getfuncinfo(memory->pc, memory);
    memory->topsp = memory->top;
    memory->top = memory->top - funcInfo.locals;
    //printf("\n\nENTERED USER FUNCTION WITH ADDRESS %d AND LOCALS %d\n\n", funcInfo.address, funcInfo.locals);
}

void execute_funcexit (avm_instruction * instr, avm_memory * memory) {
    unsigned oldTop = memory->top;
    memory->top = avm_get_envvalue(memory->topsp + AVM_SAVEDTOP_OFFSET,memory);
    memory->pc = avm_get_envvalue(memory->topsp + AVM_SAVEDPC_OFFSET,memory);
    memory->topsp = avm_get_envvalue(memory->topsp + AVM_SAVEDTOPSP_OFFSET,memory);

    while(++oldTop <= memory->top)
        avm_memcellclear(&(memory->stack[oldTop]));

}


void libfunc_print(avm_memory * memory) {
   unsigned int n = avm_totalactuals(memory);
   if(n == 0) 
   {
       avm_error("libfunc print : CALLED WITH NO ARGUMENTS");
       return;
   }
   unsigned int i;
   for(i=0;i < n;i++)
   {
       char* s = avm_tostring(avm_getactual(i,memory));
       if(s != NULL)
       {
		   printf("%s", s);
           free(s);
       }
   }
}

char* typeStrings(avm_memcell_t type)
{
    char* s;
    switch (type)
    {
        case number_m :
        {
            s = "number";
            break;
        }
        case string_m :
        {
            s = "string";
            break;
        }
        case bool_m :
        {
            s = "bool";
            break;
        }
        case table_m :
        {
            s = "table";
        }
        case userfunc_m :
        {
            s = "userfunc";
            break;
        }
        case libfunc_m :
        {
            s = "libfunc";
            break;
        }
        case nil_m :
        {
            s = "nil";
            break;
        }
        case undef_m :
        {
            s = "undef";
            break;
        }
        default: printf("type was %d",type); assert(0);
    }
    return s;
}

void libfunc_cos(avm_memory* memory) {
    unsigned int n = avm_totalactuals(memory);

    if(n != 1)  avm_error("libfunc cos : ONE ARGUMENT (NOT %d) EXPECTED IN cos", n);
    avm_memcell * arg = avm_getactual(0,memory);

    if(arg->type != number_m) avm_error("libfunc cos : CALLED cos WITH NON-NUMBER ARGUMENT");

    avm_memcellclear(&(memory->retval));
    memory->retval.type = number_m;
    memory->retval.data.numVal = cos(arg->data.numVal);

}

void libfunc_sin(avm_memory* memory) {
    unsigned int n = avm_totalactuals(memory);

    if(n != 1)  avm_error("libfunc sin : ONE ARGUMENT (NOT %d) EXPECTED IN sin", n);
    avm_memcell * arg = avm_getactual(0,memory);

    if(arg->type != number_m) avm_error("libfunc sin : CALLED sin WITH NON-NUMBER ARGUMENT");

    avm_memcellclear(&(memory->retval));
    memory->retval.type = number_m;
    memory->retval.data.numVal = sin(arg->data.numVal);

}

void libfunc_typeof(avm_memory* memory)
{
    unsigned int n = avm_totalactuals(memory);

    if(n != 1)
        avm_error("libfunc typeof : ONE ARGUMENT (NOT %d) EXPECTED IN typeof", n);
    else
    {
        avm_memcellclear(&(memory->retval));
        memory->retval.type = string_m;
        memory->retval.data.strVal = strdup(typeStrings(avm_getactual(0,memory)->type));
    }
}

void libfunc_totalarguments(avm_memory* memory)
{
    /* topsp of previous activation record */
    unsigned prev_topsp = avm_get_envvalue(memory->topsp + AVM_SAVEDTOPSP_OFFSET ,memory);
    avm_memcellclear(&(memory->retval));

    /* case of no previous activation record */
    if(prev_topsp == AVM_STACK_SIZE - 1)
    {
        avm_error("libfunc totalarguments : CALLED OUTSIDE OF A FUNCTION");
        memory->retval.type = nil_m;
    }
    else
    {
        /* extract number of actual arguments for the previous activation record */
        memory->retval.type = number_m;
        memory->retval.data.numVal = avm_get_envvalue(prev_topsp + ACTUALS_OFFSET,memory);        
    }  
}

void libfunc_argument(avm_memory* memory)
{
    /* topsp of previous activation record */
    unsigned prev_topsp = avm_get_envvalue(memory->topsp + AVM_SAVEDTOPSP_OFFSET ,memory);
    avm_memcellclear(&(memory->retval));

    /* case of no previous activation record */
    if(prev_topsp == AVM_STACK_SIZE - 1)
    {
        avm_error("libfunc totalarguments : CALLED OUTSIDE OF A FUNCTION");
        memory->retval.type = nil_m;
    }
    else
    {
        if(avm_getactual(0, memory)->type != number_m)
            avm_error("CALLED ARGUMENT(INDEX) WITH INVALID PARAMETERS.");
        unsigned i = (unsigned) avm_getactual(0, memory)->data.numVal;  
        avm_assign(&(memory->retval), &(memory->stack[i + prev_topsp + ACTUALS_OFFSET + 1]));
    }  
}

int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || *s == ' ')
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

void libfunc_input (avm_memory * memory) {
    unsigned int n = avm_totalactuals(memory);
    char * buf;
    int i = 0;
    if(n != 0) avm_error("libfunc input : CALLED WITH INVALID ARGUMENTS");
    
    avm_memcellclear(&(memory->retval));

    fgets(buf, INPUT_BUFFER_SIZE, stdin);
    if(strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0){
        memory->retval.type = bool_m;
        memory->retval.data.boolVal = strcmp(buf, "true") == 0;
    } else if(isNumeric(buf)){
        memory->retval.type = number_m;
        memory->retval.data.numVal = atof(buf);
    } else {
        memory->retval.type = string_m;
        memory->retval.data.strVal = buf;
    }
}

void libfunc_objectmemberkeys (avm_memory * memory) {
    unsigned int n = avm_totalactuals(memory);
    unsigned i;
    if(n != 1)  avm_error("libfunc objectcopy : ONE ARGUMENT (NOT %d) EXPECTED IN objectcopy", n);

    avm_memcell * arg = avm_getactual(0,memory);

    if(arg->type != table_m) avm_error("libfunc objectcopy : CALLED objectcopy WITH NON-TABLE ARGUMENT");
    
    avm_hashtable * entry = arg->data.tableVal->table;
    avm_memcellclear(&(memory->retval));

    memory->retval.type = table_m;
	memory->retval.data.tableVal = (avm_table*)malloc(sizeof(avm_table));
	memory->retval.data.tableVal->refcounter = 0;
	memory->retval.data.tableVal->table = NULL;
	avm_refcounter_incr(memory->retval.data.tableVal);

    i = 0;
    while(entry != NULL){
        memory->ax.type = number_m;
        memory->ax.data.numVal = i;
        avm_tablesetelem(memory->retval.data.tableVal, &(memory->ax), entry->index);
        i++;
        entry = entry->next;
    }   
}

void libfunc_objecttotalmembers (avm_memory * memory) {
    unsigned int n = avm_totalactuals(memory);
    if(n != 1)  avm_error("libfunc objectcopy : ONE ARGUMENT (NOT %d) EXPECTED IN objectcopy", n);

    avm_memcell * arg = avm_getactual(0,memory);

    if(arg->type != table_m) avm_error("libfunc objectcopy : CALLED objectcopy WITH NON-TABLE ARGUMENT");
    
    int count = 0;
    avm_hashtable * entry = arg->data.tableVal->table;
    while(entry != NULL){
        count++;
        entry = entry->next;
    }

    avm_memcellclear(&(memory->retval));
    memory->retval.type = number_m;
    memory->retval.data.numVal = count;
    //avm_refcounter_incr(memory->retval.data.tableVal);

}

void libfunc_objectcopy (avm_memory * memory) {
    unsigned int n = avm_totalactuals(memory);
    if(n != 1)  avm_error("libfunc objectcopy : ONE ARGUMENT (NOT %d) EXPECTED IN objectcopy", n);

    avm_memcell * arg = avm_getactual(0,memory);

    if(arg->type != table_m) avm_error("libfunc objectcopy : CALLED objectcopy WITH NON-TABLE ARGUMENT");

    avm_memcellclear(&(memory->retval));
    memory->retval.type = table_m;
    memory->retval.data.tableVal = arg->data.tableVal;
    //avm_refcounter_incr(memory->retval.data.tableVal);
}

void libfunc_strtonum (avm_memory * memory) {
    unsigned int n = avm_totalactuals(memory);
    if(n != 1)  avm_error("libfunc strtonum : ONE ARGUMENT (NOT %d) EXPECTED IN strtonum", n);

    avm_memcell * arg = avm_getactual(0,memory);

    if(arg->type != string_m) avm_error("libfunc strtonum : CALLED strtonum WITH NON-STRING ARGUMENT");

    if(!isNumeric(arg->data.strVal)) avm_error("libfunc strtonum : UNABLE TO CONVERT STRING");

    avm_memcellclear(&(memory->retval));
    memory->retval.type = number_m;
    memory->retval.data.numVal = atof(arg->data.strVal);
}

void libfunc_sqrt (avm_memory * memory) {
    unsigned int n = avm_totalactuals(memory);

    if(n != 1)  avm_error("libfunc sqrt : ONE ARGUMENT (NOT %d) EXPECTED IN sqrt", n);
    avm_memcell * arg = avm_getactual(0,memory);

    if(arg->type != number_m) avm_error("libfunc sqrt : CALLED sqrt WITH NON-NUMBER ARGUMENT");

    avm_memcellclear(&(memory->retval));
    memory->retval.type = number_m;
    memory->retval.data.numVal = sqrt(arg->data.numVal);
}

library_func_t library_funcs[] = {
    libfunc_print,
    libfunc_cos,
    libfunc_sin,
    libfunc_typeof,
    libfunc_totalarguments,
    libfunc_argument,
    libfunc_input,
    libfunc_objectmemberkeys,
    libfunc_objecttotalmembers,
    libfunc_objectcopy,
    libfunc_strtonum,
    libfunc_sqrt
};

library_func_t avm_getlibraryfunc (char * id){
    if(strcmp(id, "print") == 0)        return library_funcs[0];
    else if (strcmp(id, "cos") == 0)    return library_funcs[1];
    else if (strcmp(id, "sin") == 0)    return library_funcs[2];
    else if (strcmp(id,"typeof") == 0) return library_funcs[3];
    else if(strcmp(id,"totalarguments") == 0) return library_funcs[4];
    else if(strcmp(id,"argument") == 0) return library_funcs[5];
    else if(strcmp(id,"input") == 0) return library_funcs[6];
    else if(strcmp(id,"objectmemberkeys") == 0) return library_funcs[7];
    else if(strcmp(id,"objecttotalmembers") == 0) return library_funcs[8];
    else if(strcmp(id,"objectcopy") == 0) return library_funcs[9];
    else if(strcmp(id,"strtonum") == 0) return library_funcs[10];
    else if(strcmp(id,"sqrt") == 0) return library_funcs[11];
    else avm_error("CALLED INVALID LIBRARY FUNCTION");
    return NULL;
}

void avm_calllibfunc(char * id, avm_memory * memory){
    library_func_t f = avm_getlibraryfunc(id);
    if(!f) return;

    memory->topsp = memory->top;
    memory->totalActuals = 0;
    (*f)(memory);
    
    if(!memory->executionFinished)
        execute_funcexit((avm_instruction *) 0, memory);
}

#endif