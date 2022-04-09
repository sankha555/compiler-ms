#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#include "globalDef.h"
#include "symbolTableDef.h"
#include "astDef.h"

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

SymbolTableEntry* loopkup(SymbolTable* symbolTable, char* identifer) {
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

void populateSymbolTable(SymbolTable* symbolTable, astNode* functionRoot){
    return ;
}