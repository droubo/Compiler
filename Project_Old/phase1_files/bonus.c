#include <stdio.h>
#include <string.h>
#include <stdlib.h>


  
enum state {State0=0,Operant,Punc,Number,String,Comment1,Comment2,Letter,IF,ELSE,WHILE,BREAK,
FOR,FUNCTION,RETURN,CONTINUE,AND,OR,NOT,LOCAL,TRUE,FALSE,NIL} states;

enum state state0(char c,FILE *fp);
enum state operant(char c,FILE *fp);
enum state punc(char c,FILE *fp);
enum state number(char c,FILE *fp);
enum state id(char c,FILE *fp);
enum state comment1(char c,FILE *fp);
enum state comment2(char c,FILE *fp);
enum state string(char c,FILE *fp);
enum state letter(char c,FILE *fp);
enum state func_if(char c,FILE *fp);
enum state func_else(char c,FILE *fp);
enum state func_while(char c,FILE *fp);
enum state func_break(char c,FILE *fp);
enum state func_for(char c,FILE *fp);
enum state func_function(char c,FILE *fp);
enum state func_return(char c,FILE *fp);
enum state func_continue(char c,FILE *fp);
enum state func_and(char c,FILE *fp);
enum state func_or(char c,FILE *fp);
enum state func_not(char c,FILE *fp);
enum state func_true(char c,FILE *fp);
enum state func_false(char c,FILE *fp);
enum state func_local(char c,FILE *fp);
enum state func_nil(char c,FILE *fp);
void empty_str(char* str);

char str[100]={'\0'},prev='\0';
int i=0;
int line=1,  token =0;
struct alpha_token_t{
         int lineno;
		 int tokenno;
		 char* type;
		 char* value;
         struct alpha_token_t *next;
}*head=NULL;


void PrintTokens(struct alpha_token_t *head);
struct alpha_token_t *AddToken(struct alpha_token_t *head, int line,int tokenno,char* type,char* value) ;
char *token_type( char *c);



int main(int argc, char *argv[])
{
     char c;
     FILE *fp;
    enum state (*action[])(char c,FILE *fp)={state0,operant,punc,number,string,comment1,comment2,
    letter,func_if,func_else,func_while,func_break,func_for,func_function,func_return,func_continue,
    func_and,func_or,func_not,func_local,func_true,func_false,func_nil};

    if (argc!=2){/*an dw8hke kati diaforetiko apo duo
                       orismata tote exoume error */
            printf("Wrong parameters! \n");
            exit(1);
    }
    else{   /*alliws an vre8ei to arxeio eisodou pairnei eisodo apo auto*/
            if(!(fp=fopen(argv[1],"r")))
            {
                    printf("Cannot open source file !\n");
                    exit(1);
            }
    }
	
    states=State0;
   
     // printf("Give an input from keyboard .. \n");
      c=fgetc(fp);
      while(c!=EOF){
	
       
	if(c=='\n'){
	  line++;
	}
        
	states= (action[states])(c,fp);
	prev=c;
	c=fgetc(fp);
	
      }

  
  PrintTokens(head);
}

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
   


enum state state0(char c,FILE *fp){
 
    i=0;
    empty_str(str);
   
    if ( (c == '+') || ( c =='-') || ( c =='/') || ( c =='*')|| ( c =='%') ||
    ( c =='=') || ( c =='>') || ( c =='<')|| (c=='!') ){
     	return Operant;
    }
    else if (isalpha(c)){
        i=0;
	str[i++] = c;
	
	if(c=='i')
	  return IF;
	else if (c=='e')
	  return ELSE;
	else if (c=='w')
	  return WHILE;
	else if (c=='b')
	  return BREAK;
	else if (c=='f')
	{
	  c=fgetc(fp);
	  if(c=='o'){
	    ungetc(c,fp);
	    return FOR;  
	  }
	  else if(c=='u'){
	    ungetc(c,fp);
	    return FUNCTION;  
	  }
	    else if(c=='a'){
	    ungetc(c,fp);
	    return FALSE;  
	  }
	}
	else if (c=='r')
	  return RETURN;
	else if (c=='c')
	  return CONTINUE;
	else if (c=='a')
	  return AND;
	else if (c=='o')
	  return OR;
	else if (c=='n')
	{
	    c=fgetc(fp);
	  if(c=='o'){
	    ungetc(c,fp);
	    return NOT;  
	  }
	  else if(c=='i'){
	    ungetc(c,fp);
	    return NIL;  
	  }
	}
	  
	else if (c=='l')
	  return LOCAL;
	else if (c=='t')
	  return TRUE;
	
        return Letter;
    }
    else if (isdigit(c)){
	i=0;
	str[i++] = c;
	return Number;
    }
    else if ( (c=='{')|| (c=='}')|| (c=='[')||(c==')') || (c==']')|| (c=='(')|| (c==')') ||
      (c==';') || (c==',') || (c==':')) {
	  
      i=0;
      str[i]=c;
      head=AddToken(head,line,++token,"PUNCTUATION_MARK",str);
      return State0;
      }
    else if ((c==':') || (c=='.') )
      {
		return Punc;
      }
     else if (c=='"')
     {
		return String;
     }
    else if ((c == ' ') || (c == '\t') || (c == '\r') ){
        c = fgetc(fp);
        while((c == ' ') || (c == '\t') || (c == '\r') ){
                c = fgetc(fp);
        }
        head=AddToken(head,line,++token,"WHITE SPACES "," ");
        ungetc(c,fp);
		return State0;
    }
    else{
        if (c == '\n') line++;
        else head=AddToken(head,line,++token,"Error, Not Supported Characters!"," ");
       return State0;
    }

    
}



