/**
 *  al.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "alpha_token.h"


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

}


