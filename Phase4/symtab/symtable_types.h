#ifndef _SYMTABLE_TYPES_H
#define _SYMTABLE_TYPES_H

typedef struct Variable {
	void * value;
} Variable;

typedef struct Function {
	void * args;
	int num_of_args;
	int num_of_locals;
	int iaddress;
	int jump_label;
} Function;

typedef enum SymbolType {
	GLOBAL,
	LOCAL,
	FORMAL,
	USERFUNC,
	LIBFUNC
} SymbolType;

typedef enum scopespace_t
{
	programvar,
	functionlocal,
	formalarg
} scopespace_t;

typedef struct SymTabEntry {
	const char *name;
	unsigned int line;
	int isActive;
	union {
		Variable *varVal;
		Function *funcVal;
	} value;
	unsigned int scope;
	unsigned int func_scope;

	/* for offset */
	unsigned int offset;
	scopespace_t space;
	/*------------*/

	/*for target code*/
	unsigned int taddress;

	enum SymbolType type;
	struct SymTabEntry * nextInScope;
	struct SymTabEntry * nextInHash;
} SymTabEntry;

Variable * new_Variable(void * value);

Function * new_Function(void * args);

SymTabEntry new_SymTabEntry(const char * name, unsigned int line, int isActive, Variable * varVal,
							Function * funcVal, unsigned int scope, unsigned int func_scope, SymbolType type);

const char * SymbolTypeToString(SymbolType type);

void print_SymTabEntry(SymTabEntry entry);

#endif