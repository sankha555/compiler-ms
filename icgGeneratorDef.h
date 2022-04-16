#ifndef ICG_GENERATOR_DEF_H
#define ICG_GENERATOR_DEF_H

#include "globalDef.h"
#include "symbolTableDef.h"
#include "lexerDef.h"

typedef enum ICGrule {
    ADD_I,          // DONE   add two integers in memory
    ADD_R,          //    add two reals in memory
    SUB_I,          // DONE   subtract two integers in memory
    SUB_R,          //    subtract two reals in memory
    MUL_I,          // DONE   multiply two integers in memory
    MUL_R,          //    multiply two reals in memory
    DIV_I,          // DONE   divide two integers in memory
    DIV_R,          //    divide two reals in memory
    GOTO_L,         //    unconditional jump to label
    IF_TRUE_GOTO_L, 
    IF_FALSE_GOTO_L,
    ASSIGN_OP_INT,  // DONE   copy from one memory location 2 bytes to another
    ASSIGN_OP_REAL, // DONE   copy from one mem 4 bytes to another mem
    DEFINE_DS,      // DONE       define data segment, in code generation, adds function offsets
    DEFINE_CS,      // DONE       define code segment, add "global _start:" in code generation
    EXIT_CODE,      // DONE       in the end of intermediate code generation, adds the exit syscall during code generation
    FUNC_DEF,       // DONE       function label in the code segment, push EBP into stack, and move the function offset from constant DS into EBP
    FUNC_DEF_END,   // DONE     pop from stack into EBP, then return
    FUNC_DEF_MAIN,  // DONE
    CONVERT_TO_REAL, 
    SETUP_CALL_TRANS,
    PUSH_INPUT_VAR,
    PUSH_INPUT_IMMEDIATE,
    CALL_FUNC,
    POP_OUTPUT,
    INSERT_LABEL,
    READ,
    WRITE_IMMEDIATE,
    WRITE_VAR,
    ASSIGN_IMMEDIATE_INT,   //DONE
    ASSIGN_IMMEDIATE_REAL,
    BOOLEAN_NOT,
    BOOLEAN_AND,
    BOOLEAN_OR,
    REL_EQ_INT,
    REL_EQ_REAL,
    REL_GEQ_INT,
    REL_GEQ_REAL,
    REL_GT_INT,
    REL_GT_REAL,
    REL_LEQ_INT,
    REL_LEQ_REAL,
    REL_LT_INT,
    REL_LT_REAL,
    REL_NEQ_INT,
    REL_NEQ_REAL
} ICGrule;

typedef struct immediateOrSTE {
    SymbolTableEntry* ste;
    token immediate;
    boolean isSTE;
} immediateOrSTE;

typedef struct pentuple {
    ICGrule rule;
    SymbolTableEntry* result;
    SymbolTableEntry* argument[2];
    token immVal;
    char* jumpLabel;
} pentuple;

int numberOfPentuples;

pentuple pentupleCode [MAX_PENTUPLES_POSSIBLE];

#endif