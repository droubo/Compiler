%{
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #define YY_DECL int alpha_yylex (void* yylval)
  int token=0;
  int line=0;

/* output struct */
struct alpha_token_t{
         int lineno;
		 int tokenno;
		 char* type;
		 char* value;
         struct alpha_token_t *next;
};
struct alpha_token_t *head=NULL;

/* print of output*/
void PrintTokens(struct alpha_token_t *head)
{
     struct alpha_token_t *tmp=head;
     while (tmp != NULL)
     {
      
	    if(!strcmp(tmp->value," ")){
	      printf("%3d: #%3d \t\t\t\t %s\n",tmp->lineno,tmp->tokenno,tmp->type);
	    }
	    else{
		printf("%3d: #%3d [\"%s\"] \t\t %s\n",tmp->lineno,tmp->tokenno,tmp->value,tmp->type);
	    }
	   tmp = tmp->next;
     }
     printf("\n");
}
    

/* insert of token in struct */
struct alpha_token_t *AddToken(struct alpha_token_t *head, int line, int tokenno,char* type,char* value)
{
     struct alpha_token_t *tmp=head, *newnode;
	 newnode = (struct alpha_token_t *) malloc ( sizeof(struct alpha_token_t) );
	 newnode->lineno=line;
	 newnode->tokenno=tokenno;
	 newnode->type=strdup(type);
	 newnode->value=strdup(value);
	 newnode->next=NULL;
	 if(tmp==NULL){
		return newnode;
	 }
	 while (tmp->next != NULL)
     {
           tmp = tmp->next;
     }
	 tmp->next=newnode;
	 return head;
}



%}

/*Dhlwsh twn token*/

/*Desmeumenes lexeis*/
IF 		"if"
ELSE		"else"
WHILE 		"while"
FOR		"for"
FUNCTION	"function"
RETURN 		"return"
BREAK		"break"
CONTINUE	"continue"
AND		"and"
NOT		"not"
OR		"or"
LOCAL		"local"
TRUE		"true"	
FALSE		"false"
NIL		"nil"


/*Telestes*/

ASSIGN		"="
PLUS		"+"
MINUS		"-"
MULTI		"*"
DIV		"/"
MOD		"%"
EQUAL		"=="
NEQUAL		"!="
PPLUS		"++"
MMINUS		"--"
GREATER		">"
LESS		"<"
GREATER_EQUAL	">="
LESS_EQUAL	"<="

/*Ari8mhtikes sta8eres */

NUMBER		[0-9]+
REALNUM		[0-9]+"."[0-9]+

/*String*/
STRING		\"

/*Shmeia Sti3hs*/
LEFT_CBRACKET	\{
RIGHT_CBRACKET	\}
LEFT_SBRACKET	\[
RIGHT_SBRACKET	\]
LEFT_PARENTHES	\(
RIGHT_PARENTHES	\)
SEMICOLON	\;
COMMA		\,
COLON		\:
DCOLON		\:\:
DOT		\.
DOTS		\.\.

/*Identifier*/
ID		[a-zA-Z][a-zA-Z_0-9]*

/*Comments*/

COMMENT		"//".*
COMMENT2	"/*"
END_COMMENT2	"*/"

/*Others*/

WHITE_SPACES   [\t' '\r]*
NEW_LINE	"\n"
OTHER		.


%option noyywrap
%option yylineno
%%

{IF} 			{head=AddToken(head, yylineno,++token,"KEYWORD","if");}
				
{ELSE}			{head=AddToken(head, yylineno,++token,"KEYWORD","else");}

{WHILE}			{head=AddToken(head, yylineno,++token,"KEYWORD","while");}

{FOR}			{head=AddToken(head, yylineno,++token,"KEYWORD","for");}

{FUNCTION}		{head=AddToken(head, yylineno,++token,"KEYWORD","function");}

{RETURN}		{head=AddToken(head, yylineno,++token,"KEYWORD","return");}

{BREAK}			{head=AddToken(head, yylineno,++token,"KEYWORD","break");}

{CONTINUE}		{head=AddToken(head, yylineno,++token,"KEYWORD","continue");}

{AND}			{head=AddToken(head, yylineno,++token,"KEYWORD","and");}

{NOT}			{head=AddToken(head, yylineno,++token,"KEYWORD","not");}

{OR}			{head=AddToken(head, yylineno,++token,"KEYWORD","or");}

{LOCAL}			{head=AddToken(head, yylineno,++token,"KEYWORD","local");}

{TRUE}			{head=AddToken(head, yylineno,++token,"KEYWORD","true");}

{FALSE}			{head=AddToken(head, yylineno,++token,"KEYWORD","false");}

{NIL}			{head=AddToken(head, yylineno,++token,"KEYWORD","nil");}

{ASSIGN}		{head=AddToken(head, yylineno,++token,"OPERANT","=");}

{PLUS}			{head=AddToken(head, yylineno,++token,"OPERANT","+");}

{MINUS}			{head=AddToken(head, yylineno,++token,"OPERANT","-");}

{MULTI}			{head=AddToken(head, yylineno,++token,"OPERANT","*");}

{DIV}			{head=AddToken(head, yylineno,++token,"OPERANT","/");}

{MOD}			{head=AddToken(head, yylineno,++token,"OPERANT","%");}

{EQUAL}			{head=AddToken(head, yylineno,++token,"OPERANT","==");}

{NEQUAL}		{head=AddToken(head, yylineno,++token,"OPERANT","!=");}

{PPLUS}			{head=AddToken(head, yylineno,++token,"OPERANT","++");}

{MMINUS}		{head=AddToken(head, yylineno,++token,"OPERANT","--");}

{GREATER}		{head=AddToken(head, yylineno,++token,"OPERANT",">");}

