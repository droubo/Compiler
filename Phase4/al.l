%{
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
 * al.l
 * Lexical analyzer file for use with flex.
 */ 

#include "parser.h"
#include "symtab/numPlaces.h"

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

void print_tokens(FILE* f,struct alpha_token_t *head){
	struct alpha_token_t *tmp = head;
	while(tmp != NULL){
		if(strcmp(tmp->general_type, "STRING") != 0) fprintf(f,"%d:  #%d  \"%s\" %s %s  <- %s\n", tmp->numline, tmp->numToken, tmp->content, tmp->general_type, tmp->type, tmp->token_type);
		else fprintf(f,"%d:  #%d  \"%s\" %s \"%s\"  <- %s\n", tmp->numline, tmp->numToken, tmp->content, tmp->general_type, tmp->type, tmp->token_type);
		tmp = tmp->next;
	}
}

char * appendChar(int * size, char * string, char c){
    (*size)++;
    string = realloc(string, *size);
    string[*size - 1] = c;
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
local "local"
TRUE "true"
FALSE "false"
NIL "nil"

/* operators */
ASSIGN "="
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
LESS_EQUAL "<="

/* const */
REALCONST ([0-9]+)"."([0-9]+)
INTCONST [0-9]+

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

/* other and undefined */
OTHER [\t' '\r\n]*
UNDEFINED .

%option noyywrap
%option yylineno
%%
{IF} {insert_token(yylineno,token++,"IF", "KEYWORD", "enumerated", "if"); return IF;}
{ELSE} {insert_token(yylineno,token++,"ELSE", "KEYWORD", "enumerated", "else"); return ELSE;}
{WHILE} {insert_token(yylineno,token++,"WHILE", "KEYWORD", "enumerated", "while"); return WHILE;}
{FOR} {insert_token(yylineno,token++,"FOR", "KEYWORD", "enumerated", "for"); return FOR;}
{FUNCTION} {insert_token(yylineno,token++,"FUNCTION", "KEYWORD", "enumerated", "function"); return FUNCTION;}
{RETURN} {insert_token(yylineno,token++,"RETURN", "KEYWORD", "enumerated", "return"); return RETURN;}
{BREAK} {insert_token(yylineno,token++,"BREAK", "KEYWORD", "enumerated", "break"); return BREAK;}
{CONTINUE} {insert_token(yylineno,token++,"CONTINUE", "KEYWORD", "enumerated", "continue"); return CONTINUE;}
{AND} {insert_token(yylineno,token++,"AND", "KEYWORD", "enumerated", "and"); return AND;}
{NOT} {insert_token(yylineno,token++,"NOT", "KEYWORD", "enumerated", "not"); return NOT;}
{OR} {insert_token(yylineno,token++,"OR", "KEYWORD", "enumerated", "or"); return OR;}
{local} {insert_token(yylineno,token++,"local", "KEYWORD", "enumerated", "local"); return local;}
{TRUE} {insert_token(yylineno,token++,"TRUE", "KEYWORD", "enumerated", "true"); return TRUE;}
{FALSE} {insert_token(yylineno,token++,"FALSE", "KEYWORD", "enumerated", "false"); return FALSE;}
{NIL} {insert_token(yylineno,token++,"NIL", "KEYWORD", "enumerated", "nil"); return NIL;}

{ASSIGN} {insert_token(yylineno,token++,"ASSIGN", "OPERATOR", "enumerated", "="); return ASSIGN;}
{PLUS} {insert_token(yylineno,token++,"PLUS", "OPERATOR", "enumerated", "+"); return PLUS;}
{MINUS} {insert_token(yylineno,token++,"MINUS", "OPERATOR", "enumerated", "-"); return MINUS;}
{MULT} {insert_token(yylineno,token++,"MULT", "OPERATOR", "enumerated", "*"); return MULT;}
{DIV} {insert_token(yylineno,token++,"DIV", "OPERATOR", "enumerated", "/"); return DIV;}
{MOD} {insert_token(yylineno,token++,"MOD", "OPERATOR", "enumerated", "%"); return MOD;}
{EQUAL} {insert_token(yylineno,token++,"EQUAL", "OPERATOR", "enumerated", "=="); return EQUAL;}
{NOT_EQUAL} {insert_token(yylineno,token++,"NOT_EQUAL", "OPERATOR", "enumerated", "!="); return NOT_EQUAL;}
{PLUS_PLUS} {insert_token(yylineno,token++,"PLUS_PLUS", "OPERATOR", "enumerated", "++"); return PLUS_PLUS;}
{MINUS_MINUS} {insert_token(yylineno,token++,"MINUS_MINUS", "OPERATOR", "enumerated", "--"); return MINUS_MINUS;}
{GREATER} {insert_token(yylineno,token++,"GREATER", "OPERATOR", "enumerated", ">"); return GREATER;}
{LESS} {insert_token(yylineno,token++,"LESS", "OPERATOR", "enumerated", "<"); return LESS;}
{GREATER_EQUAL} {insert_token(yylineno,token++,"GREATER_EQUAL", "OPERATOR", "enumerated", ">="); return GREATER_EQUAL;}
{LESS_EQUAL} {insert_token(yylineno,token++,"LESS_EQUAL", "OPERATOR", "enumerated", "<="); return LESS_EQUAL;}

{LEFT_BRACE} {insert_token(yylineno,token++,"LEFT_BRACE", "PUNCUATION", "enumerated", "{"); return LEFT_BRACE;}
{RIGHT_BRACE} {insert_token(yylineno,token++,"RIGHT_BRACE", "PUNCUATION", "enumerated", "}"); return RIGHT_BRACE;}
{LEFT_BRACKET} {insert_token(yylineno,token++,"LEFT_BRACKET", "PUNCUATION", "enumerated", "["); return LEFT_BRACKET;}
{RIGHT_BRACKET} {insert_token(yylineno,token++,"RIGHT_BRACKET", "PUNCUATION", "enumerated", "]"); return RIGHT_BRACKET;}
{LEFT_PARENTHESIS} {insert_token(yylineno,token++,"LEFT_PARENTHESIS", "PUNCUATION", "enumerated", "("); return LEFT_PARENTHESIS;}
{RIGHT_PARENTHESIS} {insert_token(yylineno,token++,"RIGHT_PARENTHESIS", "PUNCUATION", "enumerated", ")"); return RIGHT_PARENTHESIS;}
{COMA} {insert_token(yylineno,token++,"COMA", "PUNCUATION", "enumerated", ","); return COMA;}
{SEMICOLON} {insert_token(yylineno,token++,"SEMICOLON", "PUNCUATION", "enumerated", ";"); return SEMICOLON;}
{COLON} {insert_token(yylineno,token++,"COLON", "PUNCUATION", "enumerated", ":"); return COLON;}
{DOUBLE_COLON} {insert_token(yylineno,token++,"DOUBLE_COLON", "PUNCUATION", "enumerated", "::"); return DOUBLE_COLON;}
{DOT} {insert_token(yylineno,token++,"DOT", "PUNCUATION", "enumerated", "."); return DOT;}
{DOUBLE_DOT} {insert_token(yylineno,token++,"DOUBLE_DOT", "PUNCUATION", "enumerated", ".."); return DOUBLE_DOT;}

{REALCONST} {insert_token(yylineno,token++, yytext, "CONST_REAL", "real", yytext); yylval.real = atof(yytext); return REALCONST;}
{INTCONST} {insert_token(yylineno,token++, yytext, "CONST_INT", "integer", yytext); yylval.integer = atoi(yytext); return INTCONST;}
{ID} {insert_token(yylineno,token++, yytext, "ID", "char*", yytext); yylval.id = strdup(yytext);  return ID;}

{STRING} {
    char curr;
    char * string;
    int size = 0;
    int flag = 0;
    int illegal = 0;
    int state = 0;
    int startline = yylineno;
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
        } else if(state == 1){
            fprintf(stderr, "Invalid escape character in string at line %d\n", startline);
            appendChar(&size, string, curr);
            state = 0;
        }
        curr = input();
    }
    appendChar(&size, string, '\0');

    if(!flag){
        fprintf(stderr, "Missing closing quote at the end of string at line %d\n", startline);
        return 0;
    }
    
    insert_token(startline, token++, string, "STRING", "char *", string);
	yylval.id = strdup(string);
    return STRING;
}

