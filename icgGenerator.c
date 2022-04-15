#include "globalDef.h"
#include <stdio.h>
#include <string.h>
#include "astDef.h"
#include "astGenerator.h"
#include "globalDef.h"
#include "symbolTable.h"
#include "symbolTableDef.h"
#include "typing.h"
#include "icgGenerator.h"
#include "icgGeneratorDef.h"
#include "lexerDef.h"

int tempVariableNumber;
int tempLabelNumber;

SymbolTableEntry* getNewTemporary(SymbolTable* currentSymbolTable, Type type) {
    TypeArrayElement* desiredTypePtr;
    if(type == Integer) {
        desiredTypePtr = intPtr;
    }
    if(type == Real) {
        desiredTypePtr = realPtr;
    }
    if(type == Boolean) {
        desiredTypePtr = booleanPtr;
    }
    char* temp = (char*)malloc(30);
    sprintf(temp,"tempVariable_xvag_%d",tempVariableNumber);
    tempVariableNumber++;
    //SymbolTableEntry* createNewSymbolTableEntry(char* identifier, boolean isFunction, SymbolTable* tablePointer, Type type);
    SymbolTableEntry* newVar = createNewSymbolTableEntry(temp,FALSE,currentSymbolTable,desiredTypePtr,type);
    insertintoSymbolTable(currentSymbolTable,newVar);
    return newVar;
}

void printICG(FILE* fp) {
    
    char *icgNames[] = {
        "ADD_I",      // add two integers in memory
        "ADD_R",      // add two reals in memory
        "SUB_I",      // subtract two integers in memory
        "SUB_R",      // subtract two reals in memory
        "MUL_I",      // multiply two integers in memory
        "MUL_R",      // multiply two reals in memory
        "DIV_I",      // divide two integers in memory
        "DIV_R",      // divide two reals in memory
        "GOTO_L",     // unconditional jump to label
        "IF_TRUE_GOTO_L", 
        "IF_FALSE_GOTO_L",
        "ASSIGN_OP_INT",  // copy from one memory location 2 bytes to another
        "ASSIGN_OP_REAL", // copy from one mem 4 bytes to another mem
        "DEFINE_DS",  // define data segment, in code generation, adds function offsets
        "DEFINE_CS",  // define code segment, add "global _start:" in code generation
        "EXIT_CODE",  // in the end of intermediate code generation, adds the exit syscall during code generation
        "FUNC_DEF",   // function label in the code segment, push EBP into stack, and move the function offset from constant DS into EBP
        "FUNC_DEF_END", // pop from stack into EBP, then return
        "FUNC_DEF_MAIN",
        "CONVERT_TO_REAL",
        "SETUP_CALL_TRANS",
        "PUSH_INPUT_VAR",
        "PUSH_INPUT_IMMEDIATE",
        "CALL_FUNC",
        "POP_OUTPUT",
        "INSERT_LABEL",
        "READ",
        "WRITE_IMMEDIATE",
        "WRITE_VAR",
        "ASSIGN_IMMEDIATE_INT",
        "ASSIGN_IMMEDIATE_REAL",
        "BOOLEAN_NOT",
        "BOOLEAN_AND",
        "BOOLEAN_OR",
        "REL_EQ_INT",
        "REL_EQ_REAL",
        "REL_GEQ_INT",
        "REL_GEQ_REAL",
        "REL_GT_INT",
        "REL_GT_REAL",
        "REL_LEQ_INT",
        "REL_LEQ_REAL",
        "REL_LT_INT",
        "REL_LT_REAL",
        "REL_NEQ_INT",
        "REL_NEQ_REAL"
    };

    fprintf(fp,"\n\n%25s%25s%25s%25s%20s%20s\n","ICG Function Name","Result","Argument_1","Argument_2","Immediate_val","jump label");

    for(int i = 0; i < numberOfPentuples; i++) {
        pentuple* temp = &pentupleCode[i];
        
        char tempResult[30];
        if(temp->result == NULL) {
            sprintf(tempResult,"NULL");
        } else {
            sprintf(tempResult,"%s",temp->result->identifier);
        }
        
        char tempArg1[30];
        if(temp->argument[0] == NULL) {
            sprintf(tempArg1,"NULL");
        } else {
            sprintf(tempArg1,"%s",temp->argument[0]->identifier);
        }
        
        char tempArg2[30];
        if(temp->argument[1] == NULL) {
            sprintf(tempArg2,"NULL");
        } else {
            sprintf(tempArg2,"%s",temp->argument[1]->identifier);
        }

        char tempLexeme[30];
        if(temp->immVal.lexeme == NULL) {
            sprintf(tempLexeme,"NULL");
        } else {
            sprintf(tempLexeme,"%s",temp->immVal.lexeme);
        }

        char tempLabel[30];
        if(temp->jumpLabel == NULL) {
            sprintf(tempLabel,"NULL");
        } else {
            sprintf(tempLabel,"%s",temp->jumpLabel);
        }
        
        fprintf(fp,"%25s%25s%25s%25s%20s%20s\n",icgNames[temp->rule],tempResult,tempArg1,tempArg2,tempLexeme,tempLabel);
        // fprintf(fp,"%20s%20s%20s%20s%30s%20s\n",icgNames[temp->rule],temp->result->identifier,temp->argument[0]->identifier,temp->argument[1]->identifier,temp->immVal.lexeme,temp->jumpLabel);
    }

    return;
}