enum state operant(char c,FILE *fp){
 


  switch(prev)
  {
    
    case '+' :
	 
	    if(c=='+') {
	      head=AddToken(head,line,++token,"OPERANT","++");
	      return State0;
	    }
	    else
	    {
	      head=AddToken(head,line,++token,"OPERANT","+");
	      ungetc(c,fp);
	      return State0;
	    }
    case '-' :
	    if(c=='-') {
	      head=AddToken(head,line,++token,"OPERANT","--");
	      return State0;
	    }
	    else
	    {
	      head=AddToken(head,line,++token,"OPERANT","--");
	      ungetc(c,fp);
	      return State0;
	    }
	  return State0;
    case '/' :
	  if(c=='/'){;
     
	    return Comment1;
	  }
	  if(c=='*'){
	    return Comment2;
	  }
      	  ungetc(c,fp);
	  head=AddToken(head,line,++token,"OPERANT","/");
	  return State0;
    case '*' :
	  ungetc(c,fp);
	  head=AddToken(head,line,++token,"OPERANT","*");
	  return State0;
    case '%' :
	 head=AddToken(head,line,++token,"OPERANT","%");
	  return State0;
    case '=' :
	       if(c=='=') {
	      head=AddToken(head,line,++token,"OPERANT","==");
	      return State0;
	    }
	    else
	    {
	      head=AddToken(head,line,++token,"OPERANT","=");
	      ungetc(c,fp);
	      return State0;
	    }
    case '>'  :
	      if(c=='=') {
	      head=AddToken(head,line,++token,"OPERANT",">=");
	      return State0;
	    }
	    else
	    {
	      head=AddToken(head,line,++token,"OPERANT",">");
	      ungetc(c,fp);
	      return State0;
	    }
    case  '<'	:
	        if(c=='=') {
	      head=AddToken(head,line,++token,"OPERANT","<=");;
	      return State0;
	    }
	    else
	    {
	      head=AddToken(head,line,++token,"OPERANT","<");
	      ungetc(c,fp);;
     
	      return State0;
	    }
     case   '!': 
	           if(c=='=') {
	      head=AddToken(head,line,++token,"OPERANT","!=");;
	      return State0;
	    }
	    else
	    {
	      head=AddToken(head, line,++token,"PUNCTUATION_MARK","!");
	      ungetc(c,fp);;
	    }
   
    default :
      
	  return State0;
  }
  
}

enum state punc (char c,FILE *fp){
  
  switch(prev)
  {
      case ':':
	      if(c==':') {
		head=AddToken(head, line,++token,"PUNCTUATION_MARK","::");
		return State0; printf("error \n");
	      }
	      else
	      {
		head=AddToken(head, line,++token,"PUNCTUATION_MARK",":");
		ungetc(c,fp);
		return State0;
	      }
	     
      case '.':
	        if(c=='.') {
		head=AddToken(head, line,++token,"PUNCTUATION_MARK","..");
		return State0;
	      }
	      else
	      {
		head=AddToken(head, line,++token,"PUNCTUATION_MARK",".");
		ungetc(c,fp);
		return State0;
	      }
	      
    
  }
  
}

enum state number(char c,FILE *fp){
    int DotPassed = 0;
    
    if (isdigit(c) || c == '.'){
	str[i++] = c;
	return Number;
    }
    else{
	int j;
	for (j=0; j<i; j++){
	  if(str[j]=='.'){
	    DotPassed++;
	  }
	    //printf("%c", str[j]);
	}
	if(DotPassed>1) 
	  head=AddToken(head,line,++token,"ERROR WRONG FORMAT","");
	  else if (DotPassed)  
	    head=AddToken(head,line,++token,"FLOATCONST",str);
	  else 
	    head=AddToken(head,line,++token,"INTCONST",str);
	return State0;
    }
}

