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

instruction* make_new_instruction()
{
    instruction *t = (instruction*) malloc(sizeof(instruction));
    t->arg1 = malloc(sizeof(vmarg));
    t->arg2 = malloc(sizeof(vmarg));
    t->result = malloc(sizeof(vmarg));

    return t;
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
    printf("\nopcode %d:\n",p->opcode);
    p->srcLine = i->srcLine;
}

void print_instruction_arg(vmarg * arg, FILE * file){
    if(arg != NULL)
        switch(arg->type)
        {
           case label_a : fprintf(file,"00 "); fprintf(file,", %d : %s    ",arg->num_val,arg->str_val); break;
           case global_a : fprintf(file,"01 "); fprintf(file,", %d : %s   ",arg->num_val,arg->str_val); break;
           case formal_a : fprintf(file,"02 "); fprintf(file,", %d : %s   ",arg->num_val,arg->str_val); break;
           case local_a : fprintf(file,"03 "); fprintf(file,", %d : %s    ",arg->num_val,arg->str_val); break;
           case number_a : fprintf(file,"04 "); fprintf(file,", %d : %f   ",FindNumStackIndex(NumHead,arg->num_val),arg->num_val); break;
           case string_a : fprintf(file,"05 "); fprintf(file,", %d : %s   ",FindStringStackIndex(StringHead,arg->str_val),arg->str_val); break;
           case bool_a : fprintf(file,"06 "); fprintf(file,", %d    ",arg->bool_val); break;
           case nil_a : fprintf(file,"07 "); fprintf(file,", NULL   "); break;
           case userfunc_a : fprintf(file,"08 "); fprintf(file,", %d : %s  ",FindFunctionStackIndex(FunHead,arg->str_val),arg->str_val); break;
           case libfunc_a : fprintf(file,"09 "); fprintf(file,", %d : %s  ",FindStringStackIndex(StringHead,arg->str_val),arg->str_val); break;
           case retval_a : fprintf(file,"10     "); break;
           default : printf("\nPRINT INSTRUCTION ASSERT FAILED : %d\n",arg->type); assert(0);
        }
    
}

void print_instructions(FILE * file, int max_lines){
    int i, j;
    printf("\n\033[1;32m>> \033[01;33mTARGET CODE\n\n\033[0m");
    instruction curr_instruction;
    for(i = 0; i < currInstruction; i++){
        curr_instruction = instructions[i];
    
        if(file == stdout){
            fprintf(file, "\033[0;36m[line %d]\033[0m", instructions[i].srcLine, i + 1);

            for(j = 0; j < numPlaces(max_lines) - numPlaces(instructions[i].srcLine) + 2 ; j++){
                fprintf(file, " ");
            }

            fprintf(file, "\033[0;32m%d\033[0m", i + 1);
        
            for(j = 0; j < numPlaces(currInstruction) - numPlaces(i + 1) + 2 ; j++){
                fprintf(file, " ");
            }
        }
        printf("\033[0;33m");
        switch(curr_instruction.opcode){
            case assign_v:    	{ fprintf(file, "ASSIGN "); break;}
            case add_v:       	{ fprintf(file, "ADD "); break; }
            case sub_v:       	{ fprintf(file, "SUB "); break; }

            case mul_v:       	{ fprintf(file, "MUL "); break; }
            case div_v:      	{ fprintf(file, "DIV "); break; }
            case mod_v:       	{ fprintf(file, "MOD "); break; }

            case uminus_v:    	{ fprintf(file, "UMINUS "); break; }
            case and_v:    		{ fprintf(file, "AND "); break; }
            case or_v:    		{ fprintf(file, "OR "); break; }

			case not_v:    		{ fprintf(file, "NOT "); break; }
            case jeq_v:    		{ fprintf(file, "IF_EQ "); break; }
            case jne_v:  	{ fprintf(file, "IF_NOTEQ "); break; }

			case jle_v: 	{ fprintf(file, "IF_LESSEQ "); break; }
            case jge_v:	{ fprintf(file, "IF_GREATEREQ "); break; }
            case jlt_v:    	{ fprintf(file, "IF_LESS "); break; }
			
			case jgt_v:    { fprintf(file, "IF_GREATER "); break; }
            case call_v:    		{ fprintf(file, "CALL "); break; }
            case pusharg_v:    		{ fprintf(file, "PARAM "); break; }
			
            case funcenter_v:    	{ fprintf(file, "FUNCSTART "); break; }

			case funcexit_v:    	{ fprintf(file, "FUNCEND "); break; }
            case newtable_v:   { fprintf(file, "TABLECREATE "); break; }
            case tablegetelem_v:  { fprintf(file, "TABLEGETELEM "); break; }
			case tablesetelem_v:  { fprintf(file, "TABLESETELEM "); break; }
            case jump_v:          { fprintf(file, "JUMP "); break; }

            default: { fprintf(file, "*ERROR* "); break; }
        }
        printf("\033[0m");
        switch(curr_instruction.opcode){
            case jeq_v:
            case jne_v:
            case jle_v:
            case jge_v:
            case jlt_v:
            case jgt_v: {
                    print_instruction_arg(curr_instruction.arg1, file);
                    print_instruction_arg(curr_instruction.arg2, file);
                    printf("\033[0;32m");
                    fprintf(file, "00, %d", curr_instruction.label);
                    printf("\033[0m");
                    break;
            }
            case jump_v: {    
                    printf("\033[0;32m"); 
                    fprintf(file, "00, %d", curr_instruction.label);
                    printf("\033[0m");
                    break; }
            case assign_v:
            {
                printf("\033[0;35m");
                print_instruction_arg(curr_instruction.result, file);
                printf("\033[0m");
		        print_instruction_arg(curr_instruction.arg1, file);
                break;
            }
            default: {
                printf("\033[0;35m");
                print_instruction_arg(curr_instruction.result, file);
                printf("\033[0m");
		        print_instruction_arg(curr_instruction.arg1, file);
                print_instruction_arg(curr_instruction.arg2, file);
                break;
            }
            
        }
        
        fprintf(file, "\n");
    }
    printf("\n\033[1;32m>>> \033[01;33mEND\n\033[0m");
}