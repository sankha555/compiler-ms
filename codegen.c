/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "globalDef.h"
#include "symbolTableDef.h"
#include "symbolTable.h"
#include "astDef.h"
#include "typing.h"
#include <string.h>

int tempVarNumber = 0;

char tempDataPlace[30];

SymbolTableEntry* getTempDataPlace(SymbolTable* currentSymbolTable, TypeArrayElement* t, char* tempVarName) {
    
    sprintf(tempVarName,"tempVariableName_%d",tempVarNumber);

    SymbolTableEntry* temp = createNewSymbolTableEntry(tempVarName,FALSE,NULL,t->type);

    if(temp == NULL) {
        printf("ERROR!! could not create temporaries.\n");
        return NULL;
    }

    insertintoSymbolTable(currentSymbolTable,temp);

    return temp;

}

void generateIntermediateCode(astNode *root, SymbolTable* globalSymbolTable, SymbolTable* currentSymbolTable, FILE* fp)
{
        
    if (root == NULL)
        return;

    SymbolTableEntry* temporaryCreated;
    SymbolTableEntry* lookupResult;
    astNode* temp;

    TypeArrayElement* IntTypeElement = lookupTypeTable(globalTypeTable,"int");
    TypeArrayElement* RealTypeElement = lookupTypeTable(globalTypeTable,"real");

    switch (root->type)
    {
        case Program:
            
            // resetting the temp variable index
            tempVarNumber = 0;
            
            generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable, fp);
            generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable, fp);

            char startCode[] = "; -----------------------------------------------------------------------------\n; A 64-bit Linux application that writes the first 90 Fibonacci numbers. To\n; assemble and run:\n;\n;     nasm -felf64 fib.asm && gcc fib.o && ./a.out\n; -----------------------------------------------------------------------------";
            char sectionStartCode[] = "\tglobal\t_start:\n\tsection\t.text\n";

            root->code = (char*)malloc(strlen(startCode)+
                                       strlen(sectionStartCode)+
                                       strlen(root->children[0]->code)+
                                       strlen(root->children[1]->code)+
                                       1);
            
            sprintf(root->code,"%s%s%s%s",startCode,sectionStartCode,root->children[0]->code,root->children[1]->code);

            break;

        case MainFunc:
            
            // lookup the symbol table of the main function
            lookupResult = lookupSymbolTable(globalSymbolTable, "_main");
            generateIntermediateCode(root->children[0], globalSymbolTable, lookupResult->tablePointer,fp);
            
            char mainStart[] = "_start:\n\n";
            char mainEnd[] = "\tmov\trax, 60\n\txor\trdi, rdi\n\tsyscall\n\n";

            root->code = (char*)malloc(strlen(mainStart)+
                                       strlen(root->children[0]->code)+
                                       strlen(mainEnd)+
                                       1);
            
            sprintf(root->code,"%s%s%s",mainStart,root->children[0]->code,mainEnd);
            
            break;

        case FuncLinkedListNode:
            
            generateIntermediateCode(root->data,globalSymbolTable,currentSymbolTable,fp);

            if(root->next != NULL) {
                generateIntermediateCode(root->next,globalSymbolTable, currentSymbolTable,fp);
            }

            root->code = (char*)malloc(strlen(root->data->code)+
                                       strlen(root->next->code)+
                                       2);
            
            sprintf(root->code,"%s\n%s",root->data->code,root->next->code);

            break;

        case FuncDef:
            // Store the values in input registers to the corresponding variables
            

            // Parse the input parameters linked list and store them in the corresponding variables
            astNode *input_params_ll = root->children[1];
            char *input_params_code = (char*)malloc(1);

            int num_input_params = 0;

            while (input_params_ll != NULL)
            {
                astNode *input_param = input_params_ll->data;

                // lookup the function's symbol table for the parameter, and store register value in the variable
                lookupResult = lookupSymbolTable(currentSymbolTable, input_param->children[1]->entry.lexeme);

                // store register value in the variable using offset from lookup result
                input_params_code = (char*)malloc(strlen(input_params_code) + strlen("\tmov [ebp-") + strlen(input_param->children[1]->entry.lexeme) + strlen("], Ri\n") + 1);
                sprintf(input_params_code, "%s\tmov [ebp+%d], R%d\n", input_params_code, lookupResult->offset, num_input_params);

                num_input_params++;
                input_params_ll = input_params_ll->next;
            }

            // Parse the output parameters linked list and store them in the corresponding variables
            astNode *output_params_ll = root->children[2];
            char *output_params_code = (char*)malloc(1);

            int num_output_params = 0;

            while (output_params_ll != NULL)
            {
                astNode *output_param = output_params_ll->data;

                // lookup the function's symbol table for the parameter, and store register value in the variable
                lookupResult = lookupSymbolTable(currentSymbolTable, output_param->children[1]->entry.lexeme);

                // store register value in the variable using offset from lookup result
                output_params_code = (char*)malloc(strlen(output_params_code) + strlen("\tmov Ri, [ebp-") + strlen(output_param->children[1]->entry.lexeme) + strlen("]\n") + 1);
                sprintf(output_params_code, "%s\tmov R%d, [ebp+%d]\n", output_params_code, num_output_params, lookupResult->offset);

                num_output_params++;
                output_params_ll = output_params_ll->next;
            }

            // parse the statements in body
            generateIntermediateCode(root->children[3], globalSymbolTable, currentSymbolTable,fp);

            /**
             * @brief writing an x86 assembly implementation of this function
             * 
             * The name of the function is stored in root->children[0]->entry.lexeme
             * The input parameters are stored in root->children[1].code
             * The body of the function is stored in root->children[3].code
             * The output parameters (return values) are stored in root->children[2].code
             * 
             * Store this in root->code
             */

            // function name
            char* funcName = root->children[0]->entry.lexeme;

            // body
            char* body = root->children[3]->code;

            // FIXME: output parameters
            // FIXME: should popa be before output_params_code?

            // procedure definition
            char* procDef = (char*)malloc(strlen(funcName) + strlen(" PROC NEAR\n\tpusha\n") + strlen(input_params_code) + strlen(body) + strlen(output_params_code) + strlen("\tpopa\n\tret\n") + strlen(funcName) + strlen(" ENDP\n") + 1);
            sprintf(procDef, "%s PROC NEAR\n\tpusha\n%s%s%s\tpopa\n\tret\n%s ENDP\n", funcName, input_params_code, body, output_params_code, funcName);
            

            
            break;

        case Stmts:
            // <typeDefinitions> - handled by symbol table
            // generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable);

            // <declarations> - handled by symbol table and put in data segment
            // generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable);

            // <otherStmts>
            generateIntermediateCode(root->children[2], globalSymbolTable, currentSymbolTable,fp);

            // <returnStmt>
            // TODO: either generate code for return statement here or above (here makes more sense in case
            //                  all the outputParamters are not used)
            generateIntermediateCode(root->children[3], globalSymbolTable, currentSymbolTable,fp);

            // for now, just return otherStmts.code
            root->code = root->children[2]->code;
            break;
        case StmtLinkedListNode:
            temp = root;
            // for each statement
            while (temp != NULL)
            {
                // parse the statement
                generateIntermediateCode(temp->data, globalSymbolTable, currentSymbolTable,fp);

                // append the code of the child to root
                root->code = (char*)malloc(strlen(root->code) + strlen(temp->data->code) + 1);
                strcat(root->code, temp->data->code);

                temp = temp->next;
            }
            break;
        case AssignmentOperation:
            // Only handle 
            break;
        case logOp_AND:
            generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable,fp);
            generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable,fp);

            // ask for a temporary data place
            temporaryCreated = getTempDataPlace(currentSymbolTable,IntTypeElement,tempDataPlace);

            sprintf(root->code, "MOV AX, %s\nMOV BX, %s\nAND AX, BX\n STOREW %s, AX\n", root->children[0]->dataPlace, root->children[1]->data, tempDataPlace);
            break;
        case arithOp_PLUS:
            // create a new temp data variables
            // check the type of children, real real, int int, real int -> real, int real -> real            
        
            generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable,fp);
            generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable,fp);
            
            int firstType = lookupSymbolTable(currentSymbolTable, root->children[0]->dataPlace)->type;
            int secondType = lookupSymbolTable(currentSymbolTable, root->children[1]->dataPlace)->type;

            if(firstType == secondType == Integer) {

                temporaryCreated = getTempDataPlace(currentSymbolTable,IntTypeElement,tempDataPlace);
                sprintf(root->code, "LOADI A\n LOADI B\n ADDI X,A,B\n STOREI X");

            } else if (firstType == secondType == Real) {

                temporaryCreated = getTempDataPlace(currentSymbolTable,RealTypeElement,tempDataPlace);
                sprintf(root->code, "LOADR A\n LOADR B\n ADDR X,A,B\n STORER X");

            } else if (firstType != secondType && firstType == Integer && secondType == Real){
                
                temporaryCreated = getTempDataPlace(currentSymbolTable,RealTypeElement,tempDataPlace);

            } else {
                
                temporaryCreated = getTempDataPlace(currentSymbolTable,RealTypeElement,tempDataPlace);

            }

            break;
        case arithOp_MINUS:

            // create a new temp data variables
            // check the type of children, real real, int int, real int -> real, int real -> real            

            getTempDataPlace(currentSymbolTable,NULL,tempDataPlace);
            
            generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable,fp);
            generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable,fp);
            
            int firstType = lookupSymbolTable(currentSymbolTable, root->children[0]->dataPlace)->type;
            int secondType = lookupSymbolTable(currentSymbolTable, root->children[1]->dataPlace)->type;

            if(firstType == secondType == Integer) {
                sprintf(root->code, "LOADI A\n LOADI B\n SUBI X,A,B\n STOREI X");
            } else if (firstType == secondType == Real) {
                sprintf(root->code, "LOADR A\n LOADR B\n SUBR X,A,B\n STORER X");
            } else if (firstType != secondType && firstType == Integer && secondType == Real){
                
            } else {

            }


            break;
        case arithOp_DIV:

            // create a new temp data variables
            // check the type of children, real real, int int, real int -> real, int real -> real            

            getTempDataPlace(currentSymbolTable,NULL,tempDataPlace);
            
            generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable,fp);
            generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable,fp);
            
            int firstType = lookupSymbolTable(currentSymbolTable, root->children[0]->dataPlace)->type;
            int secondType = lookupSymbolTable(currentSymbolTable, root->children[1]->dataPlace)->type;

            if(firstType == secondType == Integer) {
                sprintf(root->code, "LOADI A\n LOADI B\n DIVI X,A,B\n STOREI X");
            } else if (firstType == secondType == Real) {
                sprintf(root->code, "LOADR A\n LOADR B\n DIVR X,A,B\n STORER X");
            } else if (firstType != secondType && firstType == Integer && secondType == Real){
                
            } else {

            }

            break;
        case arithOp_MUL:
            
            // create a new temp data variables
            // check the type of children, real real, int int, real int -> real, int real -> real            

            getTempDataPlace(currentSymbolTable,NULL,tempDataPlace);
            
            generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable,fp);
            generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable,fp);
            
            int firstType = lookupSymbolTable(currentSymbolTable, root->children[0]->dataPlace)->type;
            int secondType = lookupSymbolTable(currentSymbolTable, root->children[1]->dataPlace)->type;

            if(firstType == secondType == Integer) {
                sprintf(root->code, "LOADI A\n LOADI B\n DIVI X,A,B\n STOREI X");
            } else if (firstType == secondType == Real) {
                sprintf(root->code, "LOADR A\n LOADR B\n DIVR X,A,B\n STORER X");
            } else if (firstType != secondType && firstType == Integer && secondType == Real){
                
            } else {

            }

            break;
    }
}

int codegen(astNode *root, FILE *fp)
{
    SymbolTable* globalSymbolTable = init_symbolTable(root);

    generateIntermediateCode(root, globalSymbolTable, NULL, fp);

    char *dataSegment = flattenSymbolTable(globalSymbolTable);

    // fprintf(fp, ".code %s", root->code);
    return 0;
}