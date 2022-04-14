#include "globalDef.h"
#include <stdio.h>
#include <string.h>
#include "astDef.h"
#include "globalDef.h"
#include "symbolTable.h"
#include "symbolTableDef.h"
#include "typing.h"
#include "icgGenerator.h"
#include "icgGeneratorDef.h"

int tempVariableNumber;

SymbolTableEntry* getNewTemporary(SymbolTable* currentSymbolTable, TypeArrayElement* typeToAdd) {
    return NULL;
}

SymbolTableEntry* createRecordItemAlias(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable) {
    astNode* curr = root;
    char* recIdentifier = curr->data->entry.lexeme;
    printf("Rec Id: %s\n", recIdentifier);

    SymbolTableEntry* entry = lookupSymbolTable(currentSymbolTable, recIdentifier);

    int finalOffset = entry->offset;
    Type finalType = entry->type->type;
    char* finalIdentifier = recIdentifier;
    int finalWidth = entry->width;

    UnionOrRecordInfo* recInfo = entry->type->compositeVariableInfo;

    while(curr->next != NULL){
        Field* field = recInfo->listOfFields; // points to first field of the record

        curr = curr->next;
        recIdentifier = curr->data->entry.lexeme;   // now I know which field to search for in the list of fields

        while(field != NULL){
            if(!strcmp(field->identifier, recIdentifier)){
                break;
            }
            field = field->next;
        }

        if(field == NULL){
            printf("Bruh, field does not exist!\n");
        }

        finalIdentifier = field->identifier;
        finalOffset += field->offset;
        finalWidth = field->width;

        if(field->datatype->compositeVariableInfo == NULL){
            finalType = field->datatype->type;
            break;
        }

        recInfo = field->datatype->compositeVariableInfo;
    }
    
    SymbolTableEntry* finalAliasEntry = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
    strcpy(finalAliasEntry->identifier, finalIdentifier);
    finalAliasEntry->offset = finalOffset;
    finalAliasEntry->type = createTypeArrayElement(finalType, finalAliasEntry->identifier);
    finalAliasEntry->width = finalWidth;

    return finalAliasEntry;
}

int parseICGcode(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable) {

    switch(root->type) {
        
        case Program: 
            
            pentupleCode[numberOfPentuples].rule = DEFINE_CS;
            numberOfPentuples++;

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable);

            pentupleCode[numberOfPentuples].rule = DEFINE_DS;
            numberOfPentuples++;

            break;

        case FuncLinkedListNode:

            parseICGcode(root->data,currentSymbolTable,globalSymbolTable);
            if(root->next != NULL) {
                parseICGcode(root->next,currentSymbolTable,globalSymbolTable);
            }
            break;

        case FuncDef:

            // push EBP in stack, get current function's offsets and store them in EBP
            pentupleCode[numberOfPentuples].rule = FUNC_DEF; 
            SymbolTableEntry* funcSymbolTableEntry = lookupSymbolTable(globalSymbolTable,root->children[0]->entry.lexeme);
            pentupleCode[numberOfPentuples].result = funcSymbolTableEntry;
            numberOfPentuples++;

            parseICGcode(root->children[3],funcSymbolTableEntry->tablePointer,globalSymbolTable);

            pentupleCode[numberOfPentuples].rule = FUNC_DEF_END;
            numberOfPentuples++;

            break;

        case MainFunc:

            pentupleCode[numberOfPentuples].rule = FUNC_DEF_MAIN;
            funcSymbolTableEntry = lookupSymbolTable(globalSymbolTable,MAIN_NAME);
            pentupleCode[numberOfPentuples].result = funcSymbolTableEntry;
            numberOfPentuples++;

            parseICGcode(root->children[0],funcSymbolTableEntry->tablePointer,globalSymbolTable);

            pentupleCode[numberOfPentuples].rule = EXIT_CODE;
            numberOfPentuples++;

            break;

        case StmtLinkedListNode:

            parseICGcode(root->data,currentSymbolTable,globalSymbolTable);
            if(root->next!=NULL) {
                parseICGcode(root->next,currentSymbolTable,globalSymbolTable);
            }

            break;

        case AssignmentOperation:

            SymbolTableEntry* result = createRecordItemAlias(root->children[0],currentSymbolTable,globalSymbolTable);
            
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable);
            
            pentupleCode[numberOfPentuples].rule = ASSIGN_OP;
            pentupleCode[numberOfPentuples].result = result;
            
            SymbolTableEntry* copier = lookupSymbolTable(currentSymbolTable,root->children[1]->dataPlace);
            pentupleCode[numberOfPentuples].argument[0] = copier;
            
            numberOfPentuples++;
            
            break;

        case FuncCall:

            //copying input parameters to function location

            break;
        default: 
            //do nothing for the astTags not mentioned
            break;

    }
    return -1;
}


int generateCompleteICGcode(astNode* root, SymbolTable* globalSymbolTable) {

    tempVariableNumber = 0;
    numberOfPentuples = 0;

    for(int i = 0; i < MAX_PENTUPLES_POSSIBLE; i++) {
        pentupleCode[i].label = NULL;
        pentupleCode[i].rule = -1;
        pentupleCode[i].result = NULL;
        pentupleCode[i].argument[0] = NULL;
        pentupleCode[i].argument[1] = NULL;
    }

    return parseICGcode(root,globalSymbolTable,globalSymbolTable);

}