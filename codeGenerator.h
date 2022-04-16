#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdio.h>
#include "symbolTable.h"


void generateAssemblyCode(FILE *fp, SymbolTable* globalSymbolTable);

#endif