enum state string(char c,FILE *fp){
	  int i = 0;
	  int error_flag = 1;
	  int warning_flag = 0;
	  
	  char *st=(char *)malloc(sizeof(char));
	  
	  *(st + i) = '\0';
          while((c!=EOF))
	  {			
		if(c=='\n'){
	  		line++;
		}
	  	while (c == '\\')
	      	{
			c = fgetc(fp);
			if(c=='\n'){
	  			line++;
			}
	      		if ( c == 'n'){
				
                        	*(st+i) = '\n';
			}
			else if ( c == 't' ){
				
				c = '\t';
                	        *(st+i) = c; 
			}
                        else if ( c == '\\' ){
                                
				*(st+i)='\\';
                        }
                        else if ( c == '\"' ){
                                
                                *(st+i)='"';
                        }
			else{
				if (!warning_flag){
					warning_flag = 1;
					++token;
				}				
	            *(st+i)='*';
				head=AddToken(head,line, token,"String WARNING, Unknown character found! a * character was added.\n","");
			}
			 st=(char *)realloc(st, ((++i)+1)*sizeof(char));
			  *(st+i)= '\0';
			c = fgetc(fp);
	      	}

		if (c == '\"'){
			error_flag = 0;
			if (!warning_flag) ++token;
			head=AddToken(head,line,token,"STRING",st);
			break;
		}
		else 
		{		  
		  *(st+i)=c;
		  st=(char *)realloc(st, ((++i)+1)*sizeof(char));
		  *(st + i ) = '\0';
		}
		c=fgetc(fp);
          }

	  if(error_flag){
		if (!warning_flag) ++token;
	  	head=AddToken(head,line,token,"STRING","Wrong Form");
		error_flag = 1;
	  }
	  i=0;
	  return State0;
}



enum state comment1(char c,FILE *fp){
  if(c!='\n'){
    return Comment1;
  }

 head=AddToken(head, line,++token,"COMMENT","//");
  return State0;
}



enum state comment2(char c,FILE *fp){
	int commentno=1;
	while(c!=EOF){
	 
	  if(commentno==0){
	    	head=AddToken(head,line,++token,"COMMENTS","RIGHT FORM");
	     ungetc(c,fp);
	    return State0;
	  }
		
	
	if(c=='\n'){
	  line++;
	}
	if(c=='/'){
		c=fgetc(fp);
		if(c=='*'){
			commentno++;
		}
		else{
			ungetc(c,fp);
		}
	}
	else if(c=='*'){
		c=fgetc(fp);
		if(c=='/'){
			commentno--;
		}
		else{
			ungetc(c,fp);
		}
	}
	c=fgetc(fp);
	}
	if(commentno!=0){
			head=AddToken(head,line,++token,"COMMENTS","Wrong Form");
	}
	
}
enum state letter(char c,FILE *fp){
  
   if (c =='_' || isalpha(c) || isdigit(c) ){
	str[i++] = c;
	
	return Letter;
   }
   else{
	
	
	

	
	  head=AddToken(head,line,++token,"IDENTIFIER",str);
	 
	
	i=0;
	 ungetc(c,fp);
	return State0;
   }
}
 

 
enum state func_if(char c,FILE *fp)
{
  str[i++]=c;
  switch(c)
  {
    
    case 'f' :
	   if(isalpha(c=fgetc(fp)))
	   {
	    ungetc(c,fp);
	    return Letter;
	   }
	   else
	   {
	      head=AddToken(head, line,++token,"KEYWORD","IF");
	      return State0;
	   }
    default  :
	    return Letter;
      
  }
}
enum state func_else(char c,FILE *fp)
{
  
  str[i++]=c;

  switch(c)
  {
    
	case 'l':
	      return ELSE;
	case 's':
	      return ELSE;
	case 'e':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		 head=AddToken(head, line,++token,"KEYWORD","ELSE");
		return State0;
	      }
	default :
	      
	      return Letter;
	
  }
}


enum state func_while(char c,FILE *fp)
{
  str[i++]=c;
  
  switch(c)
  {
    
	case 'w':
	      return WHILE;
	case 'h':
	      return WHILE;
	case 'i':
	      return WHILE;
	case 'l':
	      return WHILE;
	case 'e':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","WHILE");
		return State0;
	      }
	default :
	      return Letter;
	
  }
}