char* generateNewLabel() {
    char* temp = (char*)malloc(30);
    sprintf(temp,"controlLabel_%d",tempLabelNumber);
    tempLabelNumber++;
    return temp;
}

// SymbolTableEntry* createRecordItemAlias(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable) {
//     astNode* curr = root;
    
//     char* recIdentifier = curr->data->entry.lexeme;
//     printf("Rec Id: %s\n", recIdentifier);

//     SymbolTableEntry* entry = lookupSymbolTable(currentSymbolTable, recIdentifier);
//     if(entry == NULL){
//         printf("Null entry...\n");
//     }

//     int finalOffset = entry->offset;
//     Type finalType = entry->type->type;
//     char* finalIdentifier = recIdentifier;
//     int finalWidth = entry->width;

//     UnionOrRecordInfo* recInfo = entry->type->compositeVariableInfo;

//     while(curr->next != NULL){
//         Field* field = recInfo->listOfFields; // points to first field of the record

//         curr = curr->next;
//         recIdentifier = curr->data->entry.lexeme;   // now I know which field to search for in the list of fields

//         while(field != NULL){
//             if(!strcmp(field->identifier, recIdentifier)){
//                 break;
//             }
//             field = field->next;
//         }

//         if(field == NULL){
//             printf("Bruh, field does not exist!\n");
//         }

//         finalIdentifier = field->identifier;
//         finalOffset += field->offset;
//         finalWidth = field->width;

//         if(field->datatype->compositeVariableInfo == NULL){
//             finalType = field->datatype->type;
//             break;
//         }

//         recInfo = field->datatype->compositeVariableInfo;
//     }
    
//     SymbolTableEntry* finalAliasEntry = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
//     strcpy(finalAliasEntry->identifier, finalIdentifier);
//     finalAliasEntry->offset = finalOffset;
//     finalAliasEntry->type = createTypeArrayElement(finalType, finalAliasEntry->identifier);
//     finalAliasEntry->width = finalWidth;

//     return finalAliasEntry;
// }

SymbolTableEntry* findVariable(char* identifier, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable) {
    
    SymbolTableEntry* temp;
    temp = lookupSymbolTable(globalSymbolTable, identifier);
    if(temp == NULL) {
        temp = lookupSymbolTable(currentSymbolTable, identifier);
    }
    return temp;

}

SymbolTableEntry* createRecordItemAlias(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable) {

    SymbolTableEntry* retResult = NULL;

    if(root->next == NULL) { //it is a single instance variable id
        retResult = findVariable(root->data->entry.lexeme,currentSymbolTable,globalSymbolTable);
        return retResult;
    }

    return retResult;

}


