#ifndef SYMTABLE_TYPES_H
#define SYMTABLE_TYPES_H

typedef struct Variable {
	void * value;
} Variable;

typedef struct Function {
	void * args;
} Function;

typedef enum SymbolType {
	GLOBAL,
	LOCAL,
	FORMAL,
	USERFUNC,
	LIBFUNC
} SymbolType;

typedef struct SymTabEntry {
	const char *name;
	unsigned int line;
	int isActive;
	union {
		Variable *varVal;
		Function *funcVal;
	} value;
	unsigned int scope;
	enum SymbolType type;
	struct SymTabEntry * nextInScope;
	struct SymTabEntry * nextInHash;
} SymTabEntry;

Variable * new_Variable(void * value);

Function * new_Function(void * args);

SymTabEntry new_SymTabEntry(const char * name, unsigned int line, int isActive, Variable * varVal,
							Function * funcVal, unsigned int scope, SymbolType type);

const char * SymbolTypeToString(SymbolType type);

void print_SymTabEntry(SymTabEntry entry);

#endif