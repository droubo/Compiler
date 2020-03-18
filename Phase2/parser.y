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

stmt : expr SEMICOLON {fprintf(yyout,"stmt -> expr ;\n");}
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
     | expr op expr {fprintf(yyout,"expr -> expr op expr\n");}
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
   | EQUAL {fprintf(yyout,"op -> ==\n");}
   | NOT_EQUAL {fprintf(yyout,"op -> !=\n");}
   | AND {fprintf(yyout,"op -> and\n");}
   | OR {fprintf(yyout,"op -> or\n");}
   ;

term : LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {fprintf(yyout,"term -> ( expr )\n");}
     | MINUS expr {fprintf(yyout,"term -> - expr\n");}
     | NOT expr {fprintf(yyout,"term -> ! expr\n");}
     | PLUS_PLUS lvalue {fprintf(yyout,"term -> ++ lvalue\n");}
     | lvalue PLUS_PLUS {fprintf(yyout,"term -> lvalue ++\n");}
     | MINUS_MINUS lvalue {fprintf(yyout,"term -- lvalue\n");}
     | lvalue MINUS_MINUS {fprintf(yyout,"lavlue --\n");}
     | primary {fprintf(yyout,"term -> primary\n");}
     ;

assignexpr : lvalue ASSIGN expr {fprintf(yyout,"assignexpr -> lvalue = expr\n");}

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
			if(tmp != NULL){
				if(strcmp(SymbolTypeToString(tmp->type),"LIBFUNC")){
					fprintf(yyout, "ERROR @ line %d: %s is a library function\n", yylineno, $1);
				}
				else if(!strcmp(SymbolTypeToString(tmp->type),"GLOBAL") && tmp->scope != currfunc){
					fprintf(yyout, "ERROR @ line %d: %s cannot be accessed\n",yylineno, $1);
				}
				else if(strcmp(SymbolTypeToString(tmp->type),"USERFUNC")){
					fprintf(yyout, "ERROR @ line %d: %s is already declared as function\n",yylineno, $1);
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
			if(tmp != NULL){
				if(strcmp(SymbolTypeToString(tmp->type),"LIBFUNC")){
					fprintf(yyout, "ERROR @ line %d: %s is a library function\n",yylineno, $2);
				}
				else if(tmp->scope == currscope){
					fprintf(yyout, "ERROR @ line %d: %s already declared\n",yylineno, $2);
				}
				else insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, LOCAL));
			}
			else {
				insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, LOCAL));
			}
		
		}
       | DOUBLE_COLON ID {
				fprintf(yyout,"lvalue -> :: ID\n");
				SymTabEntry *tmp = lookup_SymTable(table, $2);
				if(tmp != NULL){
					if(!strcmp(SymbolTypeToString(tmp->type), "GLOBAL")) fprintf(yyout, "ERROR @ line %d: %s is not global variable\n",yylineno, $2);
				}
				else fprintf(yyout, "ERROR @ line %d: %s is not global variable\n",yylineno, $2);
	    }
       | member {fprintf(yyout,"lvalue -> member\n");}
       ;

member : lvalue DOT ID {fprintf(yyout,"member -> lvalue . ID\n");}
       | lvalue LEFT_BRACKET expr RIGHT_BRACKET {fprintf(yyout,"member -> lvalue [ expr ]\n");}
       | call DOT ID {fprintf(yyout,"member -> call . ID\n");}
       | call LEFT_BRACKET expr RIGHT_BRACKET {fprintf(yyout,"member -> call [ expr ]\n");}
       ;

call : call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {fprintf(yyout,"call -> call ( elist )\n");}
     | lvalue callsuffix {fprintf(yyout,"call -> lvalue callsuffix\n");}
     | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {fprintf(yyout,"call -> ( funcdef ) ( elist )\n");}
     ;

callsuffix : normcall {fprintf(yyout,"callsuffix -> normcall\n");}
           | methodcall {fprintf(yyout,"callsuffix -> methodcall\n");}
           ;

normcall : LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {fprintf(yyout,"normcall -> ( elist )\n");}
         ;

methodcall : DOUBLE_DOT ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {fprintf(yyout,"methodcall -> .. ID ( elist )\n");}
           ;

elist : expr comaexpr {fprintf(yyout,"elist -> expr comaexpr\n");}
      ;

comaexpr : /*empty*/ {fprintf(yyout,"comaexpr -> empty\n");}
         | COMA expr comaexpr {fprintf(yyout,"comaexpr -> , expr comaexpr\n");}
         ;

