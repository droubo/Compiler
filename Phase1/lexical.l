%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define YY_DECL int yylex (void* yylval)

%}

/* key words */
IF "if"
ELSE "else"
WHILE "while"
FOR "for"
FUNCTION "function"
RETURN "return"
BREAK "break"
CONTINUE "continue"
AND "and"
NOT "not"
OR "or"
LOCAL "local"
TRUE "true"
FALSE "false"
NIL "nil"

/* operators */
ASSINGN "="
PLUS "+"
MINUS "-"
MULT "*"
DIV "/"
MOD "%"
EQUAL "=="
NOT_EQUAL "!="
PLUS_PLUS "++"
MINUS_MINUS "--"
GREATER ">"
LESS "<"
GREATER_EQUAL ">="
SMALL_EQUAL "<="

/* const */
INTCONST [0-9]+
REALCONST [0-9]"."[0-9]+

/* string */
STRING \"

/*  punctuation */
LEFT_CURLY_BRACKET \{
RIGHT_CURLY_BRACKET \}
LEFT_BRACKET \[
RIGHT_BRACKET \]
LEFT_PARENTHESIS \(
RIGHT_PARENTHESIS \)
SEMICOLON \;
COMA \,
COLON \:
DOUBLE_COLON \:\:
DOT \.
DOUBLE_DOT \.\.

/* identifier */
ID [a-zA-Z][a-zA-Z0-9_]*

/* comments */
LINE_COMMENT "//".*



%%
