#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "typing.h"
#include "globalDef.h"

void printUnionOrRecordInfo(FILE* fp, UnionOrRecordInfo *info)
{
    if (info == NULL)
    {
        fprintf(fp, "NULL\n");
        return;
    }
    fprintf(fp, "identifier: %s, isUnion: %d, isRecord: %d, totalWidth: %d\n", info->identifier, info->isUnion, info->isRecord, info->totalWidth);
    Field *field = info->listOfFields;
    while (field != NULL)
    {
        fprintf(fp, "identifier: %s, datatype: %s\n", field->identifier, field->datatype->identifier);
        field = field->next;
    }
}

void printFunctionInfo(FILE *fp, FunctionType *info)
{
    if (info == NULL)
    {
        fprintf(fp, "NULL\n");
        return;
    }
    fprintf(fp, "identifier: %s\n", info->identifier);
    FunctionParameter *parameter = info->inputParameters;
    while (parameter != NULL)
    {
        fprintf(fp, "identifier: %s, datatype: %s\n", parameter->identifier, parameter->datatype->identifier);
        parameter = parameter->next;
    }
    parameter = info->outputParameters;
    while (parameter != NULL)
    {
        fprintf(fp, "identifier: %s, datatype: %s\n", parameter->identifier, parameter->datatype->identifier);
        parameter = parameter->next;
    }
}

void printAliasInfo(FILE *fp, TypeArrayElement *info)
{
    if (info == NULL)
    {
        fprintf(fp, "NULL\n");
        return;
    }
    fprintf(fp, "is alias of: %s\n", info->identifier);
}

void printTypeArrayElement(FILE* fp, TypeArrayElement *t)
{
    if (t == NULL)
    {
        fprintf(fp, "NULL");
        return;
    }
    switch (t->type)
    {
    case Integer:
        fprintf(fp, "Integer \t %s\n", t->identifier);
        break;
    case Real:
        fprintf(fp, "Real \t %s\n", t->identifier);
        break;
    case UnionType:
        fprintf(fp, "Union \t %s\n", t->identifier);
        printUnionOrRecordInfo(fp, t->compositeVariableInfo);
        break;
    case RecordType:
        fprintf(fp, "Record \t %s\n", t->identifier);
        printUnionOrRecordInfo(fp, t->compositeVariableInfo);
        break;
    case Function:
        fprintf(fp, "Function \t %s\n", t->identifier);
        printFunctionInfo(fp, t->functionInfo);
        break;
    case Alias:
        fprintf(fp, "Alias \t %s\n", t->identifier);
        printAliasInfo(fp, t->aliasTypeInfo);
        break;
    default:
        fprintf(fp, "Unknown");
        break;
    }
}

void printGlobalTypeTable(FILE* fp)
{
    fprintf(fp, "================ TYPE TABLE ================\n");
    for (int i = 0; i < K_MAP_SIZE; i++)
    {
        TypeArrayElement *head = globalTypeTable->tableEntries[i];
        while (head != NULL)
        {
            printTypeArrayElement(fp, head);
            head = head->next;
        }
    }
    fprintf(fp, "================================================\n\n\n");
}

int getWidth(Type type)
{
    int width;
    switch (type)
    {
    case Integer:
        width = 4;
        break;
    case Real:
        width = 8;
        break;
    default:
        break;
    }
    return width;
}

// the TypeArray will be implemented as a HashTable
//  basically, identifier of the type will be used to index into the Hash Table where each entry will be a linkedList(in order to deal with collisions)

// inserts the type element into the given type table, returns 1 when entered, 0 if the entry already exists
int insertintoTypeTable(TypeTable *typeTable, TypeArrayElement *entry)
{
    int hashTableIndex = hashFunctionSymbolTable(entry->identifier);
    printf("hash index: %d\n", hashTableIndex);

    TypeArrayElement *pointer = typeTable->tableEntries[hashTableIndex];
    printf("got to here\n");
    if (pointer == NULL)
    {
        printf("pointer is null\n");
        typeTable->tableEntries[hashTableIndex] = entry;
        printf("inserted\n");
        return 1;
    }
    while (pointer->next != NULL)
    {
        printf("pointer is not null\n");
        if (strcmp(pointer->identifier, entry->identifier) == 0)
        {
            // entry already exists, update tag
            pointer = entry;
            return 0;
        }
        pointer = pointer->next;
    }
    pointer->next = entry;
    return 1;
}

// returns the entry is found in the symbol table, else returns NULL
struct TypeArrayElement *lookupTypeTable(TypeTable *typeTable, char *identifier)
{
    int hashTableIndex = hashFunctionSymbolTable(identifier);

