%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include "symtab/symtable.h"

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
SymTabEntry *global_tmp;

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
%token TRUE
%token FALSE
%token NIL

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
%token GREATER
%token LESS
%token GREATER_EQUAL
%token LESS_EQUAL

%token<real> REALCONST
%token<num> INTCONST
%token STRING

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

%union {int integer; char* id; double real;}

%%

program : stmts  {fprintf(yyout, "program -> stmts\n");}
        ;

stmts : /*empty*/ {fprintf(yyout,"stmts -> empty\n");}
      | stmt stmts {fprintf(yyout,"stmts -> stmt stmts\n");}
      ;

stmt : expr SEMICOLON {fprintf(yyout,"stmt -> expr ;\n"); flag_func = 0;}
     | ifstmt {fprintf(yyout,"stmt -> ifstmt\n");}
     | whilestmt {fprintf(yyout,"stmt -> whilestmt\n");}
     | forstmt {fprintf(yyout,"stmt forstmt\n");}
     | returnstmt {fprintf(yyout,"stmt returnstmt\n");}
     | BREAK SEMICOLON {fprintf(yyout,"stmt -> break;\n");}
     | CONTINUE SEMICOLON {fprintf(yyout,"stmt -> continue;\n");}
     | block {fprintf(yyout,"stmt -> block\n");}
     | funcdef {fprintf(yyout,"stmt -> funcdef\n");}
     | SEMICOLON {fprintf(yyout,"stmt -> ;\n");}
     ;

