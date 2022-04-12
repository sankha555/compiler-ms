#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "globalDef.h"
#include "symbolTableDef.h"
#include "symbolTable.h"
#include "astDef.h"
#include "typing.h"

void generateIntermediateCode(astNode *root, SymbolTable* globalSymbolTable, SymbolTable* currentSymbolTable)
{
    if (root == NULL)
        return;

    SymbolTableEntry* lookupResult;
    astNode* temp;

    switch (root->type)
    {
        case Program:
            generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable);
            generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable);

            // just concatenate the code of the 2 children
            root->code = (char*)malloc(strlen(root->children[0]->code) + strlen(root->children[1]->code) + 1);
            // the main function is in the beginning
            strcat(root->code, root->children[1]->code);
            // the rest of the functions are after the main function
            strcpy(root->code, root->children[0]->code);
            break;
        case MainFunc:
            // lookup the symbol table of the main function
            lookupResult = lookupSymbolTable(globalSymbolTable, "_main");
            generateIntermediateCode(root->children[0], globalSymbolTable, lookupResult->tablePointer);

            // just concatenate the code of the child with "_start" label
            root->code = (char*)malloc(strlen(root->children[0]->code) + strlen("_main PROC NEAR\n\tpusha\n") + strlen("\tpopa\n\tret\n_main ENDP\n") + 1);
            strcat(root->code, "_main PROC NEAR\n\tpusha\n");
            strcat(root->code, root->children[0]->code);
            strcat(root->code, "\tpopa\n\tret\n_main ENDP\n");
            break;
        case FuncLinkedListNode:
            temp = root;
            // for each function
            while (temp != NULL)
            {
                // lookup the symbol table of the function
                lookupResult = lookupSymbolTable(globalSymbolTable, temp->data->children[0]->entry.lexeme);
                generateIntermediateCode(temp->data->children[1], globalSymbolTable, lookupResult->tablePointer);

                // append the code of the child to root
                root->code = (char*)malloc(strlen(root->code) + strlen(temp->data->children[1]->code) + 1);
                strcat(root->code, temp->data->children[1]->code);

                temp = temp->next;
            }
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
            generateIntermediateCode(root->children[3], globalSymbolTable, currentSymbolTable);

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
            generateIntermediateCode(root->children[2], globalSymbolTable, currentSymbolTable);

            // <returnStmt>
            // TODO: either generate code for return statement here or above (here makes more sense in case
            //                  all the outputParamters are not used)
            generateIntermediateCode(root->children[3], globalSymbolTable, currentSymbolTable);

            // for now, just return otherStmts.code
            root->code = root->children[2]->code;
            break;
        case StmtLinkedListNode:
            temp = root;
            // for each statement
            while (temp != NULL)
            {
                // parse the statement
                generateIntermediateCode(temp->data, globalSymbolTable, currentSymbolTable);

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
            generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable);
            generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable);

            // ask for a temporary data place
            char* tempDataPlace = getTempDataPlace();

            sprintf(root->code, "MOV AX, %s\nMOV BX, %s\nAND AX, BX\n STOREW %s, AX\n", root->children[0]->dataPlace, root->children[1]->data, tempDataPlace);
            break;
        case arithOp_PLUS:
            // create a new temp data variables
            // check the type of children, real real, int int, real int -> real, int real -> real            

            char* tempDataPlace = getTempDataPlace();
            
            generateIntermediateCode(root->children[0], globalSymbolTable, currentSymbolTable);
            generateIntermediateCode(root->children[1], globalSymbolTable, currentSymbolTable);
            
            int firstType = lookupSymbolTable(currentSymbolTable, root->children[0]->dataPlace)->type;
            int secondType = lookupSymbolTable(currentSymbolTable, root->children[1]->dataPlace)->type;

            if(firstType == secondType == INT) {
                sprintf(root->code, "LOADI A\n LOADI B\n ADDI X,A,B\n STOREI X");
            } else if (firstType == secondType == REAL) {
                sprintf(root->code, "LOADR A\n LOADR B\n ADDR X,A,B\n STORER X");
            } else if (firstType != secondType && firstType == INT && secondType == REAL){
                
            } else {

            }





    }
}

int codegen(astNode *root, FILE *fp)
{
    SymbolTable* globalSymbolTable = init_symbolTable(root);

    generateIntermediateCode(root, globalSymbolTable, NULL);

    char *dataSegment = flattenSymbolTable(globalSymbolTable);

    fprintf(fp, ".code %s", root->code);
    return 0;
}