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
 *
 */

#include "target_emit.h"

instruction* instructions = (instruction *) 0;
unsigned total_Instructions = 0;
unsigned int currInstruction = 0;

int isInteger(double val)
{
    int truncated = (int)val;
    return (val == truncated);
}

void expand_instruction (void){
    assert(total_Instructions == currInstruction);
    instruction * p = (instruction *) malloc(NEW_SIZE_I);
    if (instructions) {
        memcpy(p, instructions, CURR_SIZE_I);
        free(instructions);
    }
    instructions = p;
    total_Instructions += EXPAND_SIZE_I;
}

void emit_instruction(instruction* i) {
    if(currInstruction == total_Instructions)
        expand_instruction();
    
    instruction * p = instructions + currInstruction++;
    p->arg1 = i->arg1;
    p->arg2 = i->arg2;
    p->result = i->result;
    p->label = i->label;
    p->opcode = i->opcode;
    p->srcLine = i->srcLine;
}

void print_tables(FILE *file, StringStack *StringHead, StringStack *LibHead, FunctionStack *FunHead, NumStack *NumHead){
    int i;
    fprintf(file, "42069\n");


    fprintf(file, "%d\n", FindStringStackSize(StringHead));
    StringStack *StringTmp = StringHead;
    for(i = 0; i < FindStringStackSize(StringHead); i++){
        fprintf(file, "%d %s\n", strlen(StringTmp->s), StringTmp->s);
        StringTmp = StringTmp->next;
    }

    fprintf(file, "%d\n", FindNumStackSize(NumHead));
    NumStack *NumTmp = NumHead;
    for(i = 0; i < FindNumStackSize(NumHead); i++){
        fprintf(file, "%.3f\n", NumTmp->num);
        NumTmp = NumTmp->next;
    }
    fprintf(file, "%d\n", FindFunStackSize(FunHead));
    FunctionStack *FunTmp = FunHead;
    for(i = 0; i < FindFunStackSize(FunHead); i++){
        fprintf(file, "%d\n%d\n", FunTmp->address, FunTmp->local_size);
        FunTmp = FunTmp->next;
    }

    fprintf(file, "%d\n", FindStringStackSize(LibHead));
    StringStack *LibTmp = LibHead;
    for(i = 0; i < FindStringStackSize(LibHead); i++){
        fprintf(file, "%d %s\n", strlen(LibTmp->s), LibTmp->s);
        LibTmp = LibTmp->next;
    }
    
    fprintf(file,"%d\n", globals+1);

}
void print_instruction_arg(vmarg * arg, FILE * file){
    if(arg != NULL){
        switch(arg->type)
        {
           case label_a : fprintf(file,"%.0f",arg->num_val); break;
           case global_a : fprintf(file,"01:"); fprintf(file,"%.0f",arg->num_val); break;
           case formal_a : fprintf(file,"02:"); fprintf(file,"%.0f",arg->num_val); break;
           case local_a : fprintf(file,"03:"); fprintf(file,"%.0f",arg->num_val); break;
           case number_a : {
               fprintf(file,"04:");
               if(!isInteger(arg->num_val)) fprintf(file,"%d",FindNumStackIndex(NumHead,arg->num_val));
               else fprintf(file,"%d",FindNumStackIndex(NumHead,arg->num_val));
               
               break;
           }
           case string_a : fprintf(file,"05:"); fprintf(file,"%d",FindStringStackIndex(StringHead,arg->str_val)); break;
           case bool_a : fprintf(file,"06:"); fprintf(file,"%d",arg->bool_val); break;
           case nil_a : fprintf(file,"07:"); fprintf(file,"NULL"); break;
           case userfunc_a : fprintf(file,"08:"); fprintf(file,"%d",FindFunctionStackIndex(FunHead,arg->str_val, arg->num_val)); break;
           case libfunc_a : fprintf(file,"09:"); fprintf(file,"%d",FindStringStackIndex(LibHead,arg->str_val)); break;
           case retval_a : fprintf(file,"10:"); break;
           default : printf("\nPRINT INSTRUCTION ASSERT FAILED : %d\n",arg->type); assert(0);
        }
    }
    
}

