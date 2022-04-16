/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#include <stdio.h>
#include <string.h>
#include "globalDef.h"
#include "astDef.h"
#include "astGenerator.h"
#include "codeGenerator.h"
#include "codeGeneratorDef.h"
#include "symbolTable.h"
#include "symbolTableDef.h"
#include "typing.h"
#include "icgGenerator.h"
#include "icgGeneratorDef.h"
#include "lexerDef.h"

char* generateFuncMemOffset(char* identifier) {
    char* result = (char*)malloc(45);
    sprintf(result,"%s_memOffset",identifier);
    return result;
}

void transactInMemory(boolean fromMemory, int width, SymbolTableEntry* ptr, SymbolTable* globalSymbolTable, FILE* fp) {

    char bp[5];

    if(width == 1) {
        sprintf(bp,"bl");
    } else if (width == 2) {
        sprintf(bp,"bx");
    } else if (width == 4) {
        sprintf(bp,"ebx");
    } else {
        sprintf(bp,"rbx");
    }

    if(ptr->parentTable == globalSymbolTable) {
        fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
    }

    if(fromMemory == TRUE) {

        fprintf(fp,"\tmov\t%s, [ebp+%d]\n",bp,ptr->offset);

    } else {

        fprintf(fp,"\tmov\t[ebp+%d], %s\n",ptr->offset,bp); 

    }

    if(ptr->parentTable == globalSymbolTable) {
        fprintf(fp,"\tpop\tebp\n");
    }

    return;

}

