#include<string.h>
#include<stdlib.h>
#include<stdio.h>


#include "typing.h"
#include "globalDef.h"

int getWidth(Type type){
    switch (type)
    {
        case Integer:
            return 4;  
        case Real:
            return 8;      
        default:
            break;
    }
}

//the TypeArray will be implemented as a HashTable 
// basically, identifier of the type will be used to index into the Hash Table where each entry will be a linkedList(in order to deal with collisions)



int hashFunction(char* identifier) {
    
    //hash function for the hash table
    int hash = 7;
    for(int i = 0; i < strlen(identifier); i++) {
        hash = (hash*31 + identifier[i])%K_MAP_SIZE;
    }
    return hash;
}

//inserts the type element into the given type table, returns 1 when entered, 0 if the entry already exists
int insert(TypeTable* typeTable, TypeArrayElement* entry) {
    int hashTableIndex = hashFunction(entry->identifier);
    
    TypeArrayElement* pointer = typeTable->tableEntries[hashTableIndex];
    if(pointer == NULL) {
        typeTable->tableEntries[hashTableIndex] = entry;
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


//returns the entry is found in the symbol table, else returns NULL
TypeArrayElement* loopkup(TypeTable* typeTable, char* identifier) {
    int hashTableIndex = hashFunction(identifier);
    TypeArrayElement* entry = typeTable->tableEntries[hashTableIndex];
    while(entry != NULL) {
        if(strcmp(entry->identifier, identifier) == 0){
           return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

TypeTable* createTypeTable(char* tableID){
    TypeTable* newTable = (TypeTable*) malloc(sizeof(TypeTable));
    strcpy(newTable->tableID, tableID);
    //always insert Int and Real entries into the type Table

 
    
    
    return newTable;
}

