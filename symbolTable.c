#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#include "globalDef.h"
#include "symbolTableDef.h"
#include "astDef.h"
#include "astGenerator.h"
#include "lexer.h"


int hashFunction(char* identifier) {
    int hash = 7;
    for(int i = 0; i < strlen(identifier); i++) {
        hash = (hash*31 + identifier[i])%K_MAP_SIZE;
    }
    return hash;
}

SymbolTableEntry* lookup(SymbolTable* symbolTable, char* identifer) {
    int hashTableIndex = hashFunction(identifer);
    SymbolTableEntry* entry = symbolTable->tableEntries[hashTableIndex];
    while(entry != NULL) {
        if(strcmp(entry->identifier, identifer) == 0){
           return entry;
        }
        entry = entry->next;
    }
    return -1;
}

//1: item added in the hash table
//0: item updated in the table
int insert(SymbolTable* symbolTable, SymbolTableEntry* entry) {
    int hashTableIndex = kmapHash(entry->identifier);
    
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

// /* did you mean? */
// /*
// int insert(SymbolTable* symbolTable, SymbolTableEntry* entry) {
// 	char* lexeme = entry->identifier;
// 	int hashTableIndex = hashFunction(lexeme);

// 	SymbolTableEntry* pointer = symbolTable->tableEntries[hashTableIndex];
	
// 	// new entry
// 	if (pointer == NULL) {
// 		pointer = entry;
// 		return 0;
// 	}

// 	// update the existing entry
// 	while (pointer->next != NULL) {
// 		if (strcmp(pointer->identifier, lexeme) == 0) {
// 			pointer = entry;
// 			return 0;
// 		}
// 		pointer = pointer->next;
// 	}
// 	return 1;
// }
// */

SymbolTableEntry* createNewSymbolTableEntry(char* identifier, boolean isFunction, SymbolTableEntry* tablePointer, Type type, int width, int offset){
    SymbolTableEntry* newEntry = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
    strcpy(newEntry->identifier, identifier);
    newEntry->isFunction = isFunction;
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
    
    return newTable;
}

void printSymbolTable(SymbolTable *table, FILE *fp) {
    fprintf(fp, "Symbol Table: %s\n", table->tableID);
    fprintf(fp, "Current Offset: %d\n", table->currentOffset);
    fprintf(fp, "Total Width: %d\n", table->totalWidth);
    fprintf(fp, "Return To: %s\n", table->returnTo->tableID);
    fprintf(fp, "----------------------------------------------------\n");
    for(int i = 0; i < K_MAP_SIZE; i++) {
        SymbolTableEntry* entry = table->tableEntries[i];
        while(entry != NULL) {
            fprintf(fp, "Identifier: %s\n", entry->identifier);
            fprintf(fp, "Is Function: %d\n", entry->isFunction);

            if (entry->isFunction) {
                fprintf(fp, "Function Table Pointer: %s\n", entry->functionTablePointer->tableID);
                fprintf(fp, "Function Input Arguments: ");
                for(int j = 0; j < entry->functionInputArgumentCount; j++) {
                    fprintf(fp, "%s ", entry->functionInputArguments[j]);
                }
                fprintf(fp, "\n");
                fprintf(fp, "Function Output Arguments: ");
                for(int j = 0; j < entry->functionOutputArgumentCount; j++) {
                    fprintf(fp, "%s ", entry->functionOutputArguments[j]);
                }
                fprintf(fp, "\n");
            }
            fprintf(fp, "Is Union or Record: %d\n", entry->isUnionOrRecord);

            // if (entry->isUnionOrRecord) {
            //     fprintf(fp, "Union or Record Info: %s\n", entry->unionOrRecordInfo->typeName);
            // }

            fprintf(fp, "Type: %s\n", entry->type);
            fprintf(fp, "Width: %d\n", entry->width);
            fprintf(fp, "Offset: %d\n", entry->offset);
            fprintf(fp, "----------------------------------------------------\n");
            entry = entry->next;
        }
    }
}

// SymbolTable* getSymbolTable(char* identifier){
//     SymbolTable* head = listOfSymbolTables;
//     while(head != NULL && strcmp(head->tableID, identifier) != 0){
//         head = head->next;
//     }
//     return head;
// }

// void populateSymbolTable(SymbolTable* symbolTable, astNode* functionRoot){
//     return ;
// }

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
    populateMainFunctionTable(mainFunction, globalSymbolTable, mainFunctionSymbolTable);

    return globalSymbolTable;
}
