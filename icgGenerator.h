/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#ifndef ICG_GENERATOR_H
#define ICG_GENERATOR_H

#include "globalDef.h"
#include "typing.h"
#include "astDef.h"
#include "symbolTableDef.h"

/* 
*   create a new temporary variable in the symbol table, return the pointer to its entry
*/
SymbolTableEntry* getNewTemporary(SymbolTable* currentSymbolTable, Type type);

SymbolTableEntry* createRecordItemAlias(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable);

int generateCompleteICGcode(astNode* root, SymbolTable* globalSymbolTable);

void printICG(FILE* fp);

int parseICGcode(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable, boolean areInputParams, SymbolTableEntry* functionCalledSte);

#endif