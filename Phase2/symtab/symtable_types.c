#include "symtable_types.h"

Variable new_Variable(void * value) {
	Variable v;
	v.value = value;

	return v;
}

Function new_Function(void * args) {
	Function f;
	f.args = args;

	return f;
}

SymTabEntry new_SymTabEntry(const char * name, unsigned int line, int isActive, Variable * varVal,
							Function * funcVal, unsigned int scope, SymbolType type, 
							SymTabEntry * nextInScope, SymTabEntry * nextInHash){
	SymTabEntry s;
	s.name = name;
	s.line = line;
	s.isActive = isActive;
	s.value.varVal = varVal;
	s.value.funcVal = funcVal;
	s.scope = scope;
	s.type = type;
	s.nextInScope = nextInScope;
	s.nextInHash = nextInHash;

	return s;
}
