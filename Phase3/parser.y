%{
/**
 * CS-340 Compilers
 * Spring 2020
 * 
 * Project Phase 3: Intermediate code Generation
 * 
 * Antonis Ntroumpogiannis    csd4014@csd.uoc.gr
 * Georgios Marios Kokiadis csd3962@csd.uoc.gr
 * Nikolaos Gounakis        csd3932@csd.uoc.gr
 * 
 * parser.y
 * Parser file to be used with yacc/bison
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include "quads/temp.h"
#include "offset/offset.h"
#include "symtab/symtable.h"
#include "statement/statement.h"

#define YY_DECL int alpha_yylex(void *yylval)

extern int yylineno;
extern char *yyval;
extern char *yytext;
extern FILE *yyin;
extern FILE *yyout;
SymTable *table;
unsigned int currscope = 0;
unsigned int currfunc = 0;
unsigned int anonym_func_count = 0;
unsigned int flag_op = 0;
unsigned int fail_icode = 0;
unsigned int return_flag = 0;
unsigned int loop_flag = 0;
unsigned int bool_extra = 0;
SymTabEntry *global_tmp;
int table_flag = 0;
int jump_label = 0;
int loopcounter = 0;
loopcounterstack *lcs = NULL;
int else_flag = 0;
FILE *errorFile;
int tmp_args = 0;

extern int tempcounter;

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
%token GREATER_EQUAL 
%token GREATER 
%token LESS_EQUAL 
%token LESS
%token NOT_EQUAL

%token AND 
%token OR 
%token NOT

%token PLUS_PLUS 
%token MINUS_MINUS 


%token<real> REALCONST 
%token<real> INTCONST 
%token<id> STRING

%token LEFT_BRACE 
%token RIGHT_BRACE
%token LEFT_BRACKET 
%token RIGHT_BRACKET 
%token LEFT_PARENTHESIS 
%token RIGHT_PARENTHESIS

%token SEMICOLON 
%token COMA 
%token COLON 
%token DOUBLE_COLON 
%token DOT 
%token DOUBLE_DOT

%token<id> ID

%type<express> lvalue 
%type<express> primary 
%type<express> term 
%type<express> expr 
%type<express> assignexpr 
%type<express> const 
%type<express> member
%type<express> tablemake 
%type<express> elist 
%type<express> call 
%type<express> tableitem 
%type<sym> funcdef 
%type<integer> ifexpr 
%type<integer> whilestart 
%type<integer> whilecond 
%type<integer> forprefix 
%type<integer> N 
%type<integer> M_
%type<stmt> statements
%type<stmt> statement 
%type<stmt> break 
%type<stmt> continue_ 
%type<stmt> loopstmt 
%type<stmt> block 
%type<stmt> ifstmt 
%type<stmt> whilestmt 
%type<stmt> forstmt 
%type<stmt> elseexpr 
%type<sym> funcprefix 
%type<integer> funcbody 
%type<id> funcname 
%type<call_> callsuffix 
%type<call_> normcall 
%type<call_> methodcall
%type<integer> funcargs
%type<hashElement> indexed
%type<hashElement> comaindexedelem
%type<hashElement> indexedelem

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

%code requires {#include "quads/quads.h"}

%union {
	stmt_t stmt;
	M *_M;
	double integer;
	char *id;
	double real;
	expr *express;
	unsigned char bool;
	iopcode ipc;
	SymTabEntry *sym;
	call_struct call_;
	hashTableElement* hashElement;
}
%%

program : 
statements
;

statements: 
	{ make_stmt(&$$); }
	| statement {resettemp();} statements
	{	
		$$.breakList = mergelist($3.breakList, $1.breakList);
		$$.contList = mergelist($3.contList, $1.contList);

	}

| error_statement statements
;

error_statement : error statement;

statement : 
	expr SEMICOLON { make_stmt(&$$);}
	| ifstmt { $$ = $1;}
	| whilestmt {$$ = $1;}
	| forstmt { $$ = $1; }
	| returnstmt
	{
		if (return_flag == 0)
		{
			fprintf(errorFile, "ERROR @ line %d: Cannot use return outside of a function\n", yylineno);
			fail_icode = 1;
		}
		make_stmt(&$$);
		
	}
	| break { $$ = $1; }
	| continue_ { $$ = $1; }
	| block { $$ = $1; }
	| funcdef { make_stmt(&$$);}
	| SEMICOLON { make_stmt(&$$);};
	;

break : 
	BREAK SEMICOLON
	{
		if (loopcounter == 0)
		{
			fprintf(errorFile, "ERROR @ line %d: Cannot use break outside of a loop\n", yylineno);
			fail_icode = 1;
		}
		make_stmt(&$$);
		$$.breakList = newlist(currQuad);
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	continue_ : CONTINUE SEMICOLON
	{
		if (loopcounter == 0)
		{
			fprintf(errorFile, "ERROR @ line %d: Cannot use continue outside of a loop\n", yylineno);
			fail_icode = 1;
		}
		make_stmt(&$$);
		$$.contList = newlist(currQuad);
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	;

expr : 
	assignexpr
	| expr PLUS expr
	{
		if ($1->type == programfunc_e || $1->type == libraryfunc_e || $3->type == programfunc_e || $3->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : expr -> expr + expr\n", yylineno);
			fail_icode = 1;
		}
		flag_op = 0;

		SymTabEntry *tmp = (SymTabEntry *)newtemp(table, currscope, currfunc);
		$$ = newexpr(arithexpr_e, tmp);
		emit(add, $1, $3, $$, yylineno);
	}
	| expr MINUS expr
	{
		if ($1->type == programfunc_e || $1->type == libraryfunc_e || $3->type == programfunc_e || $3->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : expr -> expr - expr\n", yylineno);
			fail_icode = 1;
		}
	
		SymTabEntry *tmp = (SymTabEntry *)newtemp(table, currscope, currfunc);
		$$ = newexpr(arithexpr_e, tmp);
		emit(sub, $1, $3, $$, yylineno);
	}
	| expr MULT expr
	{
		if ($1->type == programfunc_e || $1->type == libraryfunc_e || $3->type == programfunc_e || $3->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : expr -> expr * expr\n", yylineno);
			fail_icode = 1;
		}
	
		SymTabEntry *tmp = (SymTabEntry *)newtemp(table, currscope, currfunc);
		$$ = newexpr(arithexpr_e, tmp);
		emit(mul, $1, $3, $$, yylineno);
	}
	| expr DIV expr
	{
		if ($1->type == programfunc_e || $1->type == libraryfunc_e || $3->type == programfunc_e || $3->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : expr -> expr / expr\n", yylineno);
			fail_icode = 1;
		}
		
		SymTabEntry *tmp = (SymTabEntry *)newtemp(table, currscope, currfunc);
		$$ = newexpr(arithexpr_e, tmp);
		emit(diva, $1, $3, $$, yylineno);
	}
	| expr MOD expr
	{
		if ($1->type == programfunc_e || $1->type == libraryfunc_e || $3->type == programfunc_e || $3->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : expr -> expr MOD expr\n", yylineno);
			fail_icode = 1;
		}
	
		SymTabEntry *tmp = (SymTabEntry *)newtemp(table, currscope, currfunc);
		$$ = newexpr(arithexpr_e, tmp);
		emit(mod, $1, $3, $$, yylineno);
	}
	| expr GREATER expr
	{
		if ($1->type == programfunc_e || $1->type == libraryfunc_e || $3->type == programfunc_e || $3->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : expr -> expr > expr\n", yylineno);
			fail_icode = 1;
		}
		flag_op = 1;
		
		$$ = newexpr(boolexpr_e, NULL);
		$$->truelist = booleanList_makeList(currQuad);
		$$->falselist = booleanList_makeList(currQuad + 1);
		emit_jump(if_greater, $1, $3, 0, yylineno);
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	| expr GREATER_EQUAL expr
	{
		if ($1->type == programfunc_e || $1->type == libraryfunc_e || $3->type == programfunc_e || $3->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : expr -> expr >= expr\n", yylineno);
			fail_icode = 1;
		}
		flag_op = 1;
	
		$$ = newexpr(boolexpr_e, NULL);
		$$->truelist = booleanList_makeList(currQuad);
		$$->falselist = booleanList_makeList(currQuad + 1);
		emit_jump(if_greatereq, $1, $3, 0, yylineno);
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	| expr LESS expr
	{
		if ($1->type == programfunc_e || $1->type == libraryfunc_e || $3->type == programfunc_e || $3->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : expr -> expr < expr\n", yylineno);
			fail_icode = 1;
		}
		flag_op = 1;
	
		$$ = newexpr(boolexpr_e, NULL);
		$$->truelist = booleanList_makeList(currQuad);
		$$->falselist = booleanList_makeList(currQuad + 1);
		emit_jump(if_less, $1, $3, 0, yylineno);
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	| expr LESS_EQUAL expr
	{
		if ($1->type == programfunc_e || $1->type == libraryfunc_e || $3->type == programfunc_e || $3->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : expr -> expr <= expr\n", yylineno);
			fail_icode = 1;
		}
		flag_op = 1;
	
		$$ = newexpr(boolexpr_e, NULL);
		$$->truelist = booleanList_makeList(currQuad);
		$$->falselist = booleanList_makeList(currQuad + 1);
		emit_jump(if_lesseq, $1, $3, 0, yylineno);
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	| expr EQUAL expr
	{
		flag_op = 1;

		$$ = newexpr(boolexpr_e, NULL);
		$$->truelist = booleanList_makeList(currQuad);
		$$->falselist = booleanList_makeList(currQuad + 1);
		emit_jump(if_eq, $1, $3, 0, yylineno);
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	| expr NOT_EQUAL expr
	{
		flag_op = 1;
	
		$$ = newexpr(boolexpr_e, NULL);
		$$->truelist = booleanList_makeList(currQuad);
		$$->falselist = booleanList_makeList(currQuad + 1);
		emit_jump(if_noteq, $1, $3, 0, yylineno);
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	| expr OR{
		flag_op = 1;
		if($1->type != boolexpr_e){
			$1->truelist = booleanList_makeList(currQuad);
			$1->falselist = booleanList_makeList(currQuad + 1);
			emit_jump(if_eq, $1, newconstboolexpr(VAR_TRUE), 42069, yylineno);
			emit_jump(jump, NULL, NULL, 0, yylineno);
		}
	}
	M_ { $M_ = currQuad + 1; } expr
	{
printf("> IN OR M_ = %d\n", $M_);
		if ($6->type != boolexpr_e) {
			$6->truelist = booleanList_makeList(currQuad);
			$6->falselist = booleanList_makeList(currQuad + 1);
			emit_jump(if_eq, $6, newconstboolexpr(VAR_TRUE), 42069, yylineno);
			emit_jump(jump, NULL, NULL, 0, yylineno);
		}

printf("IN OR\n");
printf("BOOL_EXTRA = %d\n", bool_extra);

		backpatch($1->falselist, $M_);
		$$ = newexpr(boolexpr_e, NULL);
		$$->truelist = booleanList_merge($1->truelist, $6->truelist);
		$$->falselist = $6->falselist;
	}

	| expr AND{
		flag_op = 1;
		if($1->type != boolexpr_e){
			$1->truelist = booleanList_makeList(currQuad);
			$1->falselist = booleanList_makeList(currQuad + 1);
			emit_jump(if_eq, $1, newconstboolexpr(VAR_TRUE), 420, yylineno);
			emit_jump(jump, NULL, NULL, 0, yylineno);
		}
	}
	M_ { $M_ = currQuad + 1; } expr
	{
printf("> IN AND M_ = %d\n", $M_);
		if ($6->type != boolexpr_e) {
			$6->truelist = booleanList_makeList(currQuad);
			$6->falselist = booleanList_makeList(currQuad + 1);
			emit_jump(if_eq, $6, newconstboolexpr(VAR_TRUE), 420, yylineno);
			emit_jump(jump, NULL, NULL, 0, yylineno);
		}

printf("IN AND\n");
printf("BOOL_EXTRA = %d\n", bool_extra);

		backpatch($1->truelist, $M_);
		$$ = newexpr(boolexpr_e, NULL);
		$$->truelist = $6->truelist;
		$$->falselist = booleanList_merge($1->falselist, $6->falselist);
	}
	| NOT expr
	{
		flag_op = 1;
	
		if($2->type != boolexpr_e){
			$2->truelist = booleanList_makeList(currQuad);
			$2->falselist = booleanList_makeList(currQuad + 1);
			emit_jump(if_eq, $2, newconstboolexpr(VAR_TRUE), 0, yylineno);
			emit_jump(jump, NULL, NULL, 0, yylineno);
		}	
		
		$$ = newexpr(boolexpr_e, NULL);
		$$->truelist = $2->falselist;
		$$->falselist = $2->truelist;
	}
	| term { $$ = $1;}
	;

term :
	LEFT_PARENTHESIS expr RIGHT_PARENTHESIS { $$ = $2; }
	| MINUS expr
	{
		if ($expr->type == programfunc_e || $expr->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : term -> - expr\n", yylineno);
			fail_icode = 1;
		}
		
		expr *temp = newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
		emit(uminus, $2, NULL, temp, yylineno);
		
		$$ = temp;
	}
	%prec UMINUS
	| PLUS_PLUS lvalue
	{
		if ($lvalue->type == programfunc_e || $lvalue->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : term -> ++ lvalue\n", yylineno);
			fail_icode = 1;
		}

		emit(add, $2, newconstnumexpr(1), $2, yylineno);
		emit(assign, $2, NULL, newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc)), yylineno);
		$$ = $2;
	}
	| lvalue PLUS_PLUS
	{
		if ($lvalue->type == programfunc_e || $lvalue->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : term -> lvalue ++\n", yylineno);
			fail_icode = 1;
		}

		emit(assign, $1, NULL, newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc)), yylineno);
		emit(add, $1, newconstnumexpr(1), $1, yylineno);

		$$ = $1;
	}
	| MINUS_MINUS lvalue
	{
		if ($lvalue->type == programfunc_e || $lvalue->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : term -- lvalue\n", yylineno);
			fail_icode = 1;
		}

		emit(sub, $2, newconstnumexpr(1), $2, yylineno);
		emit(assign, $2, NULL, newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc)), yylineno);

		$$ = $2;
	}
	| lvalue MINUS_MINUS
	{
		if ($lvalue->type == programfunc_e || $lvalue->type == libraryfunc_e)
		{
			fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : lavlue --\n", yylineno);
			fail_icode = 1;
		}
	
		emit(assign, $1, NULL, newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc)), yylineno);
		emit(sub, $1, newconstnumexpr(1), $1, yylineno);
		$$ = $1;
	}
	| primary { $$ = $1; }
	;

assignexpr : 
	lvalue
    {
        if ($lvalue->type == programfunc_e || $lvalue->type == libraryfunc_e)
        {
            fprintf(errorFile, "ERROR @ line %d: Unable to do this operation with function : assignexpr -> lvalue = expr\n", yylineno);
            fail_icode = 1;
        }
        table_flag = 1;
    }
	ASSIGN expr
	{
		if ($expr->type == boolexpr_e)
		{
			expr *temp;
			temp = newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
			emit(assign, newconstboolexpr(VAR_TRUE), NULL, temp, yylineno);
			emit_jump(jump, NULL, NULL, currQuad + 3, yylineno);
			emit(assign, newconstboolexpr(VAR_FALSE), NULL, temp, yylineno);
			backpatch($expr->truelist, currQuad - 2);
			backpatch($expr->falselist, currQuad);
			emit(assign, temp, NULL, $1, yylineno);
		}
		else if ($1->index != NULL)
		{
			emit(tablesetelem, $1->index, $4, $1, yylineno);
			emit_iftableitem($lvalue, table, currscope, currfunc, yylineno);
		}
		else
		{
			char name[20];
			/* temptcounter - 1 is the current tmp variable */
			sprintf(name,"_t%d",tempcounter-1);
			SymTabEntry* tmp_entry = lookup_SymTableScope(table, currscope, name);
			
			if(tempcounter == 0 || (tempcounter > 0 && tmp_entry == NULL))
			{
 				emit(assign, $4, NULL, $1, yylineno);
 			}
			else if(tempcounter > 0 && tmp_entry != NULL)
			{
				expr* tmp_expr = newexpr(var_e, tmp_entry);
				emit(assign, tmp_expr, NULL, $1, yylineno);
			}
			else
			{
				assert(0);
			}
		}
		/* second emit if not table */
		if($1->index == NULL)
		{
			expr* temp = newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
			emit(assign,$lvalue,NULL,temp,yylineno);
		}

		table_flag = 0;
	}
	;