void generateAssemblyCode(FILE* fp, SymbolTable* globalSymbolTable) {

    if(fp == NULL) {

        printf("ERROR: Invalid file pointer passed.\n");
        return;

    }

    for(int i = 0; i < numberOfPentuples; i++) {

        switch(pentupleCode[i].rule) {

            case DEFINE_CS:

                fprintf(fp,"; ----------------------------------------------------------------------------------------\n; Auto generated assembly code from intermediate quadruple code.\n; Runs on 64-bit Linux only, with NASM version 2.15.05.\n; To assemble and run:\n;\n;     nasm -felf64 hello.asm && ld hello.o && ./a.out\n; ----------------------------------------------------------------------------------------\n\n");
                fprintf(fp,"\tglobal\t_start\n\n\textern\tprintf\n\textern\tscanf\tsection\t.text\n");
                break;

            case DEFINE_DS:

                //traverse the global symbol table for all functions and add their functional offset labels
                fprintf(fp,"\n\n\tsection\t.data\n");

                fprintf(fp,"readInt:\tdb\t\"Enter Integer Number: \", 0\n");
                fprintf(fp,"readReal:\tdb\t\"Enter Real Number: \", 0\n");
                fprintf(fp,"writeRes:\tdb\t\"The number is: \", 0\n");
                fprintf(fp,"formatInt:\tdb\t\"%%hi\", 0\n");
                fprintf(fp,"formatReal:\tdb\t\"%%f\", 0\n");
                fprintf(fp,"integer1:\ttimes 2 db 0\n");
                fprintf(fp,"float1:\ttimes 4 db 0\n");

                for(int i = 0; i < K_MAP_SIZE; i++) {
                    SymbolTableEntry* temp = globalSymbolTable->tableEntries[i];
                    while(temp != NULL) {
                        if(temp->isFunction == TRUE) {
                            char* labelName = generateFuncMemOffset(temp->identifier);
                            fprintf(fp,"%s:\tdb\t%d\n",labelName,temp->offset);
                        }
                        temp = temp->next;
                    }
                }

                break;

            case EXIT_CODE:

                fprintf(fp,"\tmov\trax, 60\t\t; system call for exit\n\txor\trdi, rdi\t; exit code 0\n\tsyscall\t\t\t; invoke operating system to exit\n");
                break;

            case FUNC_DEF:

                //function label in the code segment, push EBP into stack, and move the function offset from constant DS into EBP

                fprintf(fp,"\n\n%s:\tpush\tEBP\n",pentupleCode[i].result->identifier);
                fprintf(fp,"\tmov\tEBP, %s\n",generateFuncMemOffset(pentupleCode[i].result->identifier));
                break;

            case FUNC_DEF_END:
                
                // pop from stack into EBP, then return
                fprintf(fp,"\tpop\tEBP\n\tret\n");
                break;

            case FUNC_DEF_MAIN:

                fprintf(fp,"\n\n_start:\tmov\tEBP, %s\n",generateFuncMemOffset(pentupleCode[i].result->identifier));
                break;

            case ASSIGN_OP_INT:

                //copy from one memory location, two bytes to another
                
                fprintf(fp,";-------Assign integer operation-------\n");

                transactInMemory(TRUE,2,pentupleCode[i].argument[0],globalSymbolTable,fp);
                
                transactInMemory(FALSE,2,pentupleCode[i].result,globalSymbolTable,fp);
                
                break;

            case ASSIGN_OP_REAL:

                // //copy from one memory location, four bytes to another
                
                fprintf(fp,";-------Assign real operation-------\n");

                transactInMemory(TRUE,4,pentupleCode[i].argument[0],globalSymbolTable,fp);
                
                transactInMemory(FALSE,4,pentupleCode[i].result,globalSymbolTable,fp);
                
                break;

            case ASSIGN_IMMEDIATE_INT:

                fprintf(fp,";-------Assign immediate integer-------\n");

                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                }

                fprintf(fp,"\tmov\t[ebp+%d], %s\n",pentupleCode[i].result->offset,pentupleCode[i].immVal.lexeme);

                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpop\tebp\n");
                }


                break;

            case ASSIGN_IMMEDIATE_REAL:

                fprintf(fp,";-------Assign immediate realVal-------\n");

                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                }

                // perform the assignment operation for real immediate

                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpop\tebp\n");
                }

                break;

            case ADD_I:

                fprintf(fp,";-------Add Integer variables--------\n");

                fprintf(fp,"\txor\tax, ax\n");

                for(int j = 0; j < 2; j++) {

                    transactInMemory(TRUE,2,pentupleCode[i].argument[j],globalSymbolTable,fp);

                    fprintf(fp,"\tadd\tax, bx\n");
                }

                fprintf(fp,"\tmov\tbx, ax\n");
                
                transactInMemory(FALSE,2,pentupleCode[i].result,globalSymbolTable,fp);

                break;

            case ADD_R:

                break;

            case SUB_I:

                fprintf(fp,";-------Subtract Integer variables--------\n");

                fprintf(fp,"\txor\tax, ax\n");

                for(int j = 0; j < 2; j++) {

                    transactInMemory(TRUE,2,pentupleCode[i].argument[j],globalSymbolTable,fp);

                    if(j==0)
                        fprintf(fp,"\tadd\tax, bx\n");
                    else
                        fprintf(fp,"\tsub\tax, bx\n");
                }

                fprintf(fp,"\tmov\tbx, ax\n");
                
                transactInMemory(FALSE,2,pentupleCode[i].result,globalSymbolTable,fp);

                break;

            case SUB_R:

                break;

            case MUL_I:

                fprintf(fp,";-------Multiply Integer variables--------\n");

                fprintf(fp,"\txor\teax, eax\n");

                for(int j = 0; j < 2; j++) {

                    transactInMemory(TRUE,2,pentupleCode[i].argument[j],globalSymbolTable,fp);

                    if(j==0)
                        fprintf(fp,"\tadd\tax, bx\n");
                    else
                        fprintf(fp,"\tmul\tbx\n");
                }

                fprintf(fp,"\tmov\tbx, ax\n");
                
                transactInMemory(FALSE,2,pentupleCode[i].result,globalSymbolTable,fp);

                break;

            case MUL_R:

                break;
            
            case DIV_I:

                fprintf(fp,";-------Divide Integer variables--------\n");

                fprintf(fp,"\txor\teax, eax\n");

                for(int j = 0; j < 2; j++) {

                    transactInMemory(TRUE,2,pentupleCode[i].argument[j],globalSymbolTable,fp);

                    if(j==0)
                        fprintf(fp,"\tadd\tax, bx\n");
                    else
                        fprintf(fp,"\tcwd\n\tidiv\tbx\n");
                }

                fprintf(fp,"\tshr\teax, 16\n\tmov\tbx, ax\n");
                
                transactInMemory(FALSE,2,pentupleCode[i].result,globalSymbolTable,fp);

                break;

            case DIV_R:

                

                break;

            case READ:

                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                }

                //figure out the type of var you wish to read, int or real
                if(pentupleCode[i].result->type->type == Integer) {
                    
                    fprintf(fp,"\tpush\treadInt\n");
                    fprintf(fp,"\tcall\tprintf\n");
                    fprintf(fp,"\tadd\tesp, 4\n");

                    fprintf(fp,"\tpush\tinteger1\n");
                    fprintf(fp,"\tpush\tformatInt\n");
                    fprintf(fp,"\tcall\tscanf\n");
                    fprintf(fp,"\tadd\tesp, 8\n");

                    fprintf(fp,"\tmov\tbx, integer1\n");
                    fprintf(fp,"\tmov\t[ebp+%d], bx\n",pentupleCode[i].result->offset);

                } else if(pentupleCode[i].result->type->type == Real) {
                    
                    fprintf(fp,"\tpush\treadReal\n");
                    fprintf(fp,"\tcall\tprintf\n");
                    fprintf(fp,"\tadd\tesp, 4\n");

                    fprintf(fp,"\tpush\tfloat1\n");
                    fprintf(fp,"\tpush\tformatReal\n");
                    fprintf(fp,"\tcall\tscanf\n");
                    fprintf(fp,"\tadd\tesp, 8\n");
                    
                    fprintf(fp,"\tmov\tebx, float1\n");
                    fprintf(fp,"\tmov\t[ebp+%d], ebx\n",pentupleCode[i].result->offset);

                }

                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpop\tebp\n");
                }                

                break;

            case WRITE_IMMEDIATE:

                break;

            case WRITE_VAR:

                break;

            default: break;
        }

    }

}