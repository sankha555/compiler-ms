#ifndef SYMBOLTABLEDEF_H
#define SYMBOLTABLEDEF_H

#include "globalDef.h"
#include "typing.h"

/* Aliases store information about definetype type declarations */
typedef struct aliasEntry {
    // definetype union #student as #newname
    int unionOrRecord;  // 0 = union, 1 = record
    char* actualName;   // union #student
    char* aliasName;    // #newname

    struct aliasEntry* next;
} AliasEntry;

typedef struct aliasTable {
    int count;
    AliasEntry* head;
} AliasTable;


/* Symbol Table structures */
typedef struct hashTable {
    SymbolTableEntry* table[K_MAP_SIZE];
} HashTable;

typedef struct sTableEntry {
    char* identifier; 
    boolean isFunction;  // whether this entry points to the symbol table of another function
    
    Type type;
    int width;
    int offset;
    SymbolTable* tablePointer;

    SymbolTableEntry* next;
} SymbolTableEntry;

typedef struct sTable {
    int lastStoredOffset;
    SymbolTable returnTo;

    HashTable tableEntries;
} SymbolTable;

#endif