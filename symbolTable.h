#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "symbolTableDef.h"
#include "astDef.h"

SymbolTable* addToListOfSymbolTables(SymbolTable* symbolTable);
SymbolTableEntry* lookupSymbolTable(SymbolTable* symbolTable, char* identifier) ;
int insertintoSymbolTable(SymbolTable* symbolTable, SymbolTableEntry* entry);
SymbolTableEntry* createNewSymbolTableEntry(char* identifier, boolean isFunction, SymbolTable* tablePointer, Type type);
SymbolTable* createSymbolTable(char* tableID, SymbolTable* returnTable);
SymbolTable* getSymbolTable(char* identifier);
SymbolTable* initializeSymbolTable(astNode* root);
#endif