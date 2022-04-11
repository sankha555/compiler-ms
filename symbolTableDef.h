#ifndef SYMBOLTABLEDEF_H
#define SYMBOLTABLEDEF_H

#include "globalDef.h"
#include "typing.h"

struct SymbolTable;

/* Symbol Table structures */
typedef struct SymbolTableEntry
{
    char *identifier;

    boolean isFunction;                // whether this entry points to the symbol table of another function
    struct SymbolTable *functionTablePointer; // required only is `isFunction == TRUE`
    Type *functionInputArguments;      // required only if `isFunction == TRUE`
    int functionInputArgumentCount;    // required only if `isFunction == TRUE`
    Type *functionOutputArguments;     // required only if `isFunction == TRUE`
    int functionOutputArgumentCount;   // required only if `isFunction == TRUE`

    boolean isUnionOrRecord;              // used when the identifier is a record/union
    UnionOrRecordInfo *unionOrRecordInfo; // used only when isUnionorRecord == TRUE

    Type type;
    int width;
    int offset;

    struct SymbolTableEntry *next;
} SymbolTableEntry;

typedef struct SymbolTable
{
    // header information
    char *tableID; // should generally store the name of the function
    int currentOffset;
    int totalWidth;
    struct SymbolTable *returnTo;

    // actual table entries
    struct SymbolTableEntry *tableEntries[K_MAP_SIZE];
} SymbolTable;

#endif