expr : assignexpr {fprintf(yyout,"expr -> assignexpr\n");}
     | expr op expr {if(flag_func == 1 && flag_op == 0) fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : expr -> expr op expr\n", yylineno); flag_op = 0; flag_func = 0; fprintf(yyout,"expr -> expr op expr\n");}
     | term {fprintf(yyout,"expr -> term\n");}
     ;

op : PLUS {fprintf(yyout,"op -> +\n");}
   | MINUS {fprintf(yyout,"op -> -\n");}
   | MULT {fprintf(yyout,"op -> *\n");}
   | DIV {fprintf(yyout,"op -> /\n");}
   | MOD {fprintf(yyout,"op -> %\n");}
   | GREATER {fprintf(yyout,"op -> >\n");}
   | GREATER_EQUAL {fprintf(yyout,"op -> >=\n");}
   | LESS {fprintf(yyout,"op -> <\n");}
   | LESS_EQUAL {fprintf(yyout,"op -> <=\n");}
   | EQUAL {flag_op = 1; flag_func = 0; fprintf(yyout,"op -> ==\n");}
   | NOT_EQUAL {flag_op = 1; flag_func = 0; fprintf(yyout,"op -> !=\n");}
   | AND {flag_op = 1; flag_func = 0; fprintf(yyout,"op -> and\n");}
   | OR {flag_op = 1; flag_func = 0; fprintf(yyout,"op -> or\n");}
   ;

term : LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {fprintf(yyout,"term -> ( expr )\n");}
     | MINUS expr {if(flag_func == 1) fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : term -> - expr\n", yylineno); flag_func = 0; fprintf(yyout,"term -> - expr\n");}
     | NOT expr {flag_op = 1; flag_func = 0; fprintf(yyout,"term -> not expr\n");}
     | PLUS_PLUS lvalue {if(flag_func == 1) fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : term -> ++ lvalue\n", yylineno); flag_func = 0; fprintf(yyout,"term -> ++ lvalue\n");}
     | lvalue PLUS_PLUS {if(flag_func == 1) fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : term -> lvalue ++\n", yylineno); flag_func = 0; fprintf(yyout,"term -> lvalue ++\n");}
     | MINUS_MINUS lvalue {if(flag_func == 1) fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : term -- lvalue\n", yylineno); flag_func = 0; fprintf(yyout,"term -- lvalue\n");}
     | lvalue MINUS_MINUS {if(flag_func == 1) fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : lavlue --\n", yylineno); flag_func = 0; fprintf(yyout,"lavlue --\n");}
     | primary {fprintf(yyout,"term -> primary\n");}
     ;

assignexpr : lvalue {if(flag_func == 1) fprintf(errorFile,"ERROR @ line %d: Unable to do this operation with function : assignexpr -> lvalue = expr\n", yylineno); flag_func = 0;} ASSIGN expr {fprintf(yyout,"assignexpr -> lvalue = expr\n");}

primary : lvalue {fprintf(yyout,"primary -> lvalue\n");}
        | call {fprintf(yyout,"primary -> call\n");}
        | objectdef {fprintf(yyout,"primary -> objectdef\n");}
        | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS {fprintf(yyout,"primary -> ( funcdef )\n");}
        | const {fprintf(yyout,"primary -> const\n");}
        ;

lvalue : ID {
			fprintf(yyout,"lvalue -> ID\n");
			fprintf(yyout,"\nscope: %d\n\n", currscope);
			SymTabEntry *tmp = lookup_SymTable(table, $1);
			if(tmp != NULL && tmp->isActive == 1){
			/*
				if(!strcmp(SymbolTypeToString(tmp->type),"LIBFUNC")){
					fprintf(errorFile, "ERROR @ line %d: %s is a library function\n", yylineno, $1);
				}
				*/
				if(tmp->scope != 0 && tmp->scope != currfunc && strcmp(SymbolTypeToString(tmp->type),"LIBFUNC") && strcmp(SymbolTypeToString(tmp->type),"USERFUNC")){
					fprintf(errorFile, "ERROR @ line %d: %s cannot be accessed\n",yylineno, $1);
				}
				/*
				else if(!strcmp(SymbolTypeToString(tmp->type),"USERFUNC")){
					fprintf(errorFile, "ERROR @ line %d: %s is already declared as function\n",yylineno, $1);
				}
				*/
				else if(!strcmp(SymbolTypeToString(tmp->type),"LIBFUNC") || !strcmp(SymbolTypeToString(tmp->type),"USERFUNC")){
					flag_func = 1;
					global_tmp = tmp;
				}
			}
			else {
				if(currscope == 0) insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, GLOBAL));
				else insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, LOCAL));
			}
			
		}
       | local ID {
			fprintf(yyout,"lvalue -> local ID\n");

			SymTabEntry *tmp = lookup_SymTable(table, $2);
			if(tmp != NULL && tmp->isActive == 1){
				if(!strcmp(SymbolTypeToString(tmp->type),"LIBFUNC")){
					fprintf(errorFile, "ERROR @ line %d: %s is a library function\n",yylineno, $2);
				}
                                else if(tmp->scope == currscope && strcmp(SymbolTypeToString(tmp->type),"FORMAL") == 0)
                                {
                                        /* do nothing , refer to arguments as local*/
                                }
				else if(tmp->scope == currscope){
					fprintf(errorFile, "ERROR @ line %d: %s already declared\n",yylineno, $2);
				}
				else insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, LOCAL));
			}
			else {
				insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, LOCAL));
			}
		
		}
       | DOUBLE_COLON ID {
				fprintf(yyout,"lvalue -> :: ID\n");
				SymTabEntry *tmp = lookup_SymTableScope(table, 0, $2);
				if(tmp != NULL){
					if(tmp->scope != 0) fprintf(errorFile, "ERROR @ line %d: %s is not a global variable nor a global function\n",yylineno, $2);
                                        
                                        /* found global function */
                                        if(strcmp(SymbolTypeToString(tmp->type),"LIBFUNC") == 0 || strcmp(SymbolTypeToString(tmp->type),"USERFUNC") == 0 ) flag_func = 1;
				}
				else fprintf(errorFile, "ERROR @ line %d: %s is not a global variable nor a global function\n",yylineno, $2);
	    }
       | member {fprintf(yyout,"lvalue -> member\n");}
       ;

