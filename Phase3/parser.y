%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include "symtab/symtable.h"
#include "booleanList/booleanList.h"


#define YY_DECL int alpha_yylex (void* yylval)

  extern int yylineno;
  extern char * yyval;
  extern char * yytext;
  extern FILE * yyin;
  extern FILE * yyout;

SymTable *table;
unsigned int currscope=0;
unsigned int currfunc=0;
unsigned int anonym_func_count = 0;
unsigned int flag_func = 0;
unsigned int flag_op = 0;
unsigned int fail_icode = 0;
unsigned int return_flag = 0;
unsigned int loop_flag = 0;
SymTabEntry *global_tmp;
int table_flag = 0;
int jump_label = 0;

FILE * errorFile;

%}

/* Yacc Definitions */

%start program

%token IF
%token ELSE
%token WHILE
%token FOR
%token FUNCTION
%token RETURN
%token BREAK
%token CONTINUE

%token AND
%token OR
%token NOT

%token local
%token<bool> TRUE
%token<bool> FALSE
%token<real> NIL

%token ASSIGN
%token PLUS
%token MINUS
%token MULT
%token DIV
%token MOD
%token EQUAL
%token NOT_EQUAL
%token PLUS_PLUS
%token MINUS_MINUS
%token GREATER_EQUAL
%token GREATER
%token LESS_EQUAL
%token LESS

%token<real> REALCONST
%token<real> INTCONST
%token<id> STRING

%token LEFT_BRACE /* { */
%token RIGHT_BRACE 
%token LEFT_BRACKET /* [ */
%token RIGHT_BRACKET
%token LEFT_PARENTHESIS /* ( */
%token RIGHT_PARENTHESIS

%token SEMICOLON
%token COMA
%token COLON
%token DOUBLE_COLON
%token DOT
%token DOUBLE_DOT

%token<id> ID

/*  expr types */
%type<express> lvalue
%type<express> primary
%type<express> term
%type<express> expr
%type<express> compexpr
%type<express> boolexpr
%type<express> assignexpr
%type<express> const
%type<express> member
%type<ipc> compop

/* priority */

%right ASSIGN

%left OR
%left AND

%nonassoc EQUAL NOT_EQUAL
%nonassoc GREATER GREATER_EQUAL LESS LESS_EQUAL

%left PLUS MINUS
%left MULT DIV MOD
/* UMINUS is minus infront of a term */
%right NOT PLUS_PLUS MINUS_MINUS UMINUS

%left DOT DOUBLE_DOT
%left LEFT_BRACKET RIGHT_BRACKET
%left LEFT_PARENTHESIS RIGHT_PARENTHESIS

