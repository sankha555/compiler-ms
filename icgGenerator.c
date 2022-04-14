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

int parseICGcode(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable, boolean areInputParams) {

    switch(root->type) {
        
        case Program: 
            
            pentupleCode[numberOfPentuples].rule = DEFINE_CS;
            numberOfPentuples++;

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable, areInputParams);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable, areInputParams);

            pentupleCode[numberOfPentuples].rule = DEFINE_DS;
            numberOfPentuples++;

            break;

        case FuncLinkedListNode:

            parseICGcode(root->data,currentSymbolTable,globalSymbolTable, areInputParams);
            if(root->next != NULL) {
                parseICGcode(root->next,currentSymbolTable,globalSymbolTable, areInputParams);
            }
            break;

        case FuncDef:

            // push EBP in stack, get current function's offsets and store them in EBP
            pentupleCode[numberOfPentuples].rule = FUNC_DEF; 
            SymbolTableEntry* funcSymbolTableEntry = lookupSymbolTable(globalSymbolTable,root->children[0]->entry.lexeme);
            pentupleCode[numberOfPentuples].result = funcSymbolTableEntry;
            numberOfPentuples++;

            parseICGcode(root->children[3],funcSymbolTableEntry->tablePointer,globalSymbolTable, areInputParams);

            pentupleCode[numberOfPentuples].rule = FUNC_DEF_END;
            numberOfPentuples++;

            break;

        case MainFunc:

            pentupleCode[numberOfPentuples].rule = FUNC_DEF_MAIN;
            SymbolTableEntry* funcSymbolTableEntry = lookupSymbolTable(globalSymbolTable,MAIN_NAME);
            pentupleCode[numberOfPentuples].result = funcSymbolTableEntry;
            numberOfPentuples++;

            parseICGcode(root->children[0],funcSymbolTableEntry->tablePointer,globalSymbolTable, areInputParams);

            pentupleCode[numberOfPentuples].rule = EXIT_CODE;
            numberOfPentuples++;

            break;

        case StmtLinkedListNode:

            parseICGcode(root->data,currentSymbolTable,globalSymbolTable, areInputParams);
            if(root->next!=NULL) {
                parseICGcode(root->next,currentSymbolTable,globalSymbolTable, areInputParams);
            }

            break;

        case AssignmentOperation:

            SymbolTableEntry* result = createRecordItemAlias(root->children[0],currentSymbolTable,globalSymbolTable);
            
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable, areInputParams);
            
            SymbolTableEntry* copier = lookupSymbolTable(currentSymbolTable,root->children[1]->dataPlace);
            
            //check that RHS is int while LHS is real, if yes, need to typecast
            if(result->type->type == Real && copier->type->type == Integer) {
                    
                pentupleCode[numberOfPentuples].rule = CONVERT_TO_REAL;
                pentupleCode[numberOfPentuples].result = result;
                pentupleCode[numberOfPentuples].argument[0] = copier;

            } else if (result->type->type == Real && copier->type->type == Real) {

                pentupleCode[numberOfPentuples].rule = ASSIGN_OP_REAL;
                pentupleCode[numberOfPentuples].result = result;
                pentupleCode[numberOfPentuples].argument[0] = copier;

            } else {
                
                pentupleCode[numberOfPentuples].rule = ASSIGN_OP_INT;
                pentupleCode[numberOfPentuples].result = result;
                pentupleCode[numberOfPentuples].argument[0] = copier;

            }

            numberOfPentuples++;
            
            break;

        case FuncCall:

            //figure out which function is being called
            SymbolTableEntry* functionToBeCalled = lookupSymbolTable(globalSymbolTable,root->children[1]->entry.lexeme);

            //setup edx pointer to transfer input parameters from current scope to called function scope
            pentupleCode[numberOfPentuples].rule = SETUP_FUNC_CALL_PARAM_TRANSFER;
            pentupleCode[numberOfPentuples].result = functionToBeCalled;
            numberOfPentuples++;

            //start copying input parameters into proper locations
            parseICGcode(root->children[2],currentSymbolTable,globalSymbolTable, TRUE);

            //call the function
            pentupleCode[numberOfPentuples].rule = CALL_FUNC;
            pentupleCode[numberOfPentuples].result = functionToBeCalled;
            numberOfPentuples++;

            //setup edx pointer to transfer output parameters from called function scope to current scope
            pentupleCode[numberOfPentuples].rule = SETUP_FUNC_CALL_PARAM_TRANSFER;
            pentupleCode[numberOfPentuples].result = functionToBeCalled;
            numberOfPentuples++;

            //start copying output params into proper locations
            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable, FALSE);

            break;
        
        case IdLinkedListNode:

            SymbolTableEntry* parasList[MAX_ARGUMENTS];

            astNode* ptr = root;
            int i = 0;

            while(ptr != NULL) {
                
                

            }

            if(areInputParams == TRUE) {

                

            } else {
                
                

            }
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

    return parseICGcode(root,globalSymbolTable,globalSymbolTable,FALSE);

}