member : lvalue DOT ID {fprintf(yyout,"member -> lvalue . ID\n");}
       | lvalue LEFT_BRACKET expr RIGHT_BRACKET {fprintf(yyout,"member -> lvalue [ expr ]\n");}
       | call DOT ID {fprintf(yyout,"member -> call . ID\n");}
       | call LEFT_BRACKET expr RIGHT_BRACKET {fprintf(yyout,"member -> call [ expr ]\n");}
       ;

call : call {flag_func = 0;} LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {fprintf(yyout,"call -> call ( elist )\n");}
     | lvalue {flag_func = 0;} callsuffix {fprintf(yyout,"call -> lvalue callsuffix\n");}
     | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS {flag_func = 0;} LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {fprintf(yyout,"call -> ( funcdef ) ( elist )\n");}
     ;

callsuffix : normcall {fprintf(yyout,"callsuffix -> normcall\n");}
           | methodcall {fprintf(yyout,"callsuffix -> methodcall\n");}
           ;

normcall : LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {fprintf(yyout,"normcall -> ( elist )\n");}
         ;

methodcall : DOUBLE_DOT ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {fprintf(yyout,"methodcall -> .. ID ( elist )\n");}
           ;

elist : /* empty */   {fprintf(yyout,"elist -> empty\n");}
      | expr {flag_func = 0;} comaexpr {fprintf(yyout,"elist -> expr comaexpr\n");}
      ;

comaexpr : /*empty*/ {fprintf(yyout,"comaexpr -> empty\n");}
         | COMA expr comaexpr {fprintf(yyout,"comaexpr -> , expr comaexpr\n");}
         ;

objectdef : LEFT_BRACKET elist RIGHT_BRACKET {fprintf(yyout,"objectdef -> [ elist ]\n");}
          | LEFT_BRACKET indexed RIGHT_BRACKET {fprintf(yyout,"objectdef [ indexed ]\n");}
          ;

indexed : indexedelem comaindexedelem {fprintf(yyout,"indexed -> indexedelem comaindexedelem\n");}
        ;

comaindexedelem : /*empty*/ {fprintf(yyout,"comaindexedelem -> empty\n");}
                | COMA indexedelem comaindexedelem {fprintf(yyout,"comaindexedelem -> , indexedelem comaindexedelem\n");}
                ;

indexedelem : LEFT_BRACE expr COLON expr RIGHT_BRACE {fprintf(yyout,"indexedelem -> { expr : expr }\n");}
            ;

block : LEFT_BRACE { currscope++; } stmts RIGHT_BRACE { hide_Scope(table,currscope); currscope--; fprintf(yyout,"block -> { stmts }\n");}
      ;

funcdef : FUNCTION ID {

		currfunc++;
		fprintf(yyout,"funcdef -> function ID ( idlist ) block\n");
		SymTabEntry *tmp = lookup_SymTableScope(table, currscope, $2);
		if(tmp != NULL){
			if(!strcmp(SymbolTypeToString(tmp->type),"LIBFUNC")){
				fprintf(errorFile, "ERROR @ line %d: %s is library function\n",yylineno, $2);
			}
			else fprintf(errorFile, "ERROR @ line %d: %s already declared\n",yylineno, $2);
		}
		else insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, USERFUNC));
		
		} LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block {

			currfunc--;

		}
        | FUNCTION {currfunc++;
                    char* anonym = (char *)malloc(sizeof(char)*2);
                    sprintf(anonym,"$%d",anonym_func_count++);
                    insert_SymTable(table, new_SymTabEntry(anonym, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, USERFUNC));
                   }  LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS  block {fprintf(yyout,"funcdef -> function ( idlist ) block\n");}
        ;

const : REALCONST {fprintf(yyout,"const -> REALCONST\n");}
      | INTCONST {fprintf(yyout,"const -> INTCONST\n");}
      | STRING {fprintf(yyout,"const -> STRING\n");}
      | NIL {fprintf(yyout,"const -> NIL\n");}
      | TRUE {fprintf(yyout,"const -> TRUE\n");}
      | FALSE {fprintf(yyout,"const -> FALSE\n");}
      ;
 