primary : 
	lvalue { $$ = emit_iftableitem($lvalue, table, currscope, currfunc, yylineno); }
	| call 
	| LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
	{
		$$ = newexpr(programfunc_e, $funcdef);
	}
	| const 
	| tablemake
	;

lvalue : 
	ID
	{
		SymTabEntry *tmp = lookup_SymTable(table, $ID);
		if (tmp != NULL && tmp->isActive == 1)
		{
			if (tmp->scope != 0 && tmp->func_scope != currfunc && strcmp(SymbolTypeToString(tmp->type), "LIBFUNC") && strcmp(SymbolTypeToString(tmp->type), "USERFUNC"))
			{
				fprintf(errorFile, "ERROR @ line %d: %s cannot be accessed\n", yylineno, $1);
				fail_icode = 1;
			}
			/* is function */
			else if (!strcmp(SymbolTypeToString(tmp->type), "LIBFUNC") || !strcmp(SymbolTypeToString(tmp->type), "USERFUNC"))
			{
				global_tmp = tmp;
			}
		}
		/* variable is not active but exists in another block*/
		else if (tmp != NULL && tmp->isActive == 0)
		{
			SymTabEntry* entry = lookup_SymTableScope(table,0,tmp->name);

			/* you are refearing to global */
			if(entry != NULL)
			{
				tmp = entry;
			}
			/* the global doesnt exist , so insert new */
			else
			{
				SymbolType type;
				if (currscope == 0)
					type = GLOBAL;
				else
					type = LOCAL;
				tmp = insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, currfunc, type));
				tmp->space = currscopespace();
				tmp->offset = currscopeoffset();
				inccurrscopeoffset();
				printf("var : %s | scope : %d | space : %d | offset : %d\n", tmp->name, tmp->scope, tmp->space, tmp->offset);
			}
		}
		else if (tmp == NULL)
		{
			/* insertion scope = 0 */
			if (currscope == 0)
			{
				tmp = insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, currfunc, GLOBAL));
			}
			/* insertion scope > 0 */
			else
			{
				tmp = insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, currfunc, LOCAL));
			}
			tmp->space = currscopespace();
			tmp->offset = currscopeoffset();
			inccurrscopeoffset();
			printf("var : %s | scope : %d | space : %d | offset : %d\n", tmp->name, tmp->scope, tmp->space, tmp->offset);
		}

		$$ = lvalue_expr(tmp);
	}
	| local ID
	{
		SymTabEntry *tmp = lookup_SymTable(table, $2);
		if (tmp != NULL && tmp->isActive == 1)
		{
			if (!strcmp(SymbolTypeToString(tmp->type), "LIBFUNC"))
			{
				fprintf(errorFile, "ERROR @ line %d: %s is a library function\n", yylineno, $2);
				fail_icode = 1;
			}
			else if (tmp->scope == currscope && strcmp(SymbolTypeToString(tmp->type), "FORMAL") == 0)
			{
				/* do nothing , refer to arguments as local*/
			}
			else if (tmp->scope == currscope)
			{
				fprintf(errorFile, "ERROR @ line %d: %s already declared\n", yylineno, $2);
				fail_icode = 1;
			}
			/* insert local on different scope */
			else
			{
				tmp = insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, currfunc, LOCAL));
				tmp->space = currscopespace();
				tmp->offset = currscopeoffset();
				inccurrscopeoffset();
				printf("var : %s | scope : %d | space : %d | offset : %d\n", tmp->name, tmp->scope, tmp->space, tmp->offset);
			}
		}
		/* insert local */
		else if (tmp == NULL)
		{
			tmp = insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, currfunc, LOCAL));
			tmp->space = currscopespace();
			tmp->offset = currscopeoffset();
			inccurrscopeoffset();
			printf("var : %s | scope : %d | space : %d | offset : %d\n", tmp->name, tmp->scope, tmp->space, tmp->offset);
		}
		$$ = lvalue_expr(tmp);
	}
	| DOUBLE_COLON ID
	{
		SymTabEntry *tmp = lookup_SymTableScope(table, 0, $2);
		if (tmp != NULL)
		{
			if (tmp->scope != 0)
			{
				fprintf(errorFile, "ERROR @ line %d: %s is not a global variable nor a global function\n", yylineno, $2);
				fail_icode = 1;
			}
			/* found global function */
			if (strcmp(SymbolTypeToString(tmp->type), "LIBFUNC") == 0 || strcmp(SymbolTypeToString(tmp->type), "USERFUNC") == 0)
			{
				/* do nothing here */
			}
		}
		else
		{
			fprintf(errorFile, "ERROR @ line %d: %s is not a global variable nor a global function\n", yylineno, $2);
			fail_icode = 1;
		}

		$$ = lvalue_expr(tmp);
	}
	| member { $$ = $1; }
	| tableitem { $$ = $1; }
	;

