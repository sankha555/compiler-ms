#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stdbool.h"
#include "symbolTableDef.h"
#include "astGenerator.h"
#include "astDef.h"
#include "typing.h"
#include "globalDef.h"

void printSymbolTableEntry(SymbolTable *symbolTable, SymbolTableEntry *entry, FILE *fp)
{
    fprintf(fp, "\t Name: %30s ; Scope: %30s ; Width: %d ; Offset: %d; Is Global: %3s\n", entry->identifier, symbolTable->tableID, entry->width, entry->offset, (!strcmp(symbolTable->tableID, "GLOBAL") ? "YES" : "NO"));
    // printf("Entry Type: %d\n", entry->type);
    // fprintf(fp, "\t Type Name: %s\n", entry->type->identifier);
    fprintf(fp, "\t Type Expression:\n");
    printTypeArrayElement(stdout, entry->type);
    fprintf(fp, "\t Variable Usage: %s\n", entry->usage != NULL ? entry->usage : "undefined");
    fprintf(fp, "------------------------------------------------------------------\n");
}

void printASingleSymbolTable(SymbolTable *symbolTable, FILE *fp)
{
    fprintf(fp, "================ SYMBOL TABLE : %s ================\n", symbolTable->tableID);
    fprintf(fp, "Total Width: %d Bytes\n", symbolTable->totalWidth);
    fprintf(fp, "*** Entries in Table: *** \n");
    for (int i = 0; i < K_MAP_SIZE; i++)
    {
        SymbolTableEntry *head = symbolTable->tableEntries[i];
        while (head != NULL)
        {
            printSymbolTableEntry(symbolTable, head, fp);
            head = head->next;
        }
    }
    fprintf(fp, "================================================\n\n\n");
}

void printSymbolTables(FILE *fp)
{
    SymbolTable *head = listOfSymbolTables;
    while (head != NULL)
    {
        printASingleSymbolTable(head, fp);
        head = head->next;
    }
}

SymbolTable *addToListOfSymbolTables(SymbolTable *symbolTable)
{
    if (listOfSymbolTables == NULL)
    {
        listOfSymbolTables = symbolTable;
        symbolTable->next = NULL;
    }
    else
    {
        SymbolTable *head = listOfSymbolTables;
        while (head->next != NULL)
        {
            head = head->next;
        }
        head->next = symbolTable;
        symbolTable->next = NULL;
    }

    return listOfSymbolTables;
}

int hashFunctionSymbolTable(char *identifier)
{
    int hash = 7;
    for (int i = 0; i < strlen(identifier); i++)
    {
        hash = (hash * 31 + identifier[i]) % K_MAP_SIZE;
    }
    return hash;
}