idlist : /* empty */ {fprintf(yyout,"idlist -> empty\n");}
       | ID {
			 SymTabEntry *tmp = lookup_SymTableScope(table, currscope+1, $1);
			 if(tmp == NULL && (tmp = lookup_SymTableScope(table, 0, $1)) != NULL){
				if(tmp != NULL && !strcmp(SymbolTypeToString(tmp->type),"LIBFUNC")) fprintf(errorFile,"ERROR @ line %d: Cannot have a library function as argument\n", yylineno);
				else insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1, FORMAL));
			}
			 else if(tmp != NULL && !strcmp(SymbolTypeToString(tmp->type),"FORMAL") && tmp->isActive == 1){
				fprintf(errorFile,"ERROR @ line %d: Cannot have the same argument names\n", yylineno);
			 }
			 else insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1, FORMAL));
	   
		}	
	   comaid {
			fprintf(yyout,"idlist -> ID\n");
			fprintf(yyout,"\nscope: %d\n\n", currscope);
		}
       ;

comaid : COMA ID{
			 SymTabEntry *tmp = lookup_SymTableScope(table, currscope+1, $2);
			 if(tmp == NULL && (tmp = lookup_SymTableScope(table, 0, $2)) != NULL){
				if(tmp != NULL && !strcmp(SymbolTypeToString(tmp->type),"LIBFUNC")) fprintf(errorFile,"ERROR @ line %d: Cannot have a library function as argument\n", yylineno);
				else insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1, FORMAL));
			}
			 else if(tmp != NULL && !strcmp(SymbolTypeToString(tmp->type),"FORMAL") && tmp->isActive == 1){
				fprintf(errorFile,"ERROR @ line %d: Cannot have the same argument names\n", yylineno);
			 }
			 else insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1, FORMAL));
}
	comaid {fprintf(yyout,"comaid -> , ID comaid\n");
                         fprintf(yyout,"idlist -> ID\n");
			 fprintf(yyout,"\nscope: %d\n\n", currscope);
                        }
       | {fprintf(yyout,"comaid -> empty\n");}
       ;

ifstmt : IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt ELSE stmt {fprintf(yyout,"ifstmt -> IF ( expr ) stmt ELSE stmt\n");}
       | IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt {fprintf(yyout,"ifstmt -> IF ( expr ) stmt\n");}
       ;

whilestmt : WHILE LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt {fprintf(yyout,"whilestmt -> WHILE ( expr ) stmt\n");}
          ;

forstmt : FOR LEFT_PARENTHESIS elist SEMICOLON expr SEMICOLON elist RIGHT_PARENTHESIS stmt {fprintf(yyout,"forstmt -> FOR ( elist ; expr ; elist ) stmt\n");}
        ;

returnstmt : RETURN expr SEMICOLON {fprintf(yyout,"returnstmt -> return expr ;\n"); flag_func = 0;}
           | RETURN SEMICOLON {fprintf(yyout,"returnstmt -> return ;\n");}
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
        insert_SymTable(table,new_SymTabEntry("print",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("input",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
	insert_SymTable(table,new_SymTabEntry("objectmemberkeys",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("objecttotalmembers",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("objectcopy",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("totalarguments",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("argument",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("typeof",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("strtonum",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("sqrt",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("cos",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        insert_SymTable(table,new_SymTabEntry("sin",0,1,new_Variable(NULL),new_Function(NULL),0,LIBFUNC));
        yyparse();

        if(args != NULL && strcmp(args, "-s") == 0)
                print_Scopes(table);
        else if(args != NULL && strcmp(args, "-t") == 0)
                print_SymTable(table);
        else if(args == NULL){
                print_SymTable(table);
                print_Scopes(table);
        } else printusage();

	return 0;
}