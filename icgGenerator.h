#ifndef ICG_GENERATOR_H
#define ICG_GENERATOR_H

#include "globalDef.h"
#include "typing.h"
#include "astDef.h"
#include "symbolTableDef.h"

/* 
*   create a new temporary variable in the symbol table, return the pointer to its entry
*/
SymbolTableEntry* getNewTemporary(SymbolTable* currentSymbolTable, TypeArrayElement* typeToAdd);

SymbolTableEntry* createRecordItemAlias(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable);

/*
*  
*/
int generateCompleteICGcode(astNode* root, SymbolTable* globalSymbolTable);

int parseICGcode(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable);

#endif