SymbolTableEntry *lookupSymbolTable(SymbolTable *symbolTable, char *identifier)
{
    int hashTableIndex = hashFunctionSymbolTable(identifier);
    SymbolTableEntry *entry = symbolTable->tableEntries[hashTableIndex];
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

// 1: item added in the hash table
// 0: item updated in the table
int insertintoSymbolTable(SymbolTable *symbolTable, SymbolTableEntry *entry)
{
    int hashTableIndex = hashFunctionSymbolTable(entry->identifier);

    SymbolTableEntry *pointer = symbolTable->tableEntries[hashTableIndex];
    entry->offset = symbolTable->totalWidth;

    if (pointer == NULL)
    {
        symbolTable->tableEntries[hashTableIndex] = entry;
        symbolTable->totalWidth += entry->width;

        return 1;
    }

    while (pointer->next != NULL)
    {
        if (strcmp(pointer->identifier, entry->identifier) == 0)
        {
            // entry already exists, update tag
            pointer = entry;
            return 0;
        }
        pointer = pointer->next;
    }
    pointer->next = entry;
    symbolTable->totalWidth += entry->width;

    return 1;
}

SymbolTableEntry *createNewSymbolTableEntry(char *identifier, boolean isFunction, SymbolTable *tablePointer, TypeArrayElement *type, int width)
{
    SymbolTableEntry *newEntry = (SymbolTableEntry *)malloc(sizeof(SymbolTableEntry));
    newEntry->identifier = (char *)malloc(strlen(identifier) * sizeof(char));
    strcpy(newEntry->identifier, identifier);
    newEntry->isFunction = isFunction;
    newEntry->tablePointer = tablePointer;
    newEntry->type = type;
    newEntry->width = width;
    newEntry->offset = 0;
    newEntry->next = NULL;

    return newEntry;
}

SymbolTable *createSymbolTable(char *tableID, SymbolTable *returnTable)
{
    SymbolTable *newTable = (SymbolTable *)malloc(sizeof(SymbolTable));
    newTable->tableID = (char *)malloc(strlen(tableID) * sizeof(char));
    strcpy(newTable->tableID, tableID);

    newTable->returnTo = returnTable;
    newTable->totalWidth = 0;

    for (int i = 0; i < K_MAP_SIZE; i++)
    {
        newTable->tableEntries[i] = NULL;
    }

    // newTable->currentOffset = 0;    // is this correct tho? how do we compute the offset for each new table??
    // we don't need to compute the offset for each new table, as each new table represents a function, and the offset of the variables = offset of start of stack frame + relative offset
    // we can just include the total width if required, to gauge the total memory required by the symbol table

    listOfSymbolTables = addToListOfSymbolTables(newTable);

    return newTable;
}

SymbolTable *getSymbolTable(char *identifier)
{
    SymbolTable *head = listOfSymbolTables;
    while (head != NULL && strcmp(head->tableID, identifier) != 0)
    {
        head = head->next;
    }
    return head;
}

void parseInputParams(char *functionName, astNode *root, SymbolTable *globalSymbolTable, SymbolTable *symbolTable)
{
    FunctionType *funcType = lookupTypeTable(globalTypeTable, functionName)->functionInfo;

    while (root)
    {
        ASTtag dataType = root->data->children[0]->type;
        char *typeidentifier;
        char *identifier = root->data->children[1]->entry.lexeme;

        SymbolTableEntry *entry;
        TypeArrayElement *lookupResult;

        switch (dataType)
        {
        case TypeInt:
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupTypeTable(globalTypeTable, "Int"), getWidth(Integer));
            entry->usage = "input Parameter";

            insertintoSymbolTable(symbolTable, entry);

            addToInputParameters(identifier, "Int", funcType);
            break;

        case TypeReal:
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupTypeTable(globalTypeTable, "Real"), getWidth(Real));
            entry->usage = "input Parameter";

            insertintoSymbolTable(symbolTable, entry);

            addToInputParameters(identifier, "Real", funcType);
            break;
        case TypeRecord:
            typeidentifier = root->data->children[0]->entry.lexeme;
            lookupResult = lookupTypeTable(globalTypeTable, typeidentifier);
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupResult, lookupResult->width);
            entry->usage = "input Parameter";

            insertintoSymbolTable(symbolTable, entry);

            addToInputParameters(identifier, typeidentifier, funcType);
            break;
        case TypeUnion:
            typeidentifier = root->data->children[0]->entry.lexeme;
            lookupResult = lookupTypeTable(globalTypeTable, typeidentifier);
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupResult, lookupResult->width);
            entry->usage = "input Parameter";

            insertintoSymbolTable(symbolTable, entry);

            addToInputParameters(identifier, typeidentifier, funcType);
            break;
        default:
            break;
        }

        root = root->next;
    }
}

void parseOutputParams(char *functionName, astNode *root, SymbolTable *globalSymbolTable, SymbolTable *symbolTable)
{
    FunctionType *funcType = lookupTypeTable(globalTypeTable, functionName)->functionInfo;

    while (root)
    {
        ASTtag dataType = root->data->children[0]->type;
        char *typeidentifier;
        char *identifier = root->data->children[1]->entry.lexeme;

        SymbolTableEntry *entry;
        TypeArrayElement *lookupResult;

        switch (dataType)
        {
        case TypeInt:
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupTypeTable(globalTypeTable, "Int"), getWidth(Integer));
            entry->usage = "output Parameter";
            insertintoSymbolTable(symbolTable, entry);

            addToOutputParameters(identifier, "Int", funcType);
            break;

        case TypeReal:
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupTypeTable(globalTypeTable, "Real"), getWidth(Real));
            entry->usage = "output Parameter";
            insertintoSymbolTable(symbolTable, entry);

            addToOutputParameters(identifier, "Real", funcType);
            break;
        case TypeRecord:
            typeidentifier = root->data->children[0]->entry.lexeme;
            lookupResult = lookupTypeTable(globalTypeTable, typeidentifier);
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupResult, lookupResult->width);
            entry->usage = "output Parameter";
            insertintoSymbolTable(symbolTable, entry);

            addToOutputParameters(identifier, typeidentifier, funcType);
            break;
        case TypeUnion:
            typeidentifier = root->data->children[0]->entry.lexeme;
            lookupResult = lookupTypeTable(globalTypeTable, typeidentifier);
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupResult, lookupResult->width);
            entry->usage = "output Parameter";
            insertintoSymbolTable(symbolTable, entry);

            addToOutputParameters(identifier, typeidentifier, funcType);
            break;
        default:
            break;
        }

        root = root->next;
    }
}

