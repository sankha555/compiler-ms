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
int insertintoTypeTable(TypeTable* typeTable, TypeArrayElement* entry) {
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
struct TypeArrayElement* lookup(TypeTable* typeTable, char* identifier) {
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

struct TypeTable* createTypeTable(char* tableID){
    TypeTable* newTable = (TypeTable*) malloc(sizeof(TypeTable));
    strcpy(newTable->tableID, tableID);
    //always insert Int and Real entries into the type Table
    TypeArrayElement* entry;
    
    entry = createTypeArrayElement(0, "Int");
    entry->width =4;
    insertintoTypeTable(newTable, entry);
    entry = createTypeArrayElement(1, "Real");
    entry->width =8;
    insertintoTypeTable(newTable, entry);    
    return newTable;
}

struct TypeArrayElement* createTypeArrayElement(Type type, char *identifier){

    TypeArrayElement *entry = (TypeArrayElement*)malloc(sizeof(TypeArrayElement));
    entry->type = type;
    entry->identifier = identifier;
    entry->aliasTypeInfo = NULL;
    entry->compositeVariableInfo= NULL;
    entry->functionInfo = NULL;
    entry->next = NULL;
    entry->width = 0;
    return entry;

}

struct UnionOrRecordInfo* createUnionOrRecordinfo(char *identifier){

    UnionOrRecordInfo* unionOrRecord = (UnionOrRecordInfo*)malloc(sizeof(UnionOrRecordInfo));
    unionOrRecord->identifier = identifier;
    unionOrRecord->isRecord = false;
    unionOrRecord->isUnion = true;
    unionOrRecord->listOfFields = NULL;
    unionOrRecord->totalWidth =0;
    return unionOrRecord;

}

struct FunctionType *createFunctionType(char *identifier){

    FunctionType* func = (FunctionType*)malloc(sizeof(FunctionType));
    func->identifier = identifier;
    func->inputParameters = NULL;
    func->outputParameters = NULL;
    return func;
}

struct Field* createField(char *identifier, char* typeid){

    Field* field = (Field*)malloc(sizeof(Field));
    field->identifier = identifier;
    field->datatype = lookup(globalTypeTable, typeid);
    field->width = field->datatype->width;
    field->offset = 0;
    field->next = NULL;
    return field;

}

struct Parameter *createParameter(char *identifier, char* typeid){

    Parameter *parameter = (Parameter*)malloc(sizeof(Parameter));
    parameter->identifier = identifier;
    parameter->datatype = lookup(globalTypeTable, typeid);
    parameter->next= NULL;
    return parameter;

}


struct Field* addtoListofFields(char *identifier, char* typeid,Field *listofFields){

    Field* field = createField(identifier, typeid);
    Field* trav = listofFields;
    if(trav == NULL) 
        listofFields = field;
    while(trav->next != NULL)
        trav = trav->next;
    trav->next = field;
    return listofFields;

}

struct Parameter* addtoParameterList(char* identifier,char* typeid, Parameter* paramlist){

    Parameter* parameter= createParameter(identifier, typeid);
    Parameter* trav = paramlist;
    if(trav == NULL) 
        paramlist = parameter;
    while(trav->next != NULL)
        trav = trav->next;
    trav->next = parameter;
    return paramlist;


}
