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

}

SymbolTableEntry* createRecordItemAlias(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable) {

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
            SymbolTableEntry* funcSymbolTableEntry = lookupSymbolTable(globalSymbolTable,MAIN_NAME);
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


            break;

        default: 
            //do nothing for the astTags not mentioned
            break;

    }

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