void parseTypeDefinitions(astNode *root)
{
    printf("parsing type definitions\n");

    while (root != NULL)
    {
        TypeArrayElement *entry;
        if (root->data->type == TypeRecordDefinition)
        {
            entry = createTypeArrayElement(RecordType, root->data->children[0]->entry.lexeme);
            printf("parsing record definition\n");
            // record
            astNode *recordInfo = root->data->children[0];
            char *recordName = recordInfo->entry.lexeme;

            UnionOrRecordInfo *record = createUnionOrRecordinfo(recordName);
            record->isRecord = TRUE;
            entry->compositeVariableInfo = record;

            astNode *fieldsList = root->data->children[1];
            astNode *head = fieldsList;
            while (head != NULL)
            {
                astNode *field = head->data;

                char *fieldName = field->children[1]->entry.lexeme;

                astNode *fieldTypeInfo = field->children[0];
                switch (fieldTypeInfo->type)
                {
                case TypeInt:
                    addToListofFieldsRecord(fieldName, "Int", record);
                    break;

                case TypeReal:
                    addToListofFieldsRecord(fieldName, "Real", record);
                    break;

                case FieldTypeRUID:
                    addToListofFieldsRecord(fieldName, fieldTypeInfo->entry.lexeme, record);
                    break;
                }

                head = head->next;
            }
            entry->width = record->totalWidth;
            insertintoTypeTable(globalTypeTable, entry);
        }
        else if (root->data->type == TypeUnionDefinition)
        {
            // union
            entry = createTypeArrayElement(UnionType, root->data->children[0]->entry.lexeme);
            astNode *unionInfo = root->data->children[0];
            char *unionName = unionInfo->entry.lexeme;

            UnionOrRecordInfo *newUnion = createUnionOrRecordinfo(unionName);
            newUnion->isRecord = TRUE;
            entry->compositeVariableInfo = newUnion;

            astNode *fieldsList = root->data->children[1];
            astNode *head = fieldsList;
            while (head != NULL)
            {
                astNode *field = head->data;

                char *fieldName = field->children[1]->entry.lexeme;

                astNode *fieldTypeInfo = field->children[0];
                switch (fieldTypeInfo->type)
                {
                case TypeInt:
                    addToListofFieldsUnion(fieldName, "Int", newUnion);
                    break;

                case TypeReal:
                    addToListofFieldsUnion(fieldName, "Real", newUnion);
                    break;

                case FieldTypeRUID:
                    addToListofFieldsUnion(fieldName, fieldTypeInfo->entry.lexeme, newUnion);
                    break;
                }

                head = head->next;
            }
            entry->width = newUnion->totalWidth;

            insertintoTypeTable(globalTypeTable, entry);
        }
        else if (root->data->type == DefineType)
        {
            char *actualName = root->data->children[1]->entry.lexeme;
            char *newName = root->data->children[2]->entry.lexeme;
            TypeArrayElement *alias = createTypeArrayElement(Alias, newName);
            alias->aliasTypeInfo = lookupTypeTable(globalTypeTable, actualName);
            insertintoTypeTable(globalTypeTable, alias);
        }

        root = root->next;
    }
}

void parseDeclarations(astNode *root, SymbolTable *globalSymbolTable, SymbolTable *symbolTable)
{
    printf("parsing declarations\n");
    while (root)
    {
        ASTtag dataType = root->data->children[0]->type;
        char *typeidentifier;
        astNode *variable = root->data->children[1];

        char *identifier = variable->entry.lexeme;
        SymbolTableEntry *entry;

        bool isGlobal = root->data->children[2] ? true : false;

        TypeArrayElement *intTypeElement = lookupTypeTable(globalTypeTable, "Int");

        TypeArrayElement *lookupResult;

        switch (dataType)
        {
        case TypeInt:
            entry = createNewSymbolTableEntry(identifier, false, NULL, intTypeElement, getWidth(Integer));
            if (isGlobal)
            {
                entry->usage = "global Variable";
                insertintoSymbolTable(globalSymbolTable, entry);
            }
            else
            {
                entry->usage = "local Variable";
                insertintoSymbolTable(symbolTable, entry);
            }

            break;
        case TypeReal:
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupTypeTable(globalTypeTable, "Real"), getWidth(Real));
            if (isGlobal)
            {
                entry->usage = "global Variable";
                insertintoSymbolTable(globalSymbolTable, entry);
            }
            else
            {
                entry->usage = "local Variable";
                insertintoSymbolTable(symbolTable, entry);
            }

            break;
        case TypeRecord:
            printf("parsing record\n");
            typeidentifier = root->data->children[0]->entry.lexeme;
            lookupResult = lookupTypeTable(globalTypeTable, typeidentifier);
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupResult, lookupResult->width);
            if (isGlobal)
            {
                entry->usage = "global Variable";
                insertintoSymbolTable(globalSymbolTable, entry);
            }
            else
            {
                entry->usage = "local Variable";
                insertintoSymbolTable(symbolTable, entry);
            }
            break;

        case TypeUnion:
            printf("parsing union\n");
            typeidentifier = root->data->children[0]->entry.lexeme;
            lookupResult = lookupTypeTable(globalTypeTable, typeidentifier);
            entry = createNewSymbolTableEntry(identifier, false, NULL, lookupResult, lookupResult->width);
            if (isGlobal)
            {
                entry->usage = "global Variable";
                insertintoSymbolTable(globalSymbolTable, entry);
            }
            else
            {
                entry->usage = "local Variable";
                insertintoSymbolTable(symbolTable, entry);
            }
            break;

        default:
            break;
        }

        root = root->next;
    }
    printf("parsing declarations successful\n");
}