member : 
	call DOT ID { $$ = member_item($call, $ID, table, currscope, currfunc, yylineno); }
	| call LEFT_BRACKET expr RIGHT_BRACKET
	{
		$call = emit_iftableitem($call, table, currscope, currfunc, yylineno);
		expr *item = newexpr(tableitem_e, $call->sym);
		item->index = $expr;
		$$ = item;
	}
	;

tableitem : 
	lvalue DOT ID
	{
		$$ = member_item($lvalue, $ID, table, currscope, currfunc, yylineno);
	}
	| lvalue LEFT_BRACKET expr RIGHT_BRACKET
	{
		$lvalue = emit_iftableitem($lvalue, table, currscope, currfunc, yylineno);
		expr *item = newexpr(tableitem_e, $lvalue->sym);
		item->index = $expr;
		$$ = item;
	}
	tablemake : LEFT_BRACKET elist RIGHT_BRACKET
	{
		expr *tmp = newexpr(newtable_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
		emit(tablecreate, tmp, NULL, NULL, yylineno);
		int i;
		for (i = 0; $elist; $elist = $elist->next)
		{
			emit(tablesetelem ,newconstnumexpr(i++) ,$elist ,tmp ,yylineno);
		}
		$$ = tmp;
	}
	| LEFT_BRACKET indexed RIGHT_BRACKET {
			expr *tmp = newexpr(newtable_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
			emit(tablecreate, tmp, NULL, NULL, yylineno);
			int i = 0;
			for (i = 0; $indexed; $indexed = $indexed->next)
			{
				emit(tablesetelem ,$indexed->key ,$indexed->value ,tmp ,yylineno);
			}
			$$ = tmp;
	}
	;


	call : call
	{
		
	}
    LEFT_PARENTHESIS elist RIGHT_PARENTHESIS { $$ = make_call($1, $elist, &table, yylineno, currscope, currfunc); }
	| lvalue {}
	callsuffix
	{
		$lvalue = emit_iftableitem($lvalue, table, currscope, currfunc, yylineno);
		if ($callsuffix.method)
		{
			expr *t = $lvalue;
			$lvalue = emit_iftableitem(member_item(t, $callsuffix.name, table, currscope, currfunc, yylineno), table, currscope, currfunc, yylineno);
			if($callsuffix.elist != NULL) $callsuffix.elist->next = t;
		}
		$call = make_call($lvalue, $callsuffix.elist, &table, yylineno, currscope, currfunc);
	}
	| LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS { }
	LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
	{
		expr *func = newexpr(programfunc_e, $funcdef);
		$call = make_call(func, $elist, &table, yylineno, currscope, currfunc);
	}
	;

callsuffix : 
	normcall { $$ = $1; }
	| methodcall { $$ = $1; };

normcall : 
	LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
	{
		$$.elist = $elist;
		$$.method = 0;
		$$.name = NULL;
	}
	;

methodcall : 
	DOUBLE_DOT ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
	{
		$$.elist = $elist;
		$$.method = 1;
		$$.name = $ID;
	}
	;

elist : {$$ = NULL;}
	| elist COMA expr {}
	{
		expr * list_tail;
		list_tail = $1;
		while(list_tail->next != NULL){
			list_tail = list_tail->next;
		}

		
		if ($expr->type == boolexpr_e)
		{
			expr *temp;
			temp = newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
			emit(assign, newconstboolexpr(VAR_TRUE), NULL, temp, yylineno);
			emit_jump(jump, NULL, NULL, currQuad + 3, yylineno);
			emit(assign, newconstboolexpr(VAR_FALSE), NULL, temp, yylineno);
			backpatch($expr->truelist, currQuad - 2);
			backpatch($expr->falselist, currQuad);
			$expr = temp;
			
		}

		list_tail->next = $expr;
		$$ = $1;
	}
	| expr {}
	{
		if ($expr->type == boolexpr_e)
		{
			expr *temp;
			temp = newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
			emit(assign, newconstboolexpr(VAR_TRUE), NULL, temp, yylineno);
			emit_jump(jump, NULL, NULL, currQuad + 3, yylineno);
			emit(assign, newconstboolexpr(VAR_FALSE), NULL, temp, yylineno);
			backpatch($expr->truelist, currQuad - 2);
			backpatch($expr->falselist, currQuad);
			$$ = temp;
		} else {
			$$ = $expr;
		}
	}
	;

indexed : indexedelem comaindexedelem {
	$$ = $indexedelem;
	$$->next = $comaindexedelem;
}
;

comaindexedelem : {$$ = NULL;} 
	| COMA indexedelem comaindexedelem {
	$$ = $indexedelem;
	$$->next = $3;
}
;

indexedelem : LEFT_BRACE expr COLON expr RIGHT_BRACE {
	$$ = newHashTableElement($2, $4);
}
;
block : 
	LEFT_BRACE { currscope++; }
	statements RIGHT_BRACE
	{
		hide_Scope(table, currscope);
		currscope--;
		$$ = $statements;
	}
	;

funcdef : 
	funcprefix funcargs funcbody
	{
		/* out of function block , we cant use return here */
		return_flag--;

		/* offset code when exiting function definition */
		exitscopespace();
		$funcprefix->value.funcVal->num_of_locals = $funcbody;
		$funcprefix->value.funcVal->num_of_args = $funcargs;
		restorecurroffset(pop());

		printf("function : %s , total args : %d , total locals : %d\n",$funcprefix->name,$funcprefix->value.funcVal->num_of_args,$funcprefix->value.funcVal->num_of_locals);

		$$ = $funcprefix;
		emit(funcend, NULL, NULL, newexpr(programfunc_e, $funcprefix), yylineno);
		edit_quad($funcprefix->value.funcVal->jump_label, NULL, NULL, NULL, currQuad + 1);
		currfunc--;
	}
	;

funcprefix : 
	FUNCTION funcname
	{
		jump_label = currQuad;
		emit_jump(jump, NULL, NULL, 0, yylineno);

		currfunc++;
		$$ = lookup_SymTableScope(table, currscope, $2);
		if ($$ != NULL)
		{
			if (!strcmp(SymbolTypeToString($$->type), "LIBFUNC"))
			{
				fprintf(errorFile, "ERROR @ line %d: %s is library function\n", yylineno, $2);
				fail_icode = 1;
			}
			else
			{
				fprintf(errorFile, "ERROR @ line %d: %s already declared\n", yylineno, $2);
				fail_icode = 1;
			}
		}
		else
			$$ = insert_SymTable(table, new_SymTabEntry($2, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope, currfunc, USERFUNC));

		$$->value.funcVal->iaddress = currQuad + 1;
		$$->value.funcVal->jump_label = jump_label;
		emit(funcstart, NULL, NULL, newexpr(programfunc_e, $$), yylineno);

		/* offset code for function */
		push(currscopeoffset());
		enterscopespace();
		resetformalargsoffset();
	}
	;

funcname : 
	ID { $$ = $ID; }
	|
	{
		char *anonym = (char *)malloc(sizeof(char) * 2);
		sprintf(anonym, "$%d", anonym_func_count++);
		$$ = anonym;
	}
	;

funcargs : 
	LEFT_PARENTHESIS idlist
	{
		/* exiting formal args offset */
		tmp_args = currscopeoffset();
	}
	RIGHT_PARENTHESIS
	{
		return_flag++;

		/* offset code for function locals */
		enterscopespace();
		resetfunctionlocaloffset();
		$$ = tmp_args;
	}
	;

funcbody : {push_lpstack(&lcs, loopcounter); loopcounter = 0;}
	funcblockstart block funcblockend
    { 
		loopcounter = pop_lpstack(&lcs);
		/* offset code when exiting function local space*/
		$$ = currscopeoffset();
		exitscopespace();
    }
	;

funcblockstart: {};
funcblockend: {};

const : 
	REALCONST { $$ = newconstnumexpr((double)$1); }
	| INTCONST { $$ = newconstnumexpr((double)$1); }
	| STRING { $$ = newconststringexpr($1); }
	| NIL { $$ = newconstnumexpr((double)$1); }
	| TRUE { $$ = newconstboolexpr(VAR_TRUE); }
	| FALSE { $$ = newconstboolexpr(VAR_FALSE); }
	;

idlist :
	| ID
	{
		SymTabEntry *tmp = lookup_SymTableScope(table, currscope + 1, $1);
		if (tmp == NULL && (tmp = lookup_SymTableScope(table, 0, $1)) != NULL)
		{
			if (tmp != NULL && !strcmp(SymbolTypeToString(tmp->type), "LIBFUNC"))
			{
				fprintf(errorFile, "ERROR @ line %d: Cannot have a library function as argument\n", yylineno);
				fail_icode = 1;
			}
			else
			{
				tmp = insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope + 1, currfunc, FORMAL));
				tmp->space = currscopespace();
				tmp->offset = currscopeoffset();
				inccurrscopeoffset();
				printf("var : %s | scope : %d | space : %d | offset : %d\n", tmp->name, tmp->scope, tmp->space, tmp->offset);
			}
		}
		else if (tmp != NULL && !strcmp(SymbolTypeToString(tmp->type), "FORMAL") && tmp->isActive == 1)
		{
			fprintf(errorFile, "ERROR @ line %d: Cannot have the same argument names\n", yylineno);
			fail_icode = 1;
		}
		else
		{
			tmp = insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope + 1, currfunc, FORMAL));
			tmp->space = currscopespace();
			tmp->offset = currscopeoffset();
			inccurrscopeoffset();
			printf("var : %s | scope : %d | space : %d | offset : %d\n", tmp->name, tmp->scope, tmp->space, tmp->offset);
		}
	}
	COMA idlist | ID
	{
		SymTabEntry *tmp = lookup_SymTableScope(table, currscope + 1, $1);
		if (tmp == NULL && (tmp = lookup_SymTableScope(table, 0, $1)) != NULL)
		{
			if (tmp != NULL && !strcmp(SymbolTypeToString(tmp->type), "LIBFUNC"))
			{
				fprintf(errorFile, "ERROR @ line %d: Cannot have a library function as argument\n", yylineno);
				fail_icode = 1;
			}
			else
			{
				tmp = insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope + 1, currfunc, FORMAL));
				tmp->space = currscopespace();
				tmp->offset = currscopeoffset();
				inccurrscopeoffset();
				printf("var : %s | scope : %d | space : %d | offset : %d\n", tmp->name, tmp->scope, tmp->space, tmp->offset);
			}
		}
		else if (tmp != NULL && !strcmp(SymbolTypeToString(tmp->type), "FORMAL") && tmp->isActive == 1)
		{
			fprintf(errorFile, "ERROR @ line %d: Cannot have the same argument names\n", yylineno);
			fail_icode = 1;
		}
		else
		{
			tmp = insert_SymTable(table, new_SymTabEntry($1, yylineno, 1, new_Variable(NULL), new_Function(NULL), currscope + 1, currfunc, FORMAL));
			tmp->space = currscopespace();
			tmp->offset = currscopeoffset();
			inccurrscopeoffset();
			printf("var : %s | scope : %d | space : %d | offset : %d\n", tmp->name, tmp->scope, tmp->space, tmp->offset);
		}
	}
	;