enum state func_break(char c,FILE *fp)
{
  str[i++]=c;
  switch(c)
  {
    
	case 'r':
	      return BREAK;
	case 'e':
	      return BREAK;
	case 'a':
	      return BREAK;
	case 'k':
	      
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","BREAK");
		return State0;
	      }
	default :
	      return Letter;
	
  }
}


enum state func_for(char c,FILE *fp){
  str[i++]=c;
  switch(c){
  	case 'o':
	      return FOR;
	case 'r':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","FOR");
		return State0;
	      }
	default :
	      return Letter;
	
  }
}
enum state func_function(char c,FILE *fp){
  str[i++]=c;
  switch(c){
  	case 'u':
	      return FUNCTION;
	case 'n':
	
	       if(isalpha(c=fgetc(fp)))
	      {
		
	      
		char temp=c;
		ungetc(c,fp);
		if(temp!='c'){
		   return Letter;
		}		
		else return FUNCTION;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","FUNCTION");
		return State0;
	      }
	      return FUNCTION;
	case 'c':
	      return FUNCTION;
	case 't':
	      return FUNCTION;
	case 'i':
	      return FUNCTION;	
	case 'o':
	      return FUNCTION;
	default :
	      return Letter;
	
  }
}

enum state func_return(char c,FILE *fp){
  
  str[i++]=c;
  switch(c){
  	case 'e':
	      return RETURN;
	case 't':
	      return RETURN;
	case 'u':
	      return RETURN;
	case 'r':
	      return RETURN;
	case 'n':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","RETURN");
		return State0;
	      }
	 default :
	      return Letter;
	
  }

}
enum state func_continue(char c,FILE *fp){
  
    str[i++]=c;

  switch(c){
  	case 'o':
	      return CONTINUE;
	case 'n':
	
	      if(isalpha(c=fgetc(fp)))
	      {
		char temp=c;
		ungetc(c,fp);
		
		if(temp=='t'||temp=='u'){
		
		return CONTINUE;
		}		
		else return Letter;
	      }
	
	case 't':
	      return CONTINUE;c=fgetc(fp);
	case 'i':
	      return CONTINUE;
	case 'u':
	      return CONTINUE;
	case 'e':
	  
	       if(isalpha(c=fgetc(fp)))
	      {
		char temp=c;
		
		
		ungetc(c,fp);
		 return Letter;
	      }
	      else
	      {
		 head=AddToken(head, line,++token,"KEYWORD","CONTINUE");
		return State0;c=fgetc(fp);
	      }
	 default :
	      return Letter;
  }

}


enum state func_and(char c,FILE *fp){
  
    str[i++]=c;
  switch(c){
  	case 'n':
	      return AND;
	case 'd':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","AND");
		return State0;
	      }
	default :
	      return Letter;
	
  }

}

enum state func_or(char c,FILE *fp){
    str[i++]=c;
  switch(c){
  	case 'r':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","OR");
		return State0;
	      }
	default :
	      return Letter;
	
  }

}


enum state func_not(char c,FILE *fp){    
  str[i++]=c;
  switch(c){
  	case 'o':
	      return NOT;
	case 't':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","NOT");
		return State0;
	      }
	default :
	      return Letter;
	
  }
  
}
enum state func_true(char c,FILE *fp){
   switch(c){
    	case 'r':
	      return TRUE;
	case 'u':
	      return TRUE;
	case 'e':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","TRUE");
		return State0;
	      }
	default :
	      return Letter;c=fgetc(fp);
}
}
enum state func_false(char c,FILE *fp){
   switch(c){
	
	case 'a':
	      return FALSE;
	case 'l':
	      return FALSE;
	case 's':
	      return FALSE;
	case 'e':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","FALSE");
		return State0;
	      }
	default :
	      return Letter;
}
}
enum state func_local(char c,FILE *fp)

  {
     switch(c){
      	case 'o':
	      return LOCAL;
	case 'c':
	      return LOCAL;
	case 'a':
	      return LOCAL;
	case 'l':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","LOCAL");
		return State0;
	      }
	default :
	      return Letter;
     }
}
enum state func_nil(char c,FILE *fp){
    switch(c){
  	case 'i':
	      return NIL;
	case 'l':
	      if(isalpha(c=fgetc(fp)))
	      {
		ungetc(c,fp);
		return Letter;
	      }
	      else
	      {
		  head=AddToken(head, line,++token,"KEYWORD","NIL");
		return State0;
	      }
	default :
	      return Letter;
}
}


void empty_str(char* str){
  int i;
  for(i=0;i<100;i++){
    str[i]='\0';
  }
  
}