int parseICGcode(astNode* root, SymbolTable* currentSymbolTable, SymbolTable* globalSymbolTable, boolean areInputParams, SymbolTableEntry* functionCalledSte) {

    SymbolTableEntry *leftOperand;
    SymbolTableEntry *rightOperand;
    SymbolTableEntry *holder;
    SymbolTableEntry *boolResult;
    
    if(root == NULL){
        printf("Bruh\n");
    }

    printf("Root type: %s\n", getStatmType(root->type));

    switch(root->type) {
        
        case Program: 
            
            pentupleCode[numberOfPentuples].rule = DEFINE_CS;
            numberOfPentuples++;

            if(root->children[0] != NULL) {
                parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable, areInputParams, functionCalledSte);
            }
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

        case Stmts:

            parseICGcode(root->children[2],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
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
            
            // printf("%s\n",root->children[1]->dataPlace);
            // printf("%s\n",copier->identifier);
            // printf("%d\t\n",copier->type->type);
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

            ;
            //figure out which function is being called
            SymbolTableEntry* functionToBeCalled = lookupSymbolTable(globalSymbolTable,root->children[1]->entry.lexeme);

            //setup edx pointer to transfer input parameters from current scope to called function scope
            pentupleCode[numberOfPentuples].rule = SETUP_CALL_TRANS;
            pentupleCode[numberOfPentuples].result = functionToBeCalled;
            numberOfPentuples++;

            //start copying input parameters into proper locations
            parseICGcode(root->children[2],currentSymbolTable,globalSymbolTable, TRUE, functionToBeCalled);

            //call the function
            pentupleCode[numberOfPentuples].rule = CALL_FUNC;
            pentupleCode[numberOfPentuples].result = functionToBeCalled;
            numberOfPentuples++;

            //setup edx pointer to transfer output parameters from called function scope to current scope
            pentupleCode[numberOfPentuples].rule = SETUP_CALL_TRANS;
            pentupleCode[numberOfPentuples].result = functionToBeCalled;
            numberOfPentuples++;

            //start copying output params into proper locations
            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable, FALSE, functionToBeCalled);

            break;
        
        case IdLinkedListNode:
            
            ;
            immediateOrSTE parasList[MAX_ARGUMENTS];
            FunctionParameter* functionParameters;

            if(areInputParams == TRUE) {

                astNode* ptr = root;
                int i = 0;

                // printf("collecting all input params.\n");
                
                //get all actual parameters present in the currentSymbolTable or immediate values
                while(ptr != NULL) {
                    if(ptr->data->entry.type == TK_NUM || ptr->data->entry.type == TK_RNUM) {
                        // printf("Its a constant ffs.\n");
                        parasList[i].isSTE = FALSE;
                        parasList[i].immediate = ptr->data->entry;
                    } else {
                        // printf("Its a variable ffs.\n");
                        parasList[i].ste = findVariable(ptr->data->entry.lexeme, currentSymbolTable, globalSymbolTable);
                        // printf("%s\n.",parasList[i].ste->identifier);
                        parasList[i].isSTE = TRUE;
                    }
                    i++;
                    ptr = ptr->next;
                }

                // printf("Collected all input params.\n");

                //get all function parameters symbolTableEntries in the functionSymbolTable
                functionParameters = getFunctionParameters(functionCalledSte->identifier,FALSE);

                // printf("Got the functional parameters.\n");

                FunctionParameter* temp = functionParameters;
                int j = 0;

                // printf("Adding copy code.\n");

                while(temp != NULL) {
                    
                    // printf("Fetching input param location of %s\n",temp->identifier);

                    SymbolTableEntry* tempVar = lookupSymbolTable(functionCalledSte->tablePointer,temp->identifier);

                    // printf("Fetched input param location of %s\n",temp->identifier);

                    if(parasList[j].isSTE == FALSE) {
                        
                        // printf("Actual param is an immediate value.\n");
                        pentupleCode[numberOfPentuples].rule = PUSH_INPUT_IMMEDIATE;
                        pentupleCode[numberOfPentuples].result = tempVar;
                        pentupleCode[numberOfPentuples].immVal = parasList[j].immediate;
                        numberOfPentuples++;

                    } else {
                        
                        // printf("Actual param is a variable.\n");
                        pentupleCode[numberOfPentuples].rule = PUSH_INPUT_VAR;
                        pentupleCode[numberOfPentuples].result = tempVar;
                        pentupleCode[numberOfPentuples].argument[0] = parasList[j].ste;
                        numberOfPentuples++;

                    }

                    // printf("Added input transfer for %s.\n",temp->identifier);
                    j++;
                    temp = temp->next;
                }

                // printf("added all copy statements.\n");

            } else {
                
                astNode* ptr = root;
                int i = 0;

                //get all actual parameters present in the currentSymbolTable or immediate values
                while(ptr != NULL) {
                    parasList[i].ste = findVariable(ptr->data->entry.lexeme, currentSymbolTable, globalSymbolTable);
                    parasList[i].isSTE = TRUE;
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
                    pentupleCode[numberOfPentuples].result = parasList[j].ste;
                    pentupleCode[numberOfPentuples].argument[0] = tempVar;
                    numberOfPentuples++;

                    j++;
                    temp = temp->next;
                }

            }

            break;

        case While: 

            ;
            //insert a label in the code
            char* temp = generateNewLabel();
            pentupleCode[numberOfPentuples].rule = INSERT_LABEL;
            pentupleCode[numberOfPentuples].jumpLabel = temp;
            numberOfPentuples++;

            //add all the statements within the loop in the code
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            //add the label to jump
            pentupleCode[numberOfPentuples].rule = IF_TRUE_GOTO_L;
            pentupleCode[numberOfPentuples].result = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            pentupleCode[numberOfPentuples].jumpLabel = temp;
            numberOfPentuples++;

            break;

        case If:

            //there is no else part to the if
            if(root->children[2] == NULL) {
                
                char* temp = generateNewLabel();
                pentupleCode[numberOfPentuples].rule = IF_FALSE_GOTO_L;
                pentupleCode[numberOfPentuples].result = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
                pentupleCode[numberOfPentuples].jumpLabel = temp;
                numberOfPentuples++;

                parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

                pentupleCode[numberOfPentuples].rule = INSERT_LABEL;
                pentupleCode[numberOfPentuples].jumpLabel = temp;
                numberOfPentuples++;
                
            } else {  //contains an else body

                char* temp1 = generateNewLabel();
                char* temp2 = generateNewLabel();

                pentupleCode[numberOfPentuples].rule = IF_FALSE_GOTO_L;
                pentupleCode[numberOfPentuples].result = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
                pentupleCode[numberOfPentuples].jumpLabel = temp1;
                numberOfPentuples++;

                parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

                pentupleCode[numberOfPentuples].rule = GOTO_L;
                pentupleCode[numberOfPentuples].jumpLabel = temp2;
                numberOfPentuples++;

                pentupleCode[numberOfPentuples].rule = INSERT_LABEL;
                pentupleCode[numberOfPentuples].jumpLabel = temp1;
                numberOfPentuples++;

                //write ICG code for statements within the else block
                parseICGcode(root->children[2]->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

                pentupleCode[numberOfPentuples].rule = INSERT_LABEL;
                pentupleCode[numberOfPentuples].jumpLabel = temp2;
                numberOfPentuples++;


            }
            break;
        
        case Read:

            pentupleCode[numberOfPentuples].rule = READ;
            pentupleCode[numberOfPentuples].result = createRecordItemAlias(root->children[0],currentSymbolTable,globalSymbolTable);
            numberOfPentuples++;
            
            break;

        case Write:

            if(root->children[0]->type == Num || root->children[0]->type == RealNum) {
                
                pentupleCode[numberOfPentuples].rule = WRITE_IMMEDIATE;
                pentupleCode[numberOfPentuples].immVal = root->children[0]->entry;
                numberOfPentuples++;

            } else {

                pentupleCode[numberOfPentuples].rule = WRITE_VAR;
                pentupleCode[numberOfPentuples].result = createRecordItemAlias(root->children[0],currentSymbolTable,globalSymbolTable);
                numberOfPentuples++;

            }

            break;

        case arithOp_PLUS:

            /* The + operation is valid between :-
            *  ->   int, int
            *  ->   int, real
            *  ->   real, int
            *  ->   real, real
            *  ->   record, record,  not yet handled
            *  both the children of the node will have dataPlace field, which will 
            *  contain the symbolTableEntry to the temporary holding the expression's
            *  calculated value. Check for its type and work accordingly
            */

            //write ICG code calculating for both the children
            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            //the variable which hold the value of the expression below
            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
            //checking what type of operand pair
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {

                holder = getNewTemporary(currentSymbolTable,Integer);
                pentupleCode[numberOfPentuples].rule = ADD_I;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Integer && rightOperand->type->type == Real) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //need to convert integer operand to real in a temporary
                SymbolTableEntry* convertHolder = getNewTemporary(currentSymbolTable,Real);
                pentupleCode[numberOfPentuples].rule = CONVERT_TO_REAL;
                pentupleCode[numberOfPentuples].result = convertHolder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                numberOfPentuples++;

                //perform real addition
                pentupleCode[numberOfPentuples].rule = ADD_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = convertHolder;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Real && rightOperand->type->type == Integer) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //need to convert integer operand to real in a temporary
                SymbolTableEntry* convertHolder = getNewTemporary(currentSymbolTable,Real);
                pentupleCode[numberOfPentuples].rule = CONVERT_TO_REAL;
                pentupleCode[numberOfPentuples].result = convertHolder;
                pentupleCode[numberOfPentuples].argument[0] = rightOperand;
                numberOfPentuples++;

                //perform real addition
                pentupleCode[numberOfPentuples].rule = ADD_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = convertHolder;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Real && rightOperand->type->type == Real) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //perform real addition
                pentupleCode[numberOfPentuples].rule = ADD_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } 

            root->dataPlace = holder->identifier;

            break;

        case arithOp_MINUS:

            /* The - operation is valid between :-
            *  ->   int, int
            *  ->   int, real
            *  ->   real, int
            *  ->   real, real
            *  ->   record, record,  not yet handled
            *  both the children of the node will have dataPlace field, which will 
            *  contain the symbolTableEntry to the temporary holding the expression's
            *  calculated value. Check for its type and work accordingly
            */

            //write ICG code calculating for both the children
            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            //the variable which hold the value of the expression below
            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
            //checking what type of operand pair
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {

                holder = getNewTemporary(currentSymbolTable,Integer);
                pentupleCode[numberOfPentuples].rule = SUB_I;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Integer && rightOperand->type->type == Real) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //need to convert integer operand to real in a temporary
                SymbolTableEntry* convertHolder = getNewTemporary(currentSymbolTable,Real);
                pentupleCode[numberOfPentuples].rule = CONVERT_TO_REAL;
                pentupleCode[numberOfPentuples].result = convertHolder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                numberOfPentuples++;

                //perform real addition
                pentupleCode[numberOfPentuples].rule = SUB_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = convertHolder;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Real && rightOperand->type->type == Integer) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //need to convert integer operand to real in a temporary
                SymbolTableEntry* convertHolder = getNewTemporary(currentSymbolTable,Real);
                pentupleCode[numberOfPentuples].rule = CONVERT_TO_REAL;
                pentupleCode[numberOfPentuples].result = convertHolder;
                pentupleCode[numberOfPentuples].argument[0] = rightOperand;
                numberOfPentuples++;

                //perform real addition
                pentupleCode[numberOfPentuples].rule = SUB_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = convertHolder;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Real && rightOperand->type->type == Real) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //perform real addition
                pentupleCode[numberOfPentuples].rule = SUB_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } 

            root->dataPlace = holder->identifier;

            break;

        case arithOp_MUL:

            /* The * operation is valid between :-
            *  ->   int, int
            *  ->   int, real
            *  ->   real, int
            *  ->   real, real
            *  ->   record, record,  not yet handled
            *  both the children of the node will have dataPlace field, which will 
            *  contain the symbolTableEntry to the temporary holding the expression's
            *  calculated value. Check for its type and work accordingly
            */

            //write ICG code calculating for both the children
            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            //the variable which hold the value of the expression below
            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
            //checking what type of operand pair
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {

                holder = getNewTemporary(currentSymbolTable,Integer);
                pentupleCode[numberOfPentuples].rule = MUL_I;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Integer && rightOperand->type->type == Real) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //need to convert integer operand to real in a temporary
                SymbolTableEntry* convertHolder = getNewTemporary(currentSymbolTable,Real);
                pentupleCode[numberOfPentuples].rule = CONVERT_TO_REAL;
                pentupleCode[numberOfPentuples].result = convertHolder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                numberOfPentuples++;

                //perform real addition
                pentupleCode[numberOfPentuples].rule = MUL_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = convertHolder;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Real && rightOperand->type->type == Integer) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //need to convert integer operand to real in a temporary
                SymbolTableEntry* convertHolder = getNewTemporary(currentSymbolTable,Real);
                pentupleCode[numberOfPentuples].rule = CONVERT_TO_REAL;
                pentupleCode[numberOfPentuples].result = convertHolder;
                pentupleCode[numberOfPentuples].argument[0] = rightOperand;
                numberOfPentuples++;

                //perform real addition
                pentupleCode[numberOfPentuples].rule = MUL_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = convertHolder;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Real && rightOperand->type->type == Real) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //perform real addition
                pentupleCode[numberOfPentuples].rule = MUL_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } 

            root->dataPlace = holder->identifier; 

            break;

        case arithOp_DIV:

            /* The - operation is valid between :-
            *  ->   int, int
            *  ->   int, real
            *  ->   real, int
            *  ->   real, real
            *  both the children of the node will have dataPlace field, which will 
            *  contain the symbolTableEntry to the temporary holding the expression's
            *  calculated value. Check for its type and work accordingly
            */

            //write ICG code calculating for both the children
            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            
            //the variable which hold the value of the expression below
            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);

            
            //checking what type of operand pair
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {

                holder = getNewTemporary(currentSymbolTable,Integer);
                pentupleCode[numberOfPentuples].rule = DIV_I;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Integer && rightOperand->type->type == Real) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //need to convert integer operand to real in a temporary
                SymbolTableEntry* convertHolder = getNewTemporary(currentSymbolTable,Real);
                pentupleCode[numberOfPentuples].rule = CONVERT_TO_REAL;
                pentupleCode[numberOfPentuples].result = convertHolder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                numberOfPentuples++;

                //perform real addition
                pentupleCode[numberOfPentuples].rule = DIV_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = convertHolder;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Real && rightOperand->type->type == Integer) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //need to convert integer operand to real in a temporary
                SymbolTableEntry* convertHolder = getNewTemporary(currentSymbolTable,Real);
                pentupleCode[numberOfPentuples].rule = CONVERT_TO_REAL;
                pentupleCode[numberOfPentuples].result = convertHolder;
                pentupleCode[numberOfPentuples].argument[0] = rightOperand;
                numberOfPentuples++;

                //perform real addition
                pentupleCode[numberOfPentuples].rule = DIV_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = convertHolder;
                numberOfPentuples++;

            } else if (leftOperand->type->type == Real && rightOperand->type->type == Real) {
                
                holder = getNewTemporary(currentSymbolTable,Real);

                //perform real addition
                pentupleCode[numberOfPentuples].rule = DIV_R;
                pentupleCode[numberOfPentuples].result = holder;
                pentupleCode[numberOfPentuples].argument[0] = leftOperand;
                pentupleCode[numberOfPentuples].argument[1] = rightOperand;
                numberOfPentuples++;

            } 

            root->dataPlace = holder->identifier;

            break;

        case SingleOrRecIdLinkedListNode:

            ;
            SymbolTableEntry* aliasTemp = createRecordItemAlias(root,currentSymbolTable,globalSymbolTable);
            SymbolTableEntry* realReplace = getNewTemporary(currentSymbolTable,aliasTemp->type->type);
            
            if(aliasTemp->type->type == Integer) {
                
                pentupleCode[numberOfPentuples].rule = ASSIGN_OP_INT;

            } else if(aliasTemp->type->type == Real) {

                pentupleCode[numberOfPentuples].rule = ASSIGN_OP_REAL;

            }
            
            pentupleCode[numberOfPentuples].result = realReplace;
            pentupleCode[numberOfPentuples].argument[0] = aliasTemp;
            numberOfPentuples++;

            root->dataPlace = realReplace->identifier;

            break;

        case Num:

            holder = getNewTemporary(currentSymbolTable,Integer);

            // printf("returned from temporary.\n");

            pentupleCode[numberOfPentuples].rule = ASSIGN_IMMEDIATE_INT;
            pentupleCode[numberOfPentuples].result = holder;
            pentupleCode[numberOfPentuples].immVal = root->entry;
            numberOfPentuples++;

            // printf("why\n");

            root->dataPlace = holder->identifier;

            // printf("parsing num correctly\n");
            break;

        case RealNum:

            holder = getNewTemporary(currentSymbolTable,Real);

            pentupleCode[numberOfPentuples].rule = ASSIGN_IMMEDIATE_REAL;
            pentupleCode[numberOfPentuples].result = holder;
            pentupleCode[numberOfPentuples].immVal = root->entry;
            numberOfPentuples++;

            root->dataPlace = holder->identifier;

            break;        

        case logOp_NOT:

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            pentupleCode[numberOfPentuples].rule = BOOLEAN_NOT;
            pentupleCode[numberOfPentuples].result = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            numberOfPentuples++;

            root->dataPlace = root->children[0]->dataPlace;

            break;

        case logOp_AND:

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            boolResult = getNewTemporary(currentSymbolTable,Boolean);

            pentupleCode[numberOfPentuples].rule = BOOLEAN_AND;
            pentupleCode[numberOfPentuples].result = boolResult;
            pentupleCode[numberOfPentuples].argument[0] = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            pentupleCode[numberOfPentuples].argument[1] = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);

            root->dataPlace = boolResult->identifier;

            break;

        case logOp_OR:

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            boolResult = getNewTemporary(currentSymbolTable,Boolean);

            pentupleCode[numberOfPentuples].rule = BOOLEAN_OR;
            pentupleCode[numberOfPentuples].result = boolResult;
            pentupleCode[numberOfPentuples].argument[0] = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            pentupleCode[numberOfPentuples].argument[1] = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);

            root->dataPlace = boolResult->identifier;

            break;

        case relOp_EQ:

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            boolResult = getNewTemporary(currentSymbolTable,Boolean);

            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {
                
                pentupleCode[numberOfPentuples].rule = REL_EQ_INT;

            } else {

                pentupleCode[numberOfPentuples].rule = REL_EQ_REAL;

            }

            pentupleCode[numberOfPentuples].result = boolResult;
            pentupleCode[numberOfPentuples].argument[0] = leftOperand;
            pentupleCode[numberOfPentuples].argument[1] = rightOperand;
            numberOfPentuples++;

            root->dataPlace = boolResult->identifier;

            break;

        case relOp_GE:

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            boolResult = getNewTemporary(currentSymbolTable,Boolean);

            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {
                
                pentupleCode[numberOfPentuples].rule = REL_GEQ_INT;

            } else {

                pentupleCode[numberOfPentuples].rule = REL_GEQ_REAL;

            }

            pentupleCode[numberOfPentuples].result = boolResult;
            pentupleCode[numberOfPentuples].argument[0] = leftOperand;
            pentupleCode[numberOfPentuples].argument[1] = rightOperand;
            numberOfPentuples++;

            root->dataPlace = boolResult->identifier;

            break;

        case relOp_GT:

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            boolResult = getNewTemporary(currentSymbolTable,Boolean);

            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {
                
                pentupleCode[numberOfPentuples].rule = REL_GT_INT;

            } else {

                pentupleCode[numberOfPentuples].rule = REL_GT_REAL;

            }

            pentupleCode[numberOfPentuples].result = boolResult;
            pentupleCode[numberOfPentuples].argument[0] = leftOperand;
            pentupleCode[numberOfPentuples].argument[1] = rightOperand;
            numberOfPentuples++;

            root->dataPlace = boolResult->identifier;

            break;

        case relOp_LE:

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            boolResult = getNewTemporary(currentSymbolTable,Boolean);

            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {
                
                pentupleCode[numberOfPentuples].rule = REL_LEQ_INT;

            } else {

                pentupleCode[numberOfPentuples].rule = REL_LEQ_REAL;

            }

            pentupleCode[numberOfPentuples].result = boolResult;
            pentupleCode[numberOfPentuples].argument[0] = leftOperand;
            pentupleCode[numberOfPentuples].argument[1] = rightOperand;
            numberOfPentuples++;

            root->dataPlace = boolResult->identifier;

            break;

        case relOp_LT:

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            boolResult = getNewTemporary(currentSymbolTable,Boolean);

            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {
                
                pentupleCode[numberOfPentuples].rule = REL_LT_INT;

            } else {

                pentupleCode[numberOfPentuples].rule = REL_LT_REAL;

            }

            pentupleCode[numberOfPentuples].result = boolResult;
            pentupleCode[numberOfPentuples].argument[0] = leftOperand;
            pentupleCode[numberOfPentuples].argument[1] = rightOperand;
            numberOfPentuples++;

            root->dataPlace = boolResult->identifier;

            break;

        case relOp_NE:

            parseICGcode(root->children[0],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);
            parseICGcode(root->children[1],currentSymbolTable,globalSymbolTable,areInputParams,functionCalledSte);

            boolResult = getNewTemporary(currentSymbolTable,Boolean);

            leftOperand = findVariable(root->children[0]->dataPlace,currentSymbolTable,globalSymbolTable);
            rightOperand = findVariable(root->children[1]->dataPlace,currentSymbolTable,globalSymbolTable);
            
            if(leftOperand->type->type == Integer && rightOperand->type->type == Integer) {
                
                pentupleCode[numberOfPentuples].rule = REL_NEQ_INT;

            } else {

                pentupleCode[numberOfPentuples].rule = REL_NEQ_REAL;

            }

            pentupleCode[numberOfPentuples].result = boolResult;
            pentupleCode[numberOfPentuples].argument[0] = leftOperand;
            pentupleCode[numberOfPentuples].argument[1] = rightOperand;
            numberOfPentuples++;

            root->dataPlace = boolResult->identifier;

            break;
        
        default: 
            //do nothing for the astTags not mentioned
            break;

    }
    return 1;
}


int generateCompleteICGcode(astNode* root, SymbolTable* globalSymbolTable) {

    tempVariableNumber = 0;
    numberOfPentuples = 0;
    tempLabelNumber = 0;

    for(int i = 0; i < MAX_PENTUPLES_POSSIBLE; i++) {
        pentupleCode[i].rule = -1;
        pentupleCode[i].result = NULL;
        pentupleCode[i].argument[0] = NULL;
        pentupleCode[i].argument[1] = NULL;
        pentupleCode[i].jumpLabel = NULL;
        pentupleCode[i].immVal.lexeme = NULL;
        pentupleCode[i].immVal.linenumber = 0;
        pentupleCode[i].immVal.type = -1;
    }

    parseICGcode(root,globalSymbolTable,globalSymbolTable,FALSE,NULL);

    return 1;
}