#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "symbolTableDef.h"

SymbolTable* addToListOfSymbolTables(SymbolTable* symbolTable);
int hashFunction(char* identifier);
SymbolTableEntry* loopkup(SymbolTable* symbolTable, char* identifier) ;
int insert(SymbolTable* symbolTable, SymbolTableEntry* entry);
SymbolTableEntry* createNewSymbolTableEntry(char* identifier, boolean isFunction, SymbolTableEntry* tablePointer, Type type, int width, int offset);
SymbolTable* createSymbolTable(char* tableID, SymbolTable* returnTable);
SymbolTable* getSymbolTable(char* identifier);
void populateSymbolTable(SymbolTable* symbolTable, astNode* functionRoot);
#endif