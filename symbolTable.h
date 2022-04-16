/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "symbolTableDef.h"
#include "astDef.h"

void printSymbolTables(FILE* fp);
void printASingleSymbolTable(SymbolTable *symbolTable, FILE *fp);
SymbolTable* addToListOfSymbolTables(SymbolTable* symbolTable);
SymbolTableEntry* lookupSymbolTable(SymbolTable* symbolTable, char* identifier) ;
int insertintoSymbolTable(SymbolTable* symbolTable, SymbolTableEntry* entry);
SymbolTableEntry* createNewSymbolTableEntry(char* identifier, boolean isFunction, SymbolTable* tablePointer, TypeArrayElement *type, int width);
SymbolTable* createSymbolTable(char* tableID, SymbolTable* returnTable);
SymbolTable* getSymbolTable(char* identifier);
SymbolTable* initializeSymbolTable(astNode* root);
void parseTypeDefinitions(astNode *root);


//new temp functions added
SymbolTable *initializeSymbolTableNew(astNode *root);
int populateWidthandOffset(char *typeId);
void parseTypeDefinitionsPass2(char *typeId);
void parseTypeDefinitionsPass1(char *typeId);
char* getRecordOrUnionTypeExpression(UnionOrRecordInfo* info);
char* getType(SymbolTableEntry* entry);
void calculateFunctionOffsets(SymbolTable *globalSymbolTable);
#endif