/* expr is a struct , we need to include the code */
%code requires { #include "quads/quads.h" }

%union {double integer; char* id; double real; expr *express; unsigned char bool; iopcode ipc;}

/* %expect 14 */

%%

program : statements
        ;

statements : 
           | statement statements /* correct statement ,  continue */
           | error_statement statements /* wrong statement , continue */
           ;

error_statement : error statement /* consume the stack until you find a statement */
                ;

statement :
       expr SEMICOLON
     | ifstmt
     |  whilestmt
     | forstmt
     |  returnstmt {
		 			if(return_flag == 0)
	 				 {
		 				fprintf(errorFile,"ERROR @ line %d: Cannot use return outside of a function\n",yylineno);
						fail_icode = 1;
					 }
				   }
     |  BREAK SEMICOLON {
		 				if(loop_flag == 0)
	 					 {
		 					fprintf(errorFile,"ERROR @ line %d: Cannot use break outside of a loop\n",yylineno);
							 fail_icode = 1;
						 }
	 					}
     |  CONTINUE SEMICOLON {
		 					if(loop_flag == 0)
	 						{ 
								 fprintf(errorFile,"ERROR @ line %d: Cannot use continue outside of a loop\n",yylineno);
								 fail_icode = 1;
							}
	 					   }
     |  block
     |  funcdef
     | SEMICOLON
     ;

expr : assignexpr
     	| expr PLUS expr {
        	if(flag_func == 1 && flag_op == 0) {
        	        fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : expr -> expr op expr\n", yylineno); 
        	        fail_icode = 1; 
        	}
        	flag_op = 0; flag_func = 0;

			SymTabEntry *tmp = (SymTabEntry *)newtemp(table,currscope, currfunc, 0);
			$$ = newexpr(arithexpr_e,tmp);
        	    emit(add, $1, $3, $$, yylineno);
			}
     	| expr MINUS expr {
        	if(flag_func == 1 && flag_op == 0) {
        	        fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : expr -> expr op expr\n", yylineno); 
        	        fail_icode = 1;
        	}
			SymTabEntry *tmp = (SymTabEntry *)newtemp(table,currscope, currfunc, 0);
			$$ = newexpr(arithexpr_e,tmp);
        	    emit(sub, $1, $3, $$, yylineno);
			}
     	| expr MULT expr {
        	if(flag_func == 1 && flag_op == 0) {
        	        fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : expr -> expr op expr\n", yylineno); 
        	        fail_icode = 1; 
        	}
			SymTabEntry *tmp = (SymTabEntry *)newtemp(table,currscope, currfunc, 0);
			$$ = newexpr(arithexpr_e,tmp);
        	    emit(mul, $1, $3, $$, yylineno);
			}
     	| expr DIV expr {
        	if(flag_func == 1 && flag_op == 0) {
        	    fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : expr -> expr op expr\n", yylineno); 
        	    fail_icode = 1; 
        	}
			SymTabEntry *tmp = (SymTabEntry *)newtemp(table,currscope, currfunc, 0);
			$$ = newexpr(arithexpr_e,tmp);
        	emit(diva, $1, $3, $$, yylineno);
			}

	 	| term { $$ = $1; }
		| compexpr { $$ = $1; }
     	;

compexpr :
		boolexpr OR boolexpr {
			flag_op = 1; flag_func = 0;
			backpatch($1->falselist, 69);
			$1->truelist = booleanList_merge($1->truelist, $3->falselist);
			$$ = newexpr(boolexpr_e, NULL);
			$$->falselist = $3->falselist;
		}
		
		| compexpr OR boolexpr{
			flag_op = 1; flag_func = 0;
			backpatch($1->falselist, 69);
			$1->truelist = booleanList_merge($1->truelist, $3->falselist);
			$$ = newexpr(boolexpr_e, NULL);
			$$->falselist = $3->falselist;
		}

		| boolexpr AND boolexpr {
			flag_op = 1; flag_func = 0;
		}

		| NOT boolexpr {
			flag_op = 1; flag_func = 0;
		}
		;

boolexpr : expr compop expr {
			$$ = newexpr(boolexpr_e, NULL);
			$$->truelist = booleanList_makeList(currQuad);
			$$->falselist = booleanList_makeList(currQuad + 1);
			printf("%.0f op %.0f\n", $1->numConst, $3->numConst);
			emit($2, $1, $3, NULL, yylineno);
			emit(jump,NULL, NULL, NULL, yylineno);
		}
		;

compop: GREATER {$$ = if_greater; }
   | GREATER_EQUAL {$$ = if_greatereq; }
   | LESS {$$ = if_less; }
   | LESS_EQUAL {$$ = if_lesseq; }
   | EQUAL {flag_op = 1; flag_func = 0; $$ = if_eq;}
   | NOT_EQUAL {flag_op = 1; flag_func = 0; $$ = if_noteq;}
   ;

term : LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {$$ = $2;}
     | MINUS expr {
        	if(flag_func == 1)
			{
				fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : term -> - expr\n", yylineno); 
				fail_icode = 1;
			} 
				
			flag_func = 0;
			emit(mul, $2, newconstnumexpr(-1), $2, yylineno);
            $$ = $2;
		} %prec UMINUS
     | NOT expr {flag_op = 1; flag_func = 0; fprintf(yyout,"term -> not expr\n");}
     | PLUS_PLUS lvalue {
            if(flag_func == 1)
            {
				fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : term -> ++ lvalue\n", yylineno);
				fail_icode = 1;
			}
            flag_func = 0;
			
			emit(add, $2, newconstnumexpr(1), $2, yylineno);
			$$ = $2;
		}
     | lvalue PLUS_PLUS {
		 				 if(flag_func == 1) 
	 					 {
							 fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : term -> lvalue ++\n", yylineno);
							 fail_icode = 1;
						 }
	 					 flag_func = 0;
						}
     | MINUS_MINUS lvalue {
		 					if(flag_func == 1)  
							{
								fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : term -- lvalue\n", yylineno);
								fail_icode = 1;
	 						}
	 					   flag_func = 0;
						  }
     | lvalue MINUS_MINUS {
		 				   if(flag_func == 1)
	 					   {
								fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : lavlue --\n", yylineno);
								fail_icode = 1;
						   } 
						   flag_func = 0;
						  }
     | primary { $$ = $1; }
     ;

assignexpr : lvalue {
						if(flag_func == 1){
							fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : assignexpr -> lvalue = expr\n", yylineno);
							fail_icode = 1;
						} 
						flag_func = 0; table_flag = 1; 
					} 
		ASSIGN expr {
						if($1->index != NULL){
							emit(tablesetelem, $1->index, $4, $1, yylineno);
							emit_iftableitem($lvalue, table, currscope, currfunc, 1, yylineno);
						} else{ 
							emit(assign, $4, NULL, $1, yylineno);
						}
						table_flag = 0;
					}

primary : lvalue
        | call
        | objectdef
        | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
        | const
        ;

lvalue : ID {	
			SymTabEntry *tmp = lookup_SymTable(table, $1);
			if(tmp != NULL && tmp->isActive == 1){
				if(tmp->scope != 0 && tmp->func_scope != currfunc && strcmp(SymbolTypeToString(tmp->type),"LIBFUNC") && strcmp(SymbolTypeToString(tmp->type),"USERFUNC")){
					fprintf(errorFile, "ERROR @ line %d: %s cannot be accessed\n",yylineno, $1);
					fail_icode = 1;
				}
				else if(!strcmp(SymbolTypeToString(tmp->type),"LIBFUNC") || !strcmp(SymbolTypeToString(tmp->type),"USERFUNC")){
					flag_func = 1;
					global_tmp = tmp;
				}
			}
				
            if(currscope == 0) insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope,currfunc, GLOBAL));
			else insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, currfunc, LOCAL));

			tmp = lookup_SymTable(table, $1);
			
			$$ = newexpr(var_e,tmp);
			printf("LVALUE: %s\n", $1);
			
		}
       | local ID {

			SymTabEntry *tmp = lookup_SymTable(table, $2);
			if(tmp != NULL && tmp->isActive == 1){
				if(!strcmp(SymbolTypeToString(tmp->type),"LIBFUNC")){
					fprintf(errorFile, "ERROR @ line %d: %s is a library function\n",yylineno, $2);
					fail_icode = 1;
				}
                                else if(tmp->scope == currscope && strcmp(SymbolTypeToString(tmp->type),"FORMAL") == 0)
                                {
                                        /* do nothing , refer to arguments as local*/
                                }
				else if(tmp->scope == currscope){
					fprintf(errorFile, "ERROR @ line %d: %s already declared\n",yylineno, $2);
					fail_icode = 1;
				}
				else insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope,currfunc, LOCAL));
			}
			else {
				insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope,currfunc, LOCAL));
			}
		
		}
       | DOUBLE_COLON ID {
				SymTabEntry *tmp = lookup_SymTableScope(table, 0, $2);
				if(tmp != NULL){
					if(tmp->scope != 0) 
					{ 
						fprintf(errorFile, "ERROR @ line %d: %s is not a global variable nor a global function\n",yylineno, $2);
						fail_icode = 1;
                    }      
                    /* found global function */
                    if(strcmp(SymbolTypeToString(tmp->type),"LIBFUNC") == 0 || strcmp(SymbolTypeToString(tmp->type),"USERFUNC") == 0 ) flag_func = 1;
				}
				else
				{ 
					fprintf(errorFile, "ERROR @ line %d: %s is not a global variable nor a global function\n",yylineno, $2);
					fail_icode = 1;
				}
	    }
       | member {
				 if(table_flag==1) $lvalue = emit_iftableitem($lvalue, table, currscope, currfunc, 1, yylineno);;
		}
       ;

