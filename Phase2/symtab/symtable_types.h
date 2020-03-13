#ifndef SYMTABLE_TYPES_H
#define SYMTABLE_TYPES_H

typedef struct Variable {
	const char *name;
	unsigned int line;
	void * value;
} Variable;

typedef struct Function {
	const char *name;
	unsigned int line;
} Function;

typedef enum SymbolType {
	GLOBAL,
	LOCAL,
	FORMAL,
	USERFUNC,
	LIBFUNC
} SymbolType;

typedef struct SymTabEntry {
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

#endif
