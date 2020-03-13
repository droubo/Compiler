%{
  extern int yylineno;
  extern char * yyval;
  extern char * yytext;
  extern FILE * yyin;
  extern FILE * yyout;

%}

/* Yacc Definitions */

%union {int num; char* id; double real;}
%start program
%token IF
%token ElSE
%token WHILE
%token FOR
%token FUNCTION
%token RETURN
%token BREAK
%token CONTINUE

%token AND
%token OR
%token NOT

%token LOCAL
%token TRUE
%token FALSE
%token NIL

%token ASSING
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
%token UNDEFINED

/* priority */

%right ASSING

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


%%

program : stmts
        ;

stmts : stmt stmts
      |
      ;

stmt : expr SEMICOLON
     | ifstmt
     | whilestmt
     | forstmt
     | returnstmt
     | break SEMICOLON
     | continue SEMICOLON
     | block
     | funcdef
     | SEMICOLON
     ;

expr : assignexpr
     | expr op expr
     | term
     ;

op : PLUS
   | MINUS
   | MULT
   | DIV
   | MOD
   | GREATER
   | GREATER_EQUAL
   | LESS
   | LESS_EQUAL
   | EQUAL
   | NOT_EQUAL
   | AND
   | OR
   ;

term : LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
     | MINUS expr
     | NOT expr
     | PLUS_PLUS lvalue
     | lvalue PLUS_PLUS
     | MINUS_MINUS lvalue
     | lvalue MINUS_MINUS
     | primary
     ;

assignexpr : lvalue EQUAL expr

primary : lvalue
        | call
        | objectdef
        | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
        | const
        ;

lvalue : ID
       | LOCAL ID
       | DOUBLE_COLON ID
       | member
       ;

member : lvalue DOT ID
       | lvalue LEFT_BRACKET expr RIGHT_BRACKET
       | call DOT ID
       | call LEFT_BRACKET expr RIGHT_BRACKET
       ;

call : call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
     | lvalue callsuffix
     | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
     ;

callsuffix : normcall
           | methodcall
           ;

normcall : LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
         ;

methodcall : DOUBLE_DOT ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
           ;

elist : expr comaexpr
      |
      ;

comaexpr : COMA expr comaexpr
         |
         ;

objectdef : LEFT_BRACKET elist RIGHT_BRACKET
          | LEFT_BRACKET indexed RIGHT_BRACKET
          | LEFT_BRACKET RIGHT_BRACKET
          ;

indexed : indexedelem comaindexedelem
        |
        ;

comaindexedelem : COMA indexedelem comaindexedelem
                 |
                 ;

indexedelem : LEFT_BRACE expr COLON expr RIGHT_BRACE
            ;

block : LEFT_BRACE stmts RIGHT_BRACE
      | LEFT_BRACE RIGHT_BRACE
      ;

funcdef : FUNCTION ID LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block
        | FUNCTION LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block
        ;

const : REALCONST 
      | INTCONST
      | STRING
      | NIL
      | TRUE
      | FALSE
      ;
 
idlist : ID
        | ID comaid
        |
        ;

comaid : COMA ID comaid
       |
       ;

ifstmt : IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt ElSE stmt
       | IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
       ;

whilestmt : while LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
          ;

forstmt : FOR LEFT_PARENTHESIS elist SEMICOLON expr SEMICOLON elist RIGHT_PARENTHESIS stmt
        ;

returnstmt : RETURN expr SEMICOLON
           | RETURN SEMICOLON
           ;

%%
