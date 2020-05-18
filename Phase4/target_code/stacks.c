#include "stacks.h"
#include <string.h>

void pushStringStack(StringStack **head, StringStack **tail, char *s){
	StringStack *temp = (StringStack *)malloc(sizeof(StringStack));

	if(FindStringStackIndex(*head, s) != - 1) return;

	temp->s = s;
	temp->next = NULL;
	if((*head) == NULL){
	 (*head)= temp;
	 return;
	}
	else if((*tail) == NULL){
	 (*head)->next = temp;
	 (*tail)= temp;
	 return;
	}
	(*tail)->next = temp;
}

int FindStringStackIndex(StringStack *head, char *s){
	StringStack *temp = head;
	int i = 0;
	while(temp != NULL){
		if(!strcmp(temp->s, s)) return i;
		temp = temp->next;
		i++;
	}
	return -1;
}

void pushNumStack(NumStack **head, NumStack **tail, double num){
	NumStack *temp = (NumStack *)malloc(sizeof(NumStack ));

	if(FindNumStackIndex(*head, num) != - 1) return;

	temp->num = num;
	temp->next = NULL;
	if((*head) == NULL){
	 (*head) = temp;
	 return;
	}
	else if((*tail) == NULL){
	 (*head)->next = temp;
	 (*tail) = temp;
	 return;
	}
	(*tail)->next = temp;
}

int FindNumStackIndex(NumStack *head, double num){
	NumStack *temp = head;
	int i = 0;
	while(temp != NULL){
		if(temp->num == num) return i;
		temp = temp->next;
		i++;
	}
	return -1;
}

void pushFunctionStack(FunctionStack **head, FunctionStack **tail, char *id, int address, int local_size){
	FunctionStack *temp = (FunctionStack*)malloc(sizeof(FunctionStack));

	if(FindFunctionStackIndex(*head, id) != - 1) return;

	temp->id = id;
	temp->address = address;
	temp->local_size = local_size;
	temp->next = NULL;
	if((*head) == NULL){
	 (*head) = temp;
	 return;
	}
	else if((*tail) == NULL){
	 (*tail)->next = temp;
	 (*tail) = temp;
	 return;
	}
	(*tail)->next = temp;
}

int FindFunctionStackIndex(FunctionStack *head, char *id){
	FunctionStack*temp = head;
	int i = 0;
	while(temp != NULL){
		if(!strcmp(temp->id,id)) return i;
		temp = temp->next;
		i++;
	}
	return -1;
}

void print_StringStack(StringStack *head){
	StringStack *temp = head;
	while(temp != NULL){
		printf("STRING: %s\n", temp->s);
		temp = temp->next;
	}
}
void print_LibraryStack(StringStack *head){
	StringStack *temp = head;
	while(temp != NULL){
		printf("LIBRARY: %s\n", temp->s);
		temp = temp->next;
	}
}
void print_FuncStack(FunctionStack *head){
	FunctionStack *temp = head;
	while(temp != NULL){
		printf("ID: %s ADDRESS: %d LOCALS: %d\n", temp->id, temp->address, temp->local_size);
		temp = temp->next;
	}
}
void print_NumStack(NumStack *head){
	NumStack *temp = head;
	while(temp != NULL){
		printf("NUM: %f\n", temp->num);
		temp = temp->next;
	}
} 