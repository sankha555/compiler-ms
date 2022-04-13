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

int parseICGcode(astNode* root) {

    switch(root->type) {
        
        case Program: 
            
            pentupleCode[numberOfPentuples].rule = DEFINE_CS;
            numberOfPentuples++;

            parseICGcode(root->children[0]);
            parseICGcode(root->children[1]);

            pentupleCode[numberOfPentuples].rule = DEFINE_DS;
            numberOfPentuples++;

            pentupleCode[numberOfPentuples].rule = EXIT_CODE;
            numberOfPentuples++;

            break;

    }

}


int generateCompleteICGcode(astNode* root) {

    tempVariableNumber = 0;
    numberOfPentuples = 0;

    for(int i = 0; i < MAX_PENTUPLES_POSSIBLE; i++) {
        pentupleCode[i].label = NULL;
        pentupleCode[i].rule = -1;
        pentupleCode[i].result = NULL;
        pentupleCode[i].argument[0] = NULL;
        pentupleCode[i].argument[1] = NULL;
    }

    return parseICGcode(root);

}