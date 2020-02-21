%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define YY_DECL int yylex (void* yylval)

char * appendChar(int * size, char * string, char c){
    (*size)++;
    string = realloc(string, *size);
    string[*size - 1] = c;
}

char * parseString(){
    char curr;
    char * string;
    int size;
    int flag = 0;
    int state = 0;

    string = (char *) malloc(sizeof(char));
    size = 0;

    curr = input();
    while(curr != EOF){
        if(curr == '\"' && state == 0){
            flag = 1;
            break;
        } else if(curr != '\\' && state == 0){
            state = 0;
            appendChar(&size, string, curr);
        } else if(curr == '\\' && state == 0){
            state = 1;
        } else if(curr == '\\' && state == 1){
            state = 0;
            appendChar(&size, string, '\\');
        } else if(curr == 'n' && state == 1){
            state = 0;
            appendChar(&size, string, '\n');
        } else if(curr == 't' && state == 1){
            state = 0;
            appendChar(&size, string, '\t');
        } else if(curr == '\"' && state == 1){
            state = 0;
            appendChar(&size, string, '\"');
        }

        curr = input();
    }
    appendChar(&size, string, '\0');

    if(flag)
        return string;
    else
        return "Missing closing quote";
}

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
{STRING} {head = AddToken(head, yylineno, ++token)}