#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#include "stdbool.h"
#include "symbolTableDef.h"
#include "astDef.h"
#include "typing.h"

SymbolTable* addToListOfSymbolTables(SymbolTable* symbolTable){
    if(listOfSymbolTables == NULL){
        listOfSymbolTables = symbolTable;
        symbolTable->next = NULL;
    }else{
        SymbolTable* head = listOfSymbolTables;
        while(head->next != NULL){
            head = head->next;
        }
        head->next = symbolTable;
        symbolTable->next = NULL;
    }

    return listOfSymbolTables;
}

int hashFunction(char* identifier) {
    int hash = 7;
    for(int i = 0; i < strlen(identifier); i++) {
        hash = (hash*31 + identifier[i])%K_MAP_SIZE;
    }
    return hash;
}

SymbolTableEntry* loopkup(SymbolTable* symbolTable, char* identifier) {
    int hashTableIndex = hashFunction(identifier);
    SymbolTableEntry* entry = symbolTable->tableEntries[hashTableIndex];
    while(entry != NULL) {
        if(strcmp(entry->identifier, identifier) == 0){
           return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

//1: item added in the hash table
//0: item updated in the table
int insertintoSymbolTable(SymbolTable* symbolTable, SymbolTableEntry* entry) {
    int hashTableIndex = hashFunction(entry->identifier);
    
    SymbolTableEntry* pointer = symbolTable->tableEntries[hashTableIndex];
    if(pointer == NULL) {
        symbolTable->tableEntries[hashTableIndex] = entry;
        return 1;
    }
    while(pointer->next != NULL) {
        if(strcmp(pointer->identifier, entry->identifier) == 0) {
            //entry already exists, update tag
            pointer = entry;
            return 0;    
        }
        pointer = pointer->next;
    }
    pointer->next = entry;
    return 1;
}

SymbolTableEntry* createNewSymbolTableEntry(char* identifier, boolean isFunction, SymbolTableEntry* tablePointer, Type type, int width, int offset){
    SymbolTableEntry* newEntry = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
    strcpy(newEntry->identifier, identifier);
    newEntry->isFunction = isFunction;
    newEntry->tablePointer =  tablePointer;
    newEntry->type = type;
    newEntry->width = width;
    newEntry->offset = offset;
    newEntry->next = NULL;

    return newEntry;
}

SymbolTable* createSymbolTable(char* tableID, SymbolTable* returnTable){
    SymbolTable* newTable = (SymbolTable*) malloc(sizeof(SymbolTable));
    strcpy(newTable->tableID, tableID);
    newTable->returnTo = returnTable;
    
    newTable->currentOffset = 0;    // is this correct tho? how do we compute the offset for each new table??
    // we don't need to compute the offset for each new table, as each new table represents a function, and the offset of the variables = offset of start of stack frame + relative offset
    // we can just include the total width if required, to gauge the total memory required by the symbol table
    
    listOfSymbolTables = addToListOfSymbolTables(newTable);

    return newTable;
}

SymbolTable* getSymbolTable(char* identifier){
    SymbolTable* head = listOfSymbolTables;
    while(head != NULL && strcmp(head->tableID, identifier) != 0){
        head = head->next;
    }
    return head;
}

void handleFunctionParameters(astNode* functionRootNode, SymbolTable* symbolTable){
    SymbolTable* newSymbolTable = (SymbolTable*) malloc(sizeof(SymbolTable));
    int offset = 0;

    // input parameters
    astNode* inputParameters = functionRootNode->children[1];   // this gives the linked list of parameters ig
    astNode* parameter = inputParameters;

    while(parameter != NULL){
        Type type = parameter->children[0]->type;
        char* identifier = parameter->children[1]->entry.lexeme;
        int width = getWidth(type);

        SymbolTableEntry* entry = createNewSymbolTableEntry(identifier, false, NULL, type, width, offset);
        insertintoSymbolTable(symbolTable, entry);

        offset += width;
    }

    //output parameters
    astNode* outputParameters = functionRootNode->children[2];
    parameter = outputParameters;

    while(parameter != NULL){
        Type type = parameter->children[0]->type;
        char* identifier = parameter->children[1]->entry.lexeme;
        int width = getWidth(type);

        SymbolTableEntry* entry = createNewSymbolTableEntry(identifier, false, NULL, type, width, offset);
        insertintoSymbolTable(symbolTable, entry);

        offset += width;
    }
}

/**
 * @brief Returns the symbol table entry for the given function root
 * 
 * @param root of the current function
 * @param wrapper symbol table
 * @return SymbolTable* 
 */
void populateOtherFunctionTable(astNode* root, SymbolTable* globalSymbolTable, SymbolTable* functionSymbolTable) {
    printf("Populating function table\n");
    printAbstractSyntaxTree(root, stdout);
    // the 3 children of the function node are:
    // 1. Input parameters
    // 2. Output variables
    // 3. Function Body Statements (Stmts)

    // 1. Input parameters

    // 2. Output variables

    // 3. Function Body Statements
    // <typeDefinitions> <declarations> <otherStmts> <returnStmt>
    // we need to parse typeDefinitions and declarations to get the type of the variables
    

    // If we see a record or union type, it must go in wrapper table
}

/**
 * @brief 
 * 
 * @param root 
 * @param globalSymbolTable 
 * @param functionSymbolTable 
 */
void populateMainFunctionTable(astNode* root, SymbolTable* globalSymbolTable, SymbolTable* functionSymbolTable) {
    printf("Populating main function table\n");
    printAbstractSyntaxTree(root, stdout);
    // Main does not have any input parameters, output variables
    // Only the function body statements (Stmts) are there

    // 1. Function Body Statements (Stmts)
    // <typeDefinitions> <declarations> <otherStmts> <returnStmt>
    // we need to parse typeDefinitions and declarations to get the type of the variables
    

    // If we see a record or union type, it must go in wrapper table
}

/**
 * @brief returns the WRAPPER Symbol Table - function table pointers, global variables, etc.
 * 
 * @param root of AST
 * @return SymbolTable* 
 */
SymbolTable* initializeSymbolTable(astNode* root) {
    SymbolTable* globalSymbolTable = createSymbolTable("GLOBAL", NULL);

    // AST ROOT is program
    // first child is otherFunc, second is Main
    astNode* otherFunctions = root->children[0];
    astNode* mainFunction = root->children[1];

    // go to otherFunc, it is a linked list of functions
    while (otherFunctions) {
        astNode* current = otherFunctions->data;
        SymbolTable* functionTable = createSymbolTable(current->children[0]->entry.lexeme, globalSymbolTable);
        // populate the symbol table for each function
        populateOtherFunctionTable(current, globalSymbolTable, functionTable);

        // TODO: add the function symbol table to the global symbol table

        otherFunctions = otherFunctions->next;
    }

    SymbolTable* mainFunctionSymbolTable = createSymbolTable("_main", globalSymbolTable);

    insertintoSymbolTable(globalSymbolTable, createNewSymbolTableEntry("_main", true, mainFunctionSymbolTable, NULL, 0, 0));

    populateMainFunctionTable(mainFunction, globalSymbolTable, mainFunctionSymbolTable);

    return globalSymbolTable;
}