/**
 * @brief populate the symbol tables functions other than main
 * 
 * @param root
 * @param globalSymbolTable
 * @param symbolTable
 */
void populateOtherFunctionTable(astNode *root, SymbolTable *globalSymbolTable, SymbolTable *symbolTable)
{
    char *functionName = root->children[0]->entry.lexeme;

    TypeArrayElement *typeElement = createTypeArrayElement(Function, functionName);
    FunctionType *newFuncType = createFunctionType(functionName);

    typeElement->functionInfo = newFuncType;
    insertintoTypeTable(globalTypeTable, typeElement);

    astNode *inputParams = root->children[1];
    astNode *outputParams = root->children[2];
    astNode *stmts = root->children[3];

    parseInputParams(functionName, inputParams, globalSymbolTable, symbolTable);
    parseOutputParams(functionName, outputParams, globalSymbolTable, symbolTable);
    parseDeclarations(stmts->children[1], globalSymbolTable, symbolTable);
}

/**
 * @brief
 *
 * @param root
 * @param globalSymbolTable
 * @param functionSymbolTable
 */
void populateMainFunctionTable(astNode *root, SymbolTable *globalSymbolTable, SymbolTable *functionSymbolTable)
{
    TypeArrayElement *typeElement = createTypeArrayElement(Function, MAIN_NAME);
    typeElement->functionInfo = createFunctionType(MAIN_NAME);
    insertintoTypeTable(globalTypeTable, typeElement);

    astNode *stmts = root->children[0];
    parseDeclarations(stmts->children[1], globalSymbolTable, functionSymbolTable);
}

/**
 * @brief returns the GLOBAL Symbol Table - function table pointers, global variables, etc.
 *
 * @param root of AST
 * @return SymbolTable*
 */
SymbolTable *initializeSymbolTable(astNode *root)
{
    listOfSymbolTables = NULL;
    SymbolTable *globalSymbolTable = createSymbolTable("GLOBAL", NULL);

    astNode *otherFunctions = root->children[0];
    astNode *mainFunction = root->children[1];

    SymbolTableEntry *entry;

    astNode *head = otherFunctions;

    // go to otherFunc, it is a linked list of functions
    while (head)
    {
        astNode *current = head->data;
        astNode *stmts = current->children[3];
        parseTypeDefinitions(stmts->children[0]);
        head = head->next;
    }

    parseTypeDefinitions(mainFunction->children[0]->children[0]);

    head = otherFunctions;

    while (head)
    {
        astNode *current = head->data;
        SymbolTable *functionTable = createSymbolTable(current->children[0]->entry.lexeme, globalSymbolTable);
        populateOtherFunctionTable(current, globalSymbolTable, functionTable);
        entry = createNewSymbolTableEntry(current->children[0]->entry.lexeme, true, functionTable, lookupTypeTable(globalTypeTable,current->children[0]->entry.lexeme), 0);
        entry->usage = "function";
        insertintoSymbolTable(globalSymbolTable, entry);
        char *functionName = current->children[0]->entry.lexeme;
        head = head->next;
    }

    SymbolTable *mainFunctionSymbolTable = createSymbolTable(MAIN_NAME, globalSymbolTable);

    populateMainFunctionTable(mainFunction, globalSymbolTable, mainFunctionSymbolTable);

    entry = createNewSymbolTableEntry(MAIN_NAME, true, mainFunctionSymbolTable, lookupTypeTable(globalTypeTable, MAIN_NAME), 0);
    entry->usage = "main function";

    insertintoSymbolTable(globalSymbolTable, entry);
    // printSymbolTables(stdout);

    // printGlobalTypeTable(stdout);

    return globalSymbolTable;
}
