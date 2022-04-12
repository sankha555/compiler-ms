#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "symbolTableDef.h"
#include "astDef.h"

SymbolTable* init_symbolTable(astNode *root);
SymbolTableEntry* lookupSymbolTable(SymbolTable *table, char *name);

#endif