member : lvalue DOT ID {
		$lvalue = emit_iftableitem($lvalue, table, currscope, currfunc, 1, yylineno);
		expr* item = newexpr(tableitem_e, $lvalue->sym);
		expr* tmp = newconststringexpr($3);
		item->index = tmp;
		$$ = item;
		}
       | lvalue LEFT_BRACKET expr RIGHT_BRACKET
       | call DOT ID
       | call LEFT_BRACKET expr RIGHT_BRACKET
       ;

call : call {flag_func = 0;} LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
     | lvalue {flag_func = 0;} callsuffix
     | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS {flag_func = 0;} LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
     ;

callsuffix : normcall
           | methodcall
           ;

normcall : LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
         ;

methodcall : DOUBLE_DOT ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
           ;

elist :
      | expr {flag_func = 0;} COMA elist
	  | expr {flag_func = 0;}
      ;

objectdef : LEFT_BRACKET elist RIGHT_BRACKET
          | LEFT_BRACKET indexed RIGHT_BRACKET
          ;

indexed : indexedelem comaindexedelem
        ;

comaindexedelem : /* */
                | COMA indexedelem comaindexedelem
                ;

indexedelem : LEFT_BRACE expr COLON expr RIGHT_BRACE
            ;

block : LEFT_BRACE { currscope++; } statements RIGHT_BRACE { hide_Scope(table,currscope); currscope--;}
      ;