ifstmt : 
	ifexpr statement { edit_quad((int)$1, NULL, NULL, NULL, currQuad + 1); }
	elseexpr
	{
		if (else_flag == -1)
		{
			edit_quad((int)$1, NULL, NULL, NULL, jump_label + 2);
			edit_quad((int)jump_label, NULL, NULL, NULL, currQuad + 1);
			else_flag = 0;
		}
		$$.breakList = mergelist($elseexpr.breakList, $statement.breakList);
		$$.contList = mergelist($elseexpr.contList, $statement.contList);
	}
	;

ifexpr : 
	IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
	{
		expr *temp = NULL;
		if ($expr->type == boolexpr_e)
		{
			temp = newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
			emit(assign, newconstboolexpr(VAR_TRUE), NULL, temp, yylineno);
			emit_jump(jump, NULL, NULL, currQuad + 3, yylineno);
			emit(assign, newconstboolexpr(VAR_FALSE), NULL, temp, yylineno);
			backpatch($expr->truelist, currQuad - 2);
			backpatch($expr->falselist, currQuad);
		}
		emit_jump(if_eq, temp, newconstboolexpr(VAR_TRUE), currQuad + 3, yylineno);
		$$ = currQuad;
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	;
elseexpr : 
	ELSE
	{
		jump_label = currQuad;
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	statement
	{
		$$ = $statement;
		else_flag = -1;
	}
	| { make_stmt(&$$); }
	;

whilestmt : 
	whilestart whilecond loopstmt
	{
		loop_flag = 0;
		emit_jump(jump, NULL, NULL, $whilestart, yylineno);
		edit_quad($whilecond, NULL, NULL, NULL, currQuad + 1);
		patchlist($loopstmt.breakList, currQuad + 1);
		patchlist($loopstmt.contList, $whilestart);
		$$ = $loopstmt;
		printf("\n\nWHIILE : %d %d\n\n", $loopstmt.contList, $whilestart);
	};

whilestart : WHILE { $$ = currQuad+1; };

whilecond : 
	LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
	{
		loop_flag = 1;
		expr *temp;
		if ($expr->type == boolexpr_e)
		{
			temp = newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
			emit(assign, newconstboolexpr(VAR_TRUE), NULL, temp, yylineno);
			emit_jump(jump, NULL, NULL, currQuad + 3, yylineno);
			emit(assign, newconstboolexpr(VAR_FALSE), NULL, temp, yylineno);
			backpatch($expr->truelist, currQuad - 2);
			backpatch($expr->falselist, currQuad);
		}
		emit_jump(if_eq, temp, newconstboolexpr(1), currQuad + 3, yylineno);
		$$ = currQuad;
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	;

forstmt : forprefix N elist RIGHT_PARENTHESIS N { loop_flag = 1; }
	loopstmt N
	{
		loop_flag = 0;
		edit_quad((int)$forprefix, NULL, NULL, NULL, $5 + 2);
		edit_quad((int)$2, NULL, NULL, NULL, currQuad + 1);
		edit_quad((int)$5, NULL, NULL, NULL, jump_label);
		edit_quad((int)$8, NULL, NULL, NULL, $2 + 2);
		patchlist($loopstmt.breakList, currQuad + 1);
		patchlist($loopstmt.contList, $2 + 2);
	}
	;

forprefix : FOR LEFT_PARENTHESIS elist SEMICOLON M_ expr SEMICOLON
	{
		jump_label = $M_;
		expr *temp;
		if ($expr->type == boolexpr_e)
		{
			temp = newexpr(var_e, (SymTabEntry *)newtemp(table, currscope, currfunc));
			emit(assign, newconstboolexpr(VAR_TRUE), NULL, temp, yylineno);
			emit_jump(jump, NULL, NULL, currQuad + 3, yylineno);
			emit(assign, newconstboolexpr(VAR_FALSE), NULL, temp, yylineno);
			backpatch($expr->truelist, currQuad - 2);
			backpatch($expr->falselist, currQuad);
		}
		$$ = currQuad;
		emit_jump(if_eq, temp, newconstboolexpr(1), 0, yylineno);
	}
	;

loopstart: { loopcounter++; };
loopend: { loopcounter--; };
loopstmt : loopstart statement loopend { $$ = $statement; };

M_: { $$ = currQuad + 1; };

N:
	{
		$$ = currQuad;
		emit_jump(jump, NULL, NULL, 0, yylineno);
	}
	;

returnstmt : RETURN expr SEMICOLON
	{
		if (return_flag != 0)
		{
			emit(ret, NULL, NULL, $expr, yylineno);
		}
	}
	| RETURN SEMICOLON
	{
		if (return_flag != 0)
		{
			emit(ret, NULL, NULL, NULL, yylineno);
		}
	}
	;

%%

int yyerror(char *yaccProvidedMessage)
{
	fprintf(stderr, "%s: at line %d, before token: '%s'\n", yaccProvidedMessage, yylineno, yytext);
}

void printusage()
{
	fprintf(stderr, "Invalid arguments provided.\nUsage: [program_name] <input_file_name> <output_file_name> <args> <error_log>\n");
	fprintf(stderr, "Ommiting input file or output file will send I/O to stdin and stdout.\n");
	fprintf(stderr, "<args> can be -s to print symtable sorted by scopes, -t to print symtable as a hash table, or nothing to print both.\n");
	fprintf(stderr, "<error_log> can be -log_err to export a txt with error logs , or nothing for errors to be printed to stderr.\n");
}

int main(int argc, char **argv)
{
	table = init_SymTable();
	char *args = NULL;
	char *error_output = NULL;
	char c;

	if (argc == 5)
	{
		if (!(yyin = fopen(argv[1], "r")))
		{
			fprintf(stderr, "Cannot Open Input File: %s\nAccepting input from stdin...\n", argv[1]);
			yyin = stdin;
		}
		if (!(yyout = fopen(argv[2], "w")))
		{
			fprintf(stderr, "Cannot Open Output File: %s\nOutput in stdout...\n", argv[2]);
			yyout = stdout;
		}
		args = argv[3];
		error_output = argv[4];
	}
	else if (argc == 4)
	{
		if (!(yyin = fopen(argv[1], "r")))
		{
			fprintf(stderr, "Cannot Open Input File: %s\nAccepting input from stdin...\n", argv[1]);
			yyin = stdin;
		}

		/* <args> && <error_log>*/
		if ((strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "-t") == 0) && strcmp(argv[3], "-log_err") == 0)
		{
			args = argv[2];
			error_output = argv[3];
		}
		/* output file && ( <args> || <error_log>) */
		else
		{
			/* output file && <args> */
			if (strcmp(argv[3], "-s") == 0 || strcmp(argv[3], "-t") == 0)
			{
				args = argv[3];
			}
			/* output file && <error_log> */
			else if (strcmp(argv[3], "-log_err") == 0)
			{
				error_output = argv[3];
			}
			/* error invalid argument */
			else
			{
				printusage();
				return -1;
			}

			if (!(yyout = fopen(argv[2], "w")))
			{
				fprintf(stderr, "Cannot Open Output File: %s\nOutput in stdout...\n", argv[2]);
				yyout = stdout;
			}
		}
	}
	else if (argc == 3)
	{

		if (!(yyin = fopen(argv[1], "r")))
		{
			fprintf(stderr, "Cannot Open Input File: %s\nAccepting input from stdin...\n", argv[1]);
			yyin = stdin;
		}

		/* <args> */
		if (strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "-t") == 0)
		{
			args = argv[2];
		}
		/* <error_log>*/
		else if (strcmp(argv[2], "-log_err") == 0)
		{
			error_output = argv[2];
		}
		/* output */
		else
		{
			if (!(yyout = fopen(argv[2], "w")))
			{
				fprintf(stderr, "Cannot Open Output File: %s\nOutput in stdout...\n", argv[2]);
				yyout = stdout;
			}
		}
	}
	else if (argc == 2)
	{
		if (!(yyin = fopen(argv[1], "r")))
		{
			fprintf(stderr, "Cannot Open Input File: %s\nAccepting input from stdin...\n", argv[1]);
			yyin = stdin;
		}
	}
	else
	{
		printusage();
		return -1;
	}

	if (error_output != NULL && strcmp(error_output, "-log_err") == 0)
		errorFile = fopen("error_log.txt", "w");
	else if (error_output == NULL)
		errorFile = stderr;
	else
	{
		printusage();
		return -1;
	}

	/* add library functions */
	insert_SymTable(table, new_SymTabEntry("print", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("input", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("objectmemberkeys", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("objecttotalmembers", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("objectcopy", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("totalarguments", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("argument", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("typeof", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("strtonum", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("sqrt", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("cos", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	insert_SymTable(table, new_SymTabEntry("sin", 0, 1, new_Variable(NULL), new_Function(NULL), 0, 0, LIBFUNC));
	yyparse();

	if (args != NULL && strcmp(args, "-s") == 0)
	{
	}
	//print_Scopes(table);
	else if (args != NULL && strcmp(args, "-t") == 0)
	{
	}
	//print_SymTable(table);
	else if (args == NULL)
	{
		//print_SymTable(table);
		print_Scopes(table);
	}
	else
		printusage();

	if (!fail_icode)
		print_quads(stdout, yylineno);

	else
		printf("Errors present. I-Code Generation has failed.\n");

	return 0;
}
