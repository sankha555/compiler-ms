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

void transactInMemory(int fromMemory, int width, SymbolTableEntry* ptr, SymbolTable* globalSymbolTable) {

    

}

void generateAssemblyCode(FILE* fp, SymbolTable* globalSymbolTable) {

    if(fp == NULL) {

        printf("ERROR: Invalid file pointer passed.\n");
        return;

    }

    for(int i = 0; i < numberOfPentuples; i++) {

        switch(pentupleCode[i].rule) {

            case DEFINE_CS:

                fprintf(fp,"; ----------------------------------------------------------------------------------------\n; Auto generated assembly code from intermediate quadruple code.\n; Runs on 64-bit Linux only, with NASM version 2.15.05.\n; To assemble and run:\n;\n;     nasm -felf64 hello.asm && ld hello.o && ./a.out\n; ----------------------------------------------------------------------------------------\n\n\tglobal\t_start\n\n\tsection\t.text\n");
                break;

            case DEFINE_DS:

                //traverse the global symbol table for all functions and add their functional offset labels
                fprintf(fp,"\n\n\tsection\t.data\n");

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

                //check if location to be copied from is a global variable
                if(pentupleCode[i].argument[0]->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                }

                //step 1: from the copying location, move 2 bytes into ebx

                fprintf(fp,"\tmov\tbx, [ebp+%d]\n",pentupleCode[i].argument[0]->offset);

                if(pentupleCode[i].argument[0]->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpop\tebp\n");
                }
                
                //check if location to be copied to is a global variable
                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                }

                //step 2: from the bx register, move 2 bytes into destination

                fprintf(fp,"\tmov\t[ebp+%d], bx\n",pentupleCode[i].result->offset);
            
                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpop\tebp\n");
                }
                
                break;

            case ASSIGN_OP_REAL:

                //copy from one memory location, four bytes to another
                
                fprintf(fp,";-------Assign real operation-------\n");

                //check if location to be copied from is a global variable
                if(pentupleCode[i].argument[0]->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                }

                //step 1: from the copying location, move 2 bytes into ebx

                fprintf(fp,"\tmov\tebx, [ebp+%d]\n",pentupleCode[i].argument[0]->offset);

                if(pentupleCode[i].argument[0]->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpop\tebp\n");
                }
                
                //check if location to be copied to is a global variable
                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                }

                //step 2: from the bx register, move 2 bytes into destination

                fprintf(fp,"\tmov\t[ebp+%d], ebx\n",pentupleCode[i].result->offset);
            
                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpop\tebp\n");
                }

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

                    if(pentupleCode[i].argument[j]->parentTable == globalSymbolTable) {
                        fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                    }

                    //transfer in bx and accumulate in ax
                    fprintf(fp,"\tmov\tbx, [ebp+%d]\n",pentupleCode[i].argument[j]->offset);
                    fprintf(fp,"\tadd\tax, bx\n");

                    if(pentupleCode[i].argument[j]->parentTable == globalSymbolTable) {
                        fprintf(fp,"\tpop\tebp\n");
                    }
                }
                
                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                }

                fprintf(fp,"\tmov\tbx, ax\n\tmov\t[ebp+%d], bx\n",pentupleCode[i].result->offset);

                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpop\tebp\n");
                }

                break;


            default: break;
        }

    }

}