funcdef : FUNCTION ID {
		expr *tmp_expr = newexpr(constnum_e,NULL);
		jump_label = currQuad;
		emit(jump, NULL, NULL, tmp_expr, yylineno);
		
		currfunc++;
		SymTabEntry *tmp = lookup_SymTableScope(table, currscope, $2);
		if(tmp != NULL){
			if(!strcmp(SymbolTypeToString(tmp->type),"LIBFUNC")){
				fprintf(errorFile, "ERROR @ line %d: %s is library function\n",yylineno, $2);
				fail_icode = 1;
			}
			else
			{ 
				fprintf(errorFile, "ERROR @ line %d: %s already declared\n",yylineno, $2);
				fail_icode = 1;
			}
		}
		else tmp = insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope,currfunc, USERFUNC));

		emit(funcstart, NULL, NULL, newexpr(programfunc_e, tmp), yylineno);

		} LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS { return_flag = 1; } block {
			
			/* out of function block , we cant use return here */
			return_flag = 0;

			SymTabEntry *tmp = lookup_SymTableScope(table, currscope, $2);
			emit(funcend, NULL, NULL, newexpr(programfunc_e, tmp), yylineno);
			edit_quad(jump_label, NULL, NULL, newconstnumexpr((double)currQuad+1));
			currfunc--;

		}
        | FUNCTION {currfunc++;
                    char* anonym = (char *)malloc(sizeof(char)*2);
                    sprintf(anonym,"$%d",anonym_func_count++);
                    insert_SymTable(table, new_SymTabEntry(anonym, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope,currfunc, USERFUNC));
                   }  LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS  block
        ;

