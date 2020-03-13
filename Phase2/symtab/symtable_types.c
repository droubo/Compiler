#include "symtable_types.h"

Variable new_Variable(const char * name, int line, void * value) {
	Variable v;
	v.name = name;
	v.line = line;
	v.value = value;

	return v;
}

Function new_Function(const char * name, int line) {
	Function f;
	f.name = name;
	f.line = line;

	return f;
}

SymTabEntry new_SymTabEntry(int isActive, Variable * varVal, Function * funcVal, unsigned int scope, 
							SymbolType type, SymTabEntry * nextInScope, SymTabEntry * nextInHash){
	SymTabEntry s;
	s.isActive = isActive;
	s.value.varVal = varVal;
	s.value.funcVal = funcVal;
	s.scope = scope;
	s.type = type;
	s.nextInScope = nextInScope;
	s.nextInHash = nextInHash;

	return s;
}
