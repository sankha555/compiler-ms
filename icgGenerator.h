#ifndef ICG_GENERATOR_H
#define ICG_GENERATOR_H

#include "symbolTableDef.h"
#include "globalDef.h"
#include "typing.h"
#include "astDef.h"

/* 
*   create a new temporary variable in the symbol table, return the pointer to its entry
*/
SymbolTableEntry* getNewTemporary(SymbolTable* currentSymbolTable, TypeArrayElement* typeToAdd);

/*
*  
*/
int generateCompleteICGcode(astNode* root);


int parseICGcode(astNode* root);

#endif ICG_GENERATOR_H