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

                fprintf(fp,"; ----------------------------------------------------------------------------------------\n; Auto generated assembly code from intermediate quadruple code.\n; Runs on 64-bit Linux only, with NASM version 2.15.05.\n; To assemble and run:\n;\n;     nasm -felf64 code.asm && gcc code.o && ./a.out\n; ----------------------------------------------------------------------------------------\n\n");
                fprintf(fp,"global\tmain\n");
                fprintf(fp,"extern printf\n");
                fprintf(fp,"extern scanf\n");
                fprintf(fp,"section\t.text\n");
                break;

            case DEFINE_DS:

                //traverse the global symbol table for all functions and add their functional offset labels
                fprintf(fp,"\n\nsection\t.data\n");

                // add prinf and scanf formats for ints and reals
                fprintf(fp,"\tformat_print_int: db \"%%d\", 10, 0\t; newline, nul terminator\n");
                fprintf(fp,"\tformat_print_real: db \"%%f\", 10, 0\t; newline, nul terminator\n");
                fprintf(fp,"\tformat_scan_int: db \"%%d\", 0\n");
                fprintf(fp,"\tformat_scan_real: db \"%%f\", 0\n");

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

                fprintf(fp,"\n\nmain:\n\tmov\tEBP, %s\n",generateFuncMemOffset(pentupleCode[i].result->identifier));
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
                fprintf(fp,"\tmov\tbx, %s\n",pentupleCode[i].immVal.lexeme);
                fprintf(fp,"\tmov\t[ebp+%d], bx\n",pentupleCode[i].result->offset);

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

            case ADD_R:
                /**
                 * @brief To add real variables
                 * 
                 *  finit                        ;reset fpu stacks to default
                    fld    dword [single_value1] ;push single_value1 to fpu stack
                    fld    dword [single_value2] ;push single_value2 to fpu stack
                    fadd                         ;st0 := st1 + st0
                    fstp   dword [single_sum]    ;store the summation result into mem
                 * 
                 */
                fprintf(fp,";-------Add Real variables--------\n");

                fprintf(fp,"\tfinit\t\t\t;reset fpu stacks to default\n");

                for(int j = 0; j < 2; j++) {

                    if(pentupleCode[i].argument[j]->parentTable == globalSymbolTable) {
                        fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                    }

                    // push the value to the fpu stack
                    fprintf(fp,"\tfld\tdword [ebp+%d]\n",pentupleCode[i].argument[j]->offset);

                    if(pentupleCode[i].argument[j]->parentTable == globalSymbolTable) {
                        fprintf(fp,"\tpop\tebp\n");
                    }
                }

                // perform the addition operation
                fprintf(fp,"\tfadd\t\t\t;st0 := st1 + st0\n");
                
                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
                }

                // store the result in the memory location
                fprintf(fp,"\tfstp\tdword [ebp+%d]\n",pentupleCode[i].result->offset);

                if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                    fprintf(fp,"\tpop\tebp\n");
                }

                break;
            case READ:
            /**
             * @brief Use scanf with appropriate format specifier depending on whether to read int or real
             * 
             */
            fprintf(fp,";-------Read-------\n");

            case WRITE_VAR:
            /**
             * @brief Use printf with appropriate format specifier depending on whether to write int or real
             * 
             */
            fprintf(fp,";-------Write Variable-------\n");
            
            // push the variable to print on to the stack
            if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                fprintf(fp,"\tpush\tebp\n\tmov\tebp, %d\n",globalSymbolTable->currentOffset);
            }
            
            fprintf(fp,"\tmov\tbx, [ebp+%d]\n",pentupleCode[i].result->offset);

            if(pentupleCode[i].result->parentTable == globalSymbolTable) {
                fprintf(fp,"\tpop\tebp\n");
            }

            // push the output format specifier to the stack
            if (pentupleCode[i].result->type == Integer)
                fprintf(fp,"\tpush\tformat_print_int\n");
            else
                fprintf(fp,"\tpush\tformat_print_real\n");
            
            fprintf(fp,"\tcall\tprintf\n");

            default: break;
        }

    }

}