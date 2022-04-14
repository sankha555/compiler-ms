#ifndef ICG_GENERATOR_DEF_H
#define ICG_GENERATOR_DEF_H

#include "globalDef.h"
#include "symbolTableDef.h"
#include "lexerDef.h"

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
    ASSIGN_OP_INT,  // copy from one memory location 2 bytes to another
    ASSIGN_OP_REAL, // copy from one mem 4 bytes to another mem
    DEFINE_DS,  // define data segment, in code generation, adds function offsets
    DEFINE_CS,  // define code segment, add "global _start:" in code generation
    EXIT_CODE,  // in the end of intermediate code generation, adds the exit syscall during code generation
    FUNC_DEF,   // function label in the code segment, push EBP into stack, and move the function offset from constant DS into EBP
    FUNC_DEF_END, // pop from stack into EBP, then return
    FUNC_DEF_MAIN,
    CONVERT_TO_REAL,
    SETUP_FUNC_CALL_PARAM_TRANSFER,
    PUSH_INPUT_VAR,
    PUSH_INPUT_IMMEDIATE,
    CALL_FUNC,
    POP_OUTPUT,
} ICGrule;

typedef struct immediateOrSTE {
    SymbolTableEntry* ste;
    token immediate;
    boolean isSTE;
} immediateOrSTE;

typedef struct pentuple {
    char* label;
    ICGrule rule;
    SymbolTableEntry* result;
    SymbolTableEntry* argument[2];
    token immVal;
} pentuple;

int numberOfPentuples;

pentuple pentupleCode [MAX_PENTUPLES_POSSIBLE];

#endif ICG_GENERATOR_DEF_H