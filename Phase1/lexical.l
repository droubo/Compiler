%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define INT_MIN -2147483647
#define INT_MAX 2147483647
#define YY_DECL int alpha_yylex (void* yylval)

struct alpha_token_t {
	unsigned int     numline;
	unsigned int     numToken;
	char          *content;
	char          *type;
	char		  *general_type;
	char		  *token_type;
	struct alpha_token_t *next;

};

struct alpha_token_t *head=NULL, *tail=NULL;
int token = 1;

struct alpha_token_t insert_token(int line, int tokenno,char* type,char *general_type, char *token_type, char* value){
	struct alpha_token_t *new_token, *tmp=head;
	new_token = (struct alpha_token_t *) malloc(sizeof(struct alpha_token_t));
	new_token->numline = line;
	new_token->numToken = tokenno;
	new_token->content = strdup(value);
	new_token->type = strdup(type);
	new_token->general_type = strdup(general_type);
	new_token->token_type = strdup(token_type);
	new_token->next = NULL;
	if(head == NULL){	
		head = new_token;
		tail = new_token;
	}
	else{
		tail->next = new_token;
		tail = new_token;
	}
}

void print_tokens(struct alpha_token_t *head){
	struct alpha_token_t *tmp = head;
	while(tmp != NULL){
		printf("%d:  #%d  \"%s\" %s %s  <- %s\n", tmp->numline, tmp->numToken, tmp->content, tmp->general_type, tmp->type, tmp->token_type);
		tmp = tmp->next;
	}
}

char * appendChar(int * size, char * string, char c){
    (*size)++;
    string = realloc(string, *size);
    string[*size - 1] = c;
}

