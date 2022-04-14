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
#include "lexerdef.h"

int tempVariableNumber;

SymbolTableEntry* getNewTemporary(SymbolTable* currentSymbolTable, TypeArrayElement* typeToAdd) {
    return NULL;
}

SymbolTableEntry* createRecordItemAlias(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable) {
    astNode* curr = root;
    
    char* recIdentifier = curr->data->entry.lexeme;
    printf("Rec Id: %s\n", recIdentifier);

    SymbolTableEntry* entry = lookupSymbolTable(currentSymbolTable, recIdentifier);
    if(entry == NULL){
        printf("Null entry...\n");
    }

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

SymbolTableEntry* findVariable(char* identifier, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable) {
    
    SymbolTableEntry* temp;
    temp = lookupSymbolTable(globalSymbolTable, identifier);
    if(temp == NULL) {
        temp = lookupSymbolTable(currentSymbolTable, identifier);
    }
    return temp;

}

int parseICGcode(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable, boolean areInputParams, SymbolTableEntry* functionCalledSte) {

    switch(root->type) {
        
        case Program: 
            
            pentupleCode[numberOfPentuples].rule = DEFINE_CS;
            numberOfPentuples++;

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable, areInputParams, functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable, areInputParams, functionCalledSte);

            pentupleCode[numberOfPentuples].rule = DEFINE_DS;
            numberOfPentuples++;

            break;

        case FuncLinkedListNode:

            parseICGcode(root->data,currentSymbolTable,globalSymbolTable, areInputParams,functionCalledSte);
            if(root->next != NULL) {
                parseICGcode(root->next,currentSymbolTable,globalSymbolTable, areInputParams,functionCalledSte);
            }
            break;

        case FuncDef:

            // push EBP in stack, get current function's offsets and store them in EBP
            pentupleCode[numberOfPentuples].rule = FUNC_DEF; 
            SymbolTableEntry* funcSymbolTableEntry = lookupSymbolTable(globalSymbolTable,root->children[0]->entry.lexeme);
            pentupleCode[numberOfPentuples].result = funcSymbolTableEntry;
            numberOfPentuples++;

            parseICGcode(root->children[3],funcSymbolTableEntry->tablePointer,globalSymbolTable, areInputParams, functionCalledSte);

            pentupleCode[numberOfPentuples].rule = FUNC_DEF_END;
            numberOfPentuples++;

            break;

        case MainFunc:

            pentupleCode[numberOfPentuples].rule = FUNC_DEF_MAIN;
            funcSymbolTableEntry = lookupSymbolTable(globalSymbolTable,MAIN_NAME);
            pentupleCode[numberOfPentuples].result = funcSymbolTableEntry;
            numberOfPentuples++;

            parseICGcode(root->children[0],funcSymbolTableEntry->tablePointer,globalSymbolTable, areInputParams, functionCalledSte);

            pentupleCode[numberOfPentuples].rule = EXIT_CODE;
            numberOfPentuples++;

            break;

        case StmtLinkedListNode:

            parseICGcode(root->data,currentSymbolTable,globalSymbolTable, areInputParams, functionCalledSte);
            if(root->next!=NULL) {
                parseICGcode(root->next,currentSymbolTable,globalSymbolTable, areInputParams, functionCalledSte);
            }

            break;

        case AssignmentOperation:
            ;
            SymbolTableEntry* result = createRecordItemAlias(root->children[0],currentSymbolTable,globalSymbolTable);
            
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable, areInputParams, functionCalledSte);
            
            SymbolTableEntry* copier = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
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
            parseICGcode(root->children[2],currentSymbolTable,globalSymbolTable, TRUE, functionToBeCalled);

            //call the function
            pentupleCode[numberOfPentuples].rule = CALL_FUNC;
            pentupleCode[numberOfPentuples].result = functionToBeCalled;
            numberOfPentuples++;

            //setup edx pointer to transfer output parameters from called function scope to current scope
            pentupleCode[numberOfPentuples].rule = SETUP_FUNC_CALL_PARAM_TRANSFER;
            pentupleCode[numberOfPentuples].result = functionToBeCalled;
            numberOfPentuples++;

            //start copying output params into proper locations
            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable, FALSE, functionToBeCalled);

            break;
        
        case IdLinkedListNode:

            immediateOrSTE* parasList[MAX_ARGUMENTS];
            FunctionParameter* functionParameters;

            if(areInputParams == TRUE) {

                astNode* ptr = root;
                int i = 0;
                
                //get all actual parameters present in the currentSymbolTable or immediate values
                while(ptr != NULL) {
                    parasList[i] = (immediateOrSTE*)malloc(sizeof(immediateOrSTE));
                    if(ptr->data->entry.type == TK_NUM || ptr->data->entry.type == TK_RNUM) {
                        parasList[i]->isSTE = FALSE;
                        parasList[i]->immediate = ptr->data->entry;
                    } else {
                        parasList[i]->ste = findVariable(ptr->data->entry.lexeme, currentSymbolTable, globalSymbolTable);
                        parasList[i]->isSTE = TRUE;
                    }
                    i++;
                    ptr = ptr->next;
                }

                //get all function parameters symbolTableEntries in the functionSymbolTable
                functionParameters = getFunctionParameters(functionCalledSte->identifier,FALSE);

                FunctionParameter* temp = functionParameters;
                int j = 0;

                while(temp != NULL) {
                    SymbolTableEntry* tempVar = lookupSymbolTable(functionCalledSte->tablePointer,temp->identifier);

                    if(parasList[i]->isSTE == FALSE) {
                        
                        pentupleCode[numberOfPentuples].rule = PUSH_INPUT_IMMEDIATE;
                        pentupleCode[numberOfPentuples].result = tempVar;
                        pentupleCode[numberOfPentuples].immVal = parasList[i]->immediate;
                        numberOfPentuples++;

                    } else {

                        pentupleCode[numberOfPentuples].rule = PUSH_INPUT_VAR;
                        pentupleCode[numberOfPentuples].result = tempVar;
                        pentupleCode[numberOfPentuples].argument[0] = parasList[i]->ste;
                        numberOfPentuples++;

                    }

                    j++;
                    temp = temp->next;
                }

            } else {
                
                astNode* ptr = root;
                int i = 0;

                //get all actual parameters present in the currentSymbolTable or immediate values
                while(ptr != NULL) {
                    parasList[i] = (immediateOrSTE*)malloc(sizeof(immediateOrSTE));
                    parasList[i]->ste = findVariable(ptr->data->entry.lexeme, currentSymbolTable, globalSymbolTable);
                    parasList[i]->isSTE = TRUE;
                    i++;
                    ptr = ptr->next;
                }
                
                //get all function parameters symbolTableEntries in the functionSymbolTable
                functionParameters = getFunctionParameters(functionCalledSte->identifier,TRUE);

                FunctionParameter* temp = functionParameters;
                int j = 0;

                while(temp != NULL) {
                    SymbolTableEntry* tempVar = lookupSymbolTable(functionCalledSte->tablePointer,temp->identifier);

                    pentupleCode[numberOfPentuples].rule = POP_OUTPUT;
                    pentupleCode[numberOfPentuples].result = parasList[i]->ste;
                    pentupleCode[numberOfPentuples].argument[0] = tempVar;
                    numberOfPentuples++;

                    j++;
                    temp = temp->next;
                }

            }
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
        pentupleCode[i].intValue = 0;
        pentupleCode[i].realValue = 0;
    }

    return parseICGcode(root,globalSymbolTable,globalSymbolTable,FALSE,NULL);

}