void print_instructions(FILE * file, int max_lines){
    int i, j;
    print_tables(file, StringHead, LibHead, FunHead, NumHead);
    instruction curr_instruction;

    fprintf(file, "%d\n", currInstruction);

    for(i = 0; i < currInstruction; i++){
        curr_instruction = instructions[i];
    
        fprintf(file, "%d ", instructions[i].srcLine);

        switch(curr_instruction.opcode){
            case assign_v:    	{ fprintf(file, "ASSIGN "); break;}
            case add_v:       	{ fprintf(file, "ADD "); break; }
            case sub_v:       	{ fprintf(file, "SUB "); break; }

            case mul_v:       	{ fprintf(file, "MUL "); break; }
            case div_v:      	{ fprintf(file, "DIV "); break; }
            case mod_v:       	{ fprintf(file, "MOD "); break; }

            case uminus_v:    	{ fprintf(file, "%d ", uminus_v); break; }
            case and_v:    		{ fprintf(file, "%d ", and_v); break; }
            case or_v:    		{ fprintf(file, "%d ", or_v); break; }

			case not_v:    		{ fprintf(file, "%d ", not_v); break; }
            case jeq_v:    		{ fprintf(file, "JEQ "); break; }
            case jne_v:  	{ fprintf(file, "JNE "); break; }

			case jle_v: 	{ fprintf(file, "JLE "); break; }
            case jge_v:	{ fprintf(file, "JGE "); break; }
            case jlt_v:    	{ fprintf(file, "JLT "); break; }
			
			case jgt_v:    { fprintf(file, "JGT "); break; }
            case call_v:    		{ fprintf(file, "CALL "); break; }
            case pusharg_v:    		{ fprintf(file, "PUSHARG "); break; }
			
            case funcenter_v:    	{ fprintf(file, "FUNCENTER "); break; }

			case funcexit_v:    	{ fprintf(file, "FUNCEXIT "); break; }
            case newtable_v:   { fprintf(file, "NEWTABLE "); break; }
            case tablegetelem_v:  { fprintf(file, "TABLEGETELEM "); break; }
			case tablesetelem_v:  { fprintf(file, "TABLESETELEM "); break; }
            case jump_v:          { fprintf(file, "JUMP "); break; }

            default: { fprintf(file, "*ERROR* "); break; }
        }
        switch(curr_instruction.opcode){
            case jeq_v:
            case jne_v:
            case jle_v:
            case jge_v:
            case jlt_v:
            case jgt_v: {
                    print_instruction_arg(curr_instruction.arg1, file);
                    fprintf(file," ");
                    print_instruction_arg(curr_instruction.arg2, file);
                    fprintf(file," ");
                    fprintf(file, "%d", curr_instruction.label);
                    break;
            }
            case jump_v: {    
                    fprintf(file, "%d", curr_instruction.label);
                    break; }
            case assign_v:
            {
                if(curr_instruction.result->type == retval_a && curr_instruction.result->is_getretval != 1)
                {
                    print_instruction_arg(curr_instruction.arg1, file);
                    fprintf(file," ");
		            print_instruction_arg(curr_instruction.result, file);
                }
                else
                {
                    print_instruction_arg(curr_instruction.result, file);
                    fprintf(file," ");
		            print_instruction_arg(curr_instruction.arg1, file);
                }
                
                break;
            }
            case funcenter_v:
            case funcexit_v: {
                print_instruction_arg(curr_instruction.result, file);
                break;
            }
            case call_v:
            case newtable_v:
            case pusharg_v: {
                print_instruction_arg(curr_instruction.arg1, file);
                break;                
            }
            default: {
                print_instruction_arg(curr_instruction.result, file);
                fprintf(file," ");
		        print_instruction_arg(curr_instruction.arg1, file);
                fprintf(file," ");
                print_instruction_arg(curr_instruction.arg2, file);
                break;
            }
            
        }
        
        fprintf(file, "\n");
    }}