const : REALCONST { $$ = newconstnumexpr((double) $1); }
      | INTCONST { $$ = newconstnumexpr((double) $1); }
      | STRING { $$ = newconststringexpr($1); }
      | NIL { $$ = newconstnumexpr((double) $1); }
      | TRUE { $$ = newconstboolexpr(VAR_TRUE); }
      | FALSE { $$ = newconstboolexpr(VAR_FALSE); }
      ;
 
idlist : /*   */
       | ID {
			 SymTabEntry *tmp = lookup_SymTableScope(table, currscope+1, $1);
			 if(tmp == NULL && (tmp = lookup_SymTableScope(table, 0, $1)) != NULL)
			 {
				if(tmp != NULL && !strcmp(SymbolTypeToString(tmp->type),"LIBFUNC"))
				{
					fprintf(errorFile,"ERROR @ line %d: Cannot have a library function as argument\n", yylineno);
					fail_icode = 1;
				}
				else insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1,currfunc, FORMAL));
			}
			else if(tmp != NULL && !strcmp(SymbolTypeToString(tmp->type),"FORMAL") && tmp->isActive == 1)
			{
				fprintf(errorFile,"ERROR @ line %d: Cannot have the same argument names\n", yylineno);
				fail_icode = 1;
			}
			else insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1, currfunc,FORMAL));
	   
		}	
	   COMA idlist
	   | ID {
			 SymTabEntry *tmp = lookup_SymTableScope(table, currscope+1, $1);
			 if(tmp == NULL && (tmp = lookup_SymTableScope(table, 0, $1)) != NULL)
			 {
				if(tmp != NULL && !strcmp(SymbolTypeToString(tmp->type),"LIBFUNC"))
				{
					fprintf(errorFile,"ERROR @ line %d: Cannot have a library function as argument\n", yylineno);
					fail_icode = 1;
				}
				else insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1,currfunc, FORMAL));
			}
			else if(tmp != NULL && !strcmp(SymbolTypeToString(tmp->type),"FORMAL") && tmp->isActive == 1)
			{
				fprintf(errorFile,"ERROR @ line %d: Cannot have the same argument names\n", yylineno);
				fail_icode = 1;
			}
			else insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1, currfunc,FORMAL));
	   
		}	
       ;

ifstmt : ifexpr statement { edit_quad(jump_label, NULL, NULL, newconstnumexpr((double)currQuad+1));} elseexpr
       ;

ifexpr : IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {
		emit(if_eq, newconstboolexpr(VAR_TRUE),newconstnumexpr(currQuad+3), $3, yylineno);
		jump_label = currQuad;
		emit(jump, NULL, NULL, NULL, yylineno);
		}
		;
elseexpr : 
		| ELSE statement
		;

whilestmt : WHILE LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {loop_flag = 1;} statement {loop_flag = 0;}
          ;

forstmt : FOR LEFT_PARENTHESIS elist SEMICOLON expr SEMICOLON elist RIGHT_PARENTHESIS {loop_flag = 1;} statement {loop_flag = 0;}
        ;

returnstmt : RETURN expr SEMICOLON {flag_func = 0;}
           | RETURN SEMICOLON
           ;


%%


int yyerror (char* yaccProvidedMessage)
{
	fprintf(stderr, "%s: at line %d, before token: '%s'\n", yaccProvidedMessage, yylineno, yytext);
}

void printusage(){
        fprintf(stderr, "Invalid arguments provided.\nUsage: [program_name] <input_file_name> <output_file_name> <args> <error_log>\n");
        fprintf(stderr, "Ommiting input file or output file will send I/O to stdin and stdout.\n");
        fprintf(stderr, "<args> can be -s to print symtable sorted by scopes, -t to print symtable as a hash table, or nothing to print both.\n");
        fprintf(stderr, "<error_log> can be -log_err to export a txt with error logs , or nothing for errors to be printed to stderr.\n");
}