{LESS}			{head=AddToken(head, yylineno,++token,"OPERANT","<");}

{GREATER_EQUAL}		{head=AddToken(head, yylineno,++token,"OPERANT",">=");}

{LESS_EQUAL}		{head=AddToken(head, yylineno,++token,"OPERANT","<=");}


{NUMBER} 		{head=AddToken(head, yylineno,++token,"INTCONST",yytext);}

{REALNUM}		{head=AddToken(head, yylineno,++token,"FLOATCONST",yytext);}

{STRING} {    
	  char c;
	  int i = 0;
	  int error_flag = 1;
	  char *str=(char *)malloc(sizeof(char));
	  *(str + i) = '\0';
          while((c=input())!=EOF)
	  {			

	  	while (c == '\\'){/*auth h while voleuei gia ta sunexomena slash
	      			    vlepei pote uparxei slash kai analoga vazei sto string
					ton katallhlo escape character h' petaei warning */

			c = input();

	      		if ( c == 'n'){
				
                        	*(str+i) = '\n';
			}
			else if ( c == 't' ){
				
				c = '\t';
                	        *(str+i) = c; // kai ena oti na nai gramma na valeis edw den mpainei sto string
			}
                        else if ( c == '\\' ){//auto k to apo katw mporei na 8eloun allagh :/
                                
				*(str+i)='\\';
                        }
                        else if ( c == '\"' ){
                                
                                *(str+i)='"';
                        }
			else{
				/*otan vrei kati pou den to anagnwrizei dinei keno xarakthra sto string*/
	                        *(str+i)=' ';
				printf("WARNING, Unknown character found! a space character was added.\n");
			}
			 str=(char *)realloc(str, ((++i)+1)*sizeof(char));/*realloc gia ton epomeno xarakthra*/
			*(str+i)= '\0';
			c = input();
			printf("\nhere should be next of escape character... %c\n",c);
	      	}

		if (c == '\"'){			/*an den dei autakia xwris na dei slash to string eftase sto telos tou*/
			error_flag = 0;				/*mhdemismos ths error flag gia na mhn petaksei la8os*/
			head=AddToken(head, yylineno,++token,"STRING", str);	/*eisagwgh sth lista*/
			break;
		}
		else 	/*alliws sunexizri na vazei xarakthres*/
		{		  
			*(str+i)=c;	
			str=(char *)realloc(str, ((++i)+1)*sizeof(char));
			*(str + i ) = '\0';
		}
          }

	  if(error_flag){	/*an h error flag den mhdenistei pote tote exoume ftasei sto EOF
				  xwris na kleisei to string opote exoume error			*/
	  	head=AddToken(head,yylineno,++token,"STRING","Wrong Form");
		error_flag = 1;		//for next string probably not needed
	  }
}

{LEFT_CBRACKET}		{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK","{");}

{RIGHT_CBRACKET}	{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK","}");}

{LEFT_SBRACKET}		{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK","[");}

{RIGHT_SBRACKET}	{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK","]");}

{LEFT_PARENTHES}	{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK","(");}

{RIGHT_PARENTHES}	{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK",")");}

{SEMICOLON}             {head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK",";");}

{COMMA}			{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK",",");}

{COLON}			{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK",":");}

{DCOLON}		{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK","::");}

{DOT}			{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK",".");}

{DOTS}			{head=AddToken(head, yylineno,++token,"PUNCTUATION_MARK","..");}

{ID}			{head=AddToken(head, yylineno,++token,"IDENTIFIER",yytext);}

{COMMENT}		{head=AddToken(head, yylineno,++token,"COMMENT","//");}

{COMMENT2}		{
					char token;
					int commentno=1;/*counter twn comment*/
					while((token=input())!=EOF){
						if(commentno==0){/*otan o counter ginei mhden
								exoume iso ari8mo sxoliwn pou 
								anoigoun me osa kleinoun */
							break;
						}
						if(token=='/'){
							token=input();
							if(token=='*'){/*au3anoume ton counter*/
								commentno++;
							}
							else{/*ftiaxnoume to teleutaio char*/
								unput(token);
							}
						}
						else if(token=='*'){
							token=input();
							if(token=='/'){/*meiwnoume ton counter*/
								commentno--;
							}
							else{/*ftiaxnoume to teleutaio char*/
								unput(token);
							}
						}						
					}
					if(commentno!=0){/*an sto EOF o counter den einai mhden exoume error*/
						head=AddToken(head,yylineno,++token,"COMMENTS","Fail Form");
					}
					else{
						head=AddToken(head,yylineno,++token,"COMMENTS","Right Form");
					}
				}

{WHITE_SPACES}		{head=AddToken(head, yylineno,++token,"WHITE_SPACES"," ");}

{NEW_LINE}		{line++;}

{OTHER}			{head=AddToken(head, yylineno,++token,"OTHER","Non-Supported Type");}


  
%%
int main(int argc, char *argv[])
{
  	FILE *fp;
	if(argc==1){/*an den dw8ei txt ws orisma
  			pairnei eisodo apo konsola */
    		printf("Give an input from keyboard .. \n");
	    	yyin=stdin;
   		// yyout=stdout;
  	}
  	else if (argc!=2){/*an dw8hke kati diaforetiko apo duo
			   orismata tote exoume error */
   		printf("Wrong parameters! \n");
    		exit(1);
  	}
  	else{	/*alliws an vre8ei to arxeio eisodou pairnei eisodo apo auto*/
    		if(!(fp=fopen(argv[1],"r")))
    		{
      			printf("Cannot open source file !\n");
      			exit(1);
    		}
    	yyin=fp;
	}

	alpha_yylex(NULL);
	PrintTokens(head);/*print to output tou le3ikografikou analuth*/

}
