#include "symtable_types.h"
#include <stdio.h>

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

void print_SymTabEntry(SymTabEntry entry){
	printf("%s @%d | active: %d type: %s\n", entry.name, entry.line, entry.isActive, SymbolTypeToString(entry.type));
}

const char * SymbolTypeToString(SymbolType type){
	switch(type){
		case GLOBAL: return "GLOBAL";
		case LOCAL: return "LOCAL";
		case FORMAL: return "FORMAL";
		case USERFUNC: return "USERFUNC";
		case LIBFUNC: return "LIBFUNC";
	}
	return NULL;
}