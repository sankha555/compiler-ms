#ifndef SYMBOLTABLEDEF_H
#define SYMBOLTABLEDEF_H

#include "globalDef.h"
#include "typing.h"

/* Aliases store information about definetype type declarations */
typedef struct AliasEntry {
    // definetype union #student as #newname
    int unionOrRecord;  // 0 = union, 1 = record
    char* actualName;   // union #student
    char* aliasName;    // #newname

    struct aliasEntry* next;
} AliasEntry;

typedef struct AliasTable {
    int count;
    struct AliasEntry* head;
} AliasTable;


/* Symbol Table structures */
typedef struct SymbolTableEntry {
    char* identifier; 

    boolean isFunction;  // whether this entry points to the symbol table of another function
    struct SymbolTable* tablePointer;   // required only is `isFunction == TRUE`

    Type type;
    int width;
    int offset;

    struct SymbolTableEntry* next;
} SymbolTableEntry;

typedef struct SymbolTable {
    // header information
    char* tableID;          // should genderally store the name of the function
    int currentOffset;
    struct SymbolTable* returnTo;

    // actual table entries
    struct SymbolTableEntry* tableEntries[K_MAP_SIZE];
} SymbolTable;

#endif