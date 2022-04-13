#ifndef ICG_GENERATOR_H
#define ICG_GENERATOR_H

#include "globalDef.h"
#include "symbolTableDef.h"

typedef enum ICGrule {
    ADD_I,      // add two integers in memory
    ADD_R,      // add two reals in memory
    SUB_I,      // subtract two integers in memory
    SUB_R,      // subtract two reals in memory
    MUL_I,      // multiply two integers in memory
    MUL_R,      // multiply two reals in memory
    DIV_I,      // divide two integers in memory
    DIV_R,      // divide two reals in memory
    GOTO_L,     // unconditional jump to label
    IF_TRUE_GOTO_L, 
    IF_FALSE_GOTO_L,

} ICGrule;


typedef struct pentuple {
    char* label;
    ICGrule rule;
    SymbolTableEntry* result;
    SymbolTableEntry* argument[2];
} pentuple;


pentuple IntermediateCode [MAX_PENTUPLES_POSSIBLE];

#endif ICG_GENERATOR_H