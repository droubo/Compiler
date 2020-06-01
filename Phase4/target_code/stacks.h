#include <stdio.h>
#include <stdlib.h>

typedef struct StringStack {
	char *s;
	struct StringStack *next;
} StringStack;

typedef struct FunctionStack {
	char *id;
	int address;
	int local_size;
	struct FunctionStack *next;
} FunctionStack;

typedef struct NumStack {
	double num;
	struct NumStack *next;
} NumStack;


int FindStringStackSize(StringStack *head);
int FindNumStackSize(NumStack *head);
int FindFunStackSize(FunctionStack *head);

void pushStringStack(StringStack **head, StringStack **tail, char *s);
int FindStringStackIndex(StringStack *head, char *s);

void pushNumStack(NumStack **head, NumStack **tail, double num);
int FindNumStackIndex(NumStack *head, double num);

void pushFunctionStack(FunctionStack **head, FunctionStack **tail, char *id, int address, int local_size);
int FindFunctionStackIndex(FunctionStack *head, char *id, int address);

void print_Stack(StringStack *head);
void print_StringStack(StringStack *head);
void print_LibraryStack(StringStack *head);
void print_NumStack(NumStack *head);
void print_FuncStack(FunctionStack *head);