int main(int argc, char** argv)
{
        table = init_SymTable();
        char * args = NULL;
        char * error_output = NULL;
        char c;
        
       	if (argc == 5){
		if( !(yyin = fopen(argv[1], "r")) ) {
			fprintf(stderr, "Cannot Open Input File: %s\nAccepting input from stdin...\n", argv[1]);
                        yyin = stdin;
		}
		if(!(yyout = fopen(argv[2], "w")) )
		{
			fprintf(stderr, "Cannot Open Output File: %s\nOutput in stdout...\n", argv[2]);
			yyout = stdout;
		}
                args = argv[3];
                error_output = argv[4];
	}
        else if(argc == 4)
        {
                if( !(yyin = fopen(argv[1], "r")) ) {
			fprintf(stderr, "Cannot Open Input File: %s\nAccepting input from stdin...\n", argv[1]);
                        yyin = stdin;
		}
                
                /* <args> && <error_log>*/
                if((strcmp(argv[2],"-s") == 0 || strcmp(argv[2],"-t") == 0) && strcmp(argv[3],"-log_err") == 0)
                { 
                        args = argv[2];
                        error_output = argv[3];

                }
                /* output file && ( <args> || <error_log>) */ 
                else
                {
                        /* output file && <args> */
                        if(strcmp(argv[3],"-s") == 0 || strcmp(argv[3],"-t") == 0)
                        {
                                args = argv[3];
                        }
                        /* output file && <error_log> */
                        else if(strcmp(argv[3],"-log_err") == 0)
                        {
                                error_output = argv[3];
                        }
                        /* error invalid argument */
                        else
                        {
                                printusage();
                                return -1;
                        }

                        if(!(yyout = fopen(argv[2], "w")) )
		        {
			        fprintf(stderr, "Cannot Open Output File: %s\nOutput in stdout...\n", argv[2]);
			        yyout = stdout;
		        }

                }

        }
	else if (argc == 3){
		
                if( !(yyin = fopen(argv[1], "r")) ) {
			fprintf(stderr, "Cannot Open Input File: %s\nAccepting input from stdin...\n", argv[1]);
			yyin = stdin;
		}

                /* <args> */
                if(strcmp(argv[2],"-s") == 0 || strcmp(argv[2],"-t") == 0)
                {
                        args = argv[2];
                }
                /* <error_log>*/
                else if(strcmp(argv[2],"-log_err") == 0)
                {
                        error_output = argv[2];
                }
                /* output */
                else
                {
                         if(!(yyout = fopen(argv[2], "w")) )
		        {
			        fprintf(stderr, "Cannot Open Output File: %s\nOutput in stdout...\n", argv[2]);
			        yyout = stdout;
		        }
                }
	}
        else if(argc == 2)
        {
                if( !(yyin = fopen(argv[1], "r")) ) {
			fprintf(stderr, "Cannot Open Input File: %s\nAccepting input from stdin...\n", argv[1]);
                        yyin = stdin;
		}
        }
	else{
		printusage();
		return -1;
	}

        if(error_output != NULL && strcmp(error_output,"-log_err") == 0) errorFile = fopen("error_log.txt", "w");
        else if(error_output == NULL) errorFile = stderr;
        else
        {
                 printusage();
                 return -1;
        }

        /* add library functions */
        insert_SymTable(table,new_SymTabEntry("print",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("input",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
		insert_SymTable(table,new_SymTabEntry("objectmemberkeys",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("objecttotalmembers",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("objectcopy",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("totalarguments",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("argument",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("typeof",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("strtonum",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("sqrt",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("cos",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("sin",0,1,new_Variable(NULL),new_Function(NULL),0,0,LIBFUNC));
        yyparse();

        if(args != NULL && strcmp(args, "-s") == 0) {}
                //print_Scopes(table);
        else if(args != NULL && strcmp(args, "-t") == 0) {}
                //print_SymTable(table);
        else if(args == NULL){
                //print_SymTable(table);
                //print_Scopes(table);
        } else printusage();

		if(!fail_icode)
        	print_quads(stdout);
		else
			printf("icode generation failed\n");

	return 0;
}