int numPlaces (int n) {
    if (n < 0) return numPlaces ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
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
NULL "null"

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
LEFT_BRACE \{
RIGHT_BRACE \}
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
MULTILINE_COMMENT "/*"

%option noyywrap
%option yylineno
%%
{IF} {insert_token(yylineno,token++,"IF", "KEYWORD", "enumerated", "if");}
{ELSE} {insert_token(yylineno,token++,"ELSE", "KEYWORD", "enumerated", "else");}
{WHILE} {insert_token(yylineno,token++,"WHILE", "KEYWORD", "enumerated", "while");}
{FOR} {insert_token(yylineno,token++,"FOR", "KEYWORD", "enumerated", "for");}
{FUNCTION} {insert_token(yylineno,token++,"FUNCTION", "KEYWORD", "enumerated", "function");}
{RETURN} {insert_token(yylineno,token++,"RETURN", "KEYWORD", "enumerated", "return");}
{BREAK} {insert_token(yylineno,token++,"BREAK", "KEYWORD", "enumerated", "break");}
{CONTINUE} {insert_token(yylineno,token++,"CONTINUE", "KEYWORD", "enumerated", "continue");}
{AND} {insert_token(yylineno,token++,"AND", "KEYWORD", "enumerated", "and");}
{NOT} {insert_token(yylineno,token++,"NOT", "KEYWORD", "enumerated", "not");}
{OR} {insert_token(yylineno,token++,"OR", "KEYWORD", "enumerated", "or");}
{LOCAL} {insert_token(yylineno,token++,"LOCAL", "KEYWORD", "enumerated", "local");}
{TRUE} {insert_token(yylineno,token++,"TRUE", "KEYWORD", "enumerated", "true");}
{FALSE} {insert_token(yylineno,token++,"FALSE", "KEYWORD", "enumerated", "false");}
{NULL} {insert_token(yylineno,token++,"NULL", "KEYWORD", "enumerated", "null");}

{ASSINGN} {insert_token(yylineno,token++,"ASSINGN", "OPERATOR", "enumerated", "=");}
{PLUS} {insert_token(yylineno,token++,"PLUS", "OPERATOR", "enumerated", "+");}
{MINUS} {insert_token(yylineno,token++,"MINUS", "OPERATOR", "enumerated", "-");}
{MULT} {insert_token(yylineno,token++,"MULT", "OPERATOR", "enumerated", "*");}
{DIV} {insert_token(yylineno,token++,"DIV", "OPERATOR", "enumerated", "/");}
{MOD} {insert_token(yylineno,token++,"MOD", "OPERATOR", "enumerated", "%");}
{EQUAL} {insert_token(yylineno,token++,"EQUAL", "OPERATOR", "enumerated", "==");}
{NOT_EQUAL} {insert_token(yylineno,token++,"NOT_EQUAL", "OPERATOR", "enumerated", "!=");}
{PLUS_PLUS} {insert_token(yylineno,token++,"PLUS_PLUS", "OPERATOR", "enumerated", "++");}
{MINUS_MINUS} {insert_token(yylineno,token++,"MINUS_MINUS", "OPERATOR", "enumerated", "--");}
{GREATER} {insert_token(yylineno,token++,"GREATER", "OPERATOR", "enumerated", ">");}
{LESS} {insert_token(yylineno,token++,"LESS", "OPERATOR", "enumerated", "<");}
{GREATER_EQUAL} {insert_token(yylineno,token++,"GREATER_EQUAL", "OPERATOR", "enumerated", ">=");}
{SMALL_EQUAL} {insert_token(yylineno,token++,"SMALL_EQUAL", "OPERATOR", "enumerated", "<=");}

{LEFT_BRACE} {insert_token(yylineno,token++,"LEFT_BRACE", "PUNCUATION", "enumerated", "{");}
{RIGHT_BRACE} {insert_token(yylineno,token++,"RIGHT_BRACE", "PUNCUATION", "enumerated", "}");}
{LEFT_BRACKET} {insert_token(yylineno,token++,"LEFT_BRACKET", "PUNCUATION", "enumerated", "[");}
{RIGHT_BRACKET} {insert_token(yylineno,token++,"RIGHT_BRACKET", "PUNCUATION", "enumerated", "]");}
{LEFT_PARENTHESIS} {insert_token(yylineno,token++,"LEFT_PARENTHESIS", "PUNCUATION", "enumerated", "(");}
{RIGHT_PARENTHESIS} {insert_token(yylineno,token++,"RIGHT_PARENTHESIS", "PUNCUATION", "enumerated", ")");}
{COMA} {insert_token(yylineno,token++,"COMA", "PUNCUATION", "enumerated", ",");}
{SEMICOLON} {insert_token(yylineno,token++,"SEMICOLON", "PUNCUATION", "enumerated", ";");}
{COLON} {insert_token(yylineno,token++,"COLON", "PUNCUATION", "enumerated", ":");}
{DOUBLE_COLON} {insert_token(yylineno,token++,"DOUBLE_COLON", "PUNCUATION", "enumerated", "::");}
{DOT} {insert_token(yylineno,token++,"DOT", "PUNCUATION", "enumerated", ".");}
{DOUBLE_DOT} {insert_token(yylineno,token++,"DOUBLE_DOT", "PUNCUATION", "enumerated", "..");}

{INTCONST} {insert_token(yylineno,token++, yytext, "CONST_INT", "integer", yytext);}
{REALCONST} {insert_token(yylineno,token++, yytext, "CONST_REAL", "real", yytext);}
{ID} {insert_token(yylineno,token++, yytext, "ID", "char*", yytext);}

{STRING} {
    char curr;
    char * string;
    int size = 0;
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

    if(!flag)
        string = "Missing closing quote";
    
    insert_token(yylineno, token++, yytext, "STRING", "char *", string);
}

{LINE_COMMENT} {}

{MULTILINE_COMMENT} {
    char curr;
    int size = 0;
    int counter = 1;
    int state = 0;
    char * string;
    int startline = yylineno;
    int endline;

    curr = input();
    while(curr != EOF){
        if(curr == '/' && state == 3){
            counter--;
            if(counter == 0)
                break;
            else
                state = 0;
        } else if(curr == '/' && state == 0){
            state = 1;
        } else if(curr == '*' && state == 1){
            state = 0;
            counter++;
        } else if(curr == '*' && state == 0){
            state = 3;
        }
        curr = input();
    }

    endline = yylineno;

    string = malloc(sizeof(char)*(numPlaces(startline) + numPlaces(endline) + 4));
    sprintf(string, "%d - %d", startline, endline);

    if(counter != 0)
        string = "Malformed multiline comment";
    

    insert_token(startline, token++, yytext, "MULTILINE COMMENT", "enumerated", string);
}

%%


int main(int argc , char** args)
{
    if(argc == 1)
    {
       /* output --> stdout */
    }
    else if(argc == 2)
    {
         /* output --> args[1] */
         int _t;
       
    }
    alpha_yylex(NULL);
	print_tokens(head);

}