    TypeArrayElement *entry = typeTable->tableEntries[hashTableIndex];
    while (entry != NULL)
    {
        if (strcmp(entry->identifier, identifier) == 0)
        {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

struct TypeTable *createTypeTable(char *tableID)
{
    TypeTable *newTable = (TypeTable *)malloc(sizeof(TypeTable));

    newTable->tableID = (char *)malloc(strlen(tableID) * sizeof(char));
    strcpy(newTable->tableID, tableID);

    for (int i = 0; i < 32; i++)
    {
        newTable->tableEntries[i] = NULL;
    }

    // always insert Int and Real entries into the type Table
    TypeArrayElement *entry;

    entry = createTypeArrayElement(0, "Int");
    entry->width = 4;
    insertintoTypeTable(newTable, entry);
    printf("Inserted into type table\n");
    entry = createTypeArrayElement(1, "Real");
    entry->width = 8;
    insertintoTypeTable(newTable, entry);
    return newTable;
}

struct TypeArrayElement *createTypeArrayElement(Type type, char *identifier)
{

    TypeArrayElement *entry = (TypeArrayElement *)malloc(sizeof(TypeArrayElement));
    entry->type = type;

    entry->identifier = (char *)malloc(strlen(identifier) * sizeof(char));
    strcpy(entry->identifier, identifier);

    entry->aliasTypeInfo = NULL;
    entry->compositeVariableInfo = NULL;
    entry->functionInfo = NULL;
    entry->next = NULL;
    entry->width = 0;
    return entry;
}

struct UnionOrRecordInfo *createUnionOrRecordinfo(char *identifier)
{

    UnionOrRecordInfo *unionOrRecord = (UnionOrRecordInfo *)malloc(sizeof(UnionOrRecordInfo));

    unionOrRecord->identifier = (char *)malloc(strlen(identifier) * sizeof(char));
    strcpy(unionOrRecord->identifier, identifier);
    unionOrRecord->isRecord = false;
    unionOrRecord->isUnion = false;
    unionOrRecord->listOfFields = NULL;
    unionOrRecord->totalWidth = 0;
    return unionOrRecord;
}

struct FunctionType *createFunctionType(char *identifier)
{

    FunctionType *func = (FunctionType *)malloc(sizeof(FunctionType));

    func->identifier = (char *)malloc(strlen(identifier) * sizeof(char));
    strcpy(func->identifier, identifier);
    func->inputParameters = NULL;
    func->outputParameters = NULL;
    return func;
}

struct Field *createField(char *identifier, char *typeid)
{

    Field *field = (Field *)malloc(sizeof(Field));

    field->identifier = (char *)malloc(strlen(identifier) * sizeof(char));
    strcpy(field->identifier, identifier);

    field->datatype = lookupTypeTable(globalTypeTable, typeid);
    field->width = field->datatype->width;
    field->offset = 0;
    field->next = NULL;
    return field;
}

struct FunctionParameter *createParameter(char *identifier, char *typeid)
{

    FunctionParameter *parameter = (FunctionParameter *)malloc(sizeof(FunctionParameter));

    parameter->identifier = (char *)malloc(strlen(identifier) * sizeof(char));
    strcpy(parameter->identifier, identifier);

    parameter->datatype = lookupTypeTable(globalTypeTable, typeid);
    parameter->next = NULL;
    return parameter;
}

void addToListofFieldsRecord(char *identifier, char *typeid, UnionOrRecordInfo *info)
{

    Field *field = createField(identifier, typeid);
    field->offset = info->totalWidth;
    Field *trav = info->listOfFields;
    if (trav == NULL)
    {
        info->listOfFields = field;
    }
    else
    {
        while (trav->next != NULL)
        {
            trav = trav->next;
        }
        trav->next = field;
    }

    info->totalWidth += field->width;
}

void addToListofFieldsUnion(char *identifier, char *typeid, UnionOrRecordInfo *info)
{

    Field *field = createField(identifier, typeid);
    field->offset = 0;
    Field *trav = info->listOfFields;
    if (trav == NULL)
    {
        info->listOfFields = field;
    }
    else
    {
        while (trav->next != NULL)
        {
            trav = trav->next;
        }
        trav->next = field;
    }

    info->totalWidth = field->width > info->totalWidth ? field->width : info->totalWidth;
}

void addToInputParameters(char *identifier, char *typeid, FunctionType *info)
{

    FunctionParameter *parameter = createParameter(identifier, typeid);
    FunctionParameter *trav = info->inputParameters;
    if (trav == NULL)
    {
        info->inputParameters = parameter;
    }
    else
    {
        while (trav->next != NULL)
        {
            trav = trav->next;
        }
        trav->next = parameter;
    }
}

void addToOutputParameters(char *identifier, char *typeid, FunctionType *info)
{

    FunctionParameter *parameter = createParameter(identifier, typeid);
    FunctionParameter *trav = info->outputParameters;
    if (trav == NULL)
    {
        info->outputParameters = parameter;
    }
    else
    {
        while (trav->next != NULL)
        {
            trav = trav->next;
        }
        trav->next = parameter;
    }
}