objectdef : LEFT_BRACKET elist RIGHT_BRACKET {fprintf(yyout,"objectdef -> [ elist ]\n");}
          | LEFT_BRACKET indexed RIGHT_BRACKET {fprintf(yyout,"objectdef [ indexed ]\n");}
          | LEFT_BRACKET RIGHT_BRACKET {fprintf(yyout,"objectdef -> [ ]\n");}
          ;

indexed : indexedelem comaindexedelem {fprintf(yyout,"indexed -> indexedelem comaindexedelem\n");}
        ;

comaindexedelem : /*empty*/ {fprintf(yyout,"comaindexedelem -> empty\n");}
                | COMA indexedelem comaindexedelem {fprintf(yyout,"comaindexedelem -> , indexedelem comaindexedelem\n");}
                ;

indexedelem : LEFT_BRACE expr COLON expr RIGHT_BRACE {fprintf(yyout,"indexedelem -> { expr : expr }\n");}
            ;

block : LEFT_BRACE {
				
				if(currscope != currfunc) insert_SymTable(table, new_SymTabEntry("$0", yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope-1, USERFUNC));

		} stmts RIGHT_BRACE {fprintf(yyout,"block -> { stmts }\n");}
      ;

funcdef : FUNCTION ID {

		currfunc++;
		currscope++;
		fprintf(yyout,"funcdef -> function ID ( idlist ) block\n");
		SymTabEntry *tmp = lookup_SymTableScope(table, currscope-1, $2);
		if(tmp != NULL && strcmp(SymbolTypeToString(tmp->type),"USERFUNC")){
			if(SymbolTypeToString(tmp->type) ==  "LIBFUNC"){
				fprintf(yyout, "ERROR @ line %d: %s is library function\n",yylineno, $2);
			}
			fprintf(yyout, "ERROR @ line %d: %s already declared\n",yylineno, $2);
		}
		else insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, USERFUNC));
		
		} LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block {

			currfunc--;
			currscope--;

		}
        | FUNCTION {currscope++;} LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block {fprintf(yyout,"funcdef -> function ( idlist ) block\n");  currscope--;}
        ;

const : REALCONST {fprintf(yyout,"const -> REALCONST\n");}
      | INTCONST {fprintf(yyout,"const -> INTCONST\n");}
      | STRING {fprintf(yyout,"const -> STRING\n");}
      | NIL {fprintf(yyout,"const -> NIL\n");}
      | TRUE {fprintf(yyout,"const -> TRUE\n");}
      | FALSE {fprintf(yyout,"const -> FALSE\n");}
      ;
 
idlist : /* empty */ {fprintf(yyout,"idlist -> empty\n");}
       | ID comaid {
			fprintf(yyout,"idlist -> ID\n");
			fprintf(yyout,"\nscope: %d\n\n", currscope);
			insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1, FORMAL));
		}
       ;

comaid : COMA ID comaid {fprintf(yyout,"comaid -> , ID comaid\n");
                         fprintf(yyout,"idlist -> ID\n");
			 fprintf(yyout,"\nscope: %d\n\n", currscope);
			 insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope+1, FORMAL));
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

returnstmt : RETURN expr SEMICOLON {fprintf(yyout,"returnstmt -> return expr ;\n");}
           | RETURN SEMICOLON {fprintf(yyout,"returnstmt -> return ;\n");}
           ;


%%

int yyerror (char* yaccProvidedMessage)
{
	fprintf(stderr, "%s: at line %d, before token: '%s'\n", yaccProvidedMessage, yylineno, yytext);
}

void printusage(){
        fprintf(stderr, "Invalid arguments provided.\nUsage: [programname] <inputfilename> <outputfilename> <args>\n");
        fprintf(stderr, "Ommiting input file or output file will send I/O to stdin and stdout.\n");
        fprintf(stderr, "<args> can be -s to print symtable sorted by scopes, -t to print symtable as a hash table, or -st for both.\n");
}

int main(int argc, char** argv)
{
        table = init_SymTable();
        char * args;

	if (argc == 4){
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
	}
	else if (argc == 3){
		if( !(yyin = fopen(argv[1], "r")) ) {
			fprintf(stderr, "Cannot Open Input File: %s\nAccepting input from stdin...\n", argv[1]);
			yyin = stdin;
		}
                args = argv[2];
	}
	else{
		printusage();
		return 0;
	}
        
	yyparse();



        if(!strcmp(args, "-s"))
                print_Scopes(table);
        else if(!strcmp(args, "-t"))
                print_SymTable(table);
        else if(!strcmp(args, "-st")){
                print_SymTable(table);
                print_Scopes(table);
        } else printusage();

	return 0;
}