{LINE_COMMENT} {insert_token(yylineno,token++,"LINE_COMMENT","COMMENT","enumerated","");}

{MULTILINE_COMMENT} {
    /* CORRECT COMMENTS */
    char curr;
    int size = 0;
    int counter = 1;
    int state = 0;
    char * string;
    int startline = yylineno;
    int endline;

    curr = input();
    while(curr != EOF){
        if(curr == '/' && state == 2){
            counter--;
            if(counter == 0)
                break;
            else
                state = 0;
        } else if(curr == '*' && state == 1){
            state = 0;
            counter++;
        } else if(curr == '*' && state == 0){
            state = 2;
        } else if(curr == '/' && state == 0) {
            state = 1;
        } else if(state == 1 || state == 2){
            state = 0;
        }
        curr = input();
    }
    
    endline = yylineno;

    if(counter != 0){
        fprintf(stderr, "Malformed multiline comment at line %d\n", startline);
        return 0;
    }

    string = malloc(sizeof(char)*(numPlaces(startline) + numPlaces(endline) + 4));
    sprintf(string, "%d - %d", startline, endline);

    insert_token(startline, token++, "MULTILINE_COMMENT", "COMMENT", "enumerated", string);
}

{OTHER} {}
{UNDEFINED} {fprintf(stderr, "Undefined input\n");}

%%

/*
int main(int argc , char** args) {
    FILE *fin = stdin, *fout = stdout;
    if(argc == 2 || argc == 3) {
        fin = fopen(args[1], "r");
        if(fin == NULL)
        { 
            fprintf(stderr, "Error, %s not found\n",args[1]);
            exit(-1);
        }
        if(argc == 3) fout = fopen(args[2], "w");
        yyin = fin;
        
    } else {
        fprintf(stderr, "Invalid arguments. Usage:\nlexical <input filename> <output filename>\nlexical <input filename>");
        return -1;
    }
	alpha_yylex(NULL);
    print_tokens(fout,head);
    fclose(fout);
    fclose(fin);

    return 0;
}*/
