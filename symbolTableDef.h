#ifndef SYMBOLTABLEDEF_H
#define SYMBOLTABLEDEF_H

#include "globalDef.h"
#include "typing.h"
#include "astDef.h"

/* Aliases store information about definetype type declarations */
typedef struct AliasEntry {
    // definetype union #student as #newname
    int unionOrRecord;  // 0 = union, 1 = record
    char* actualName;   // union #student
    char* aliasName;    // #newname

    struct aliasEntry* next;
} AliasEntry;

//AliasTable is actually a linkedList of Alias entries
typedef struct AliasTable {
    int count;
    struct AliasEntry* head;
} AliasTable;


/* Symbol Table structures */
/* Symbol Table structures */
typedef struct SymbolTableEntry {
    char* identifier; 

    boolean isFunction;  // whether this entry points to the symbol table of another function
    struct SymbolTable* tablePointer;   // required only if `isFunction == TRUE`

    
    TypeArrayElement* type; //pointer to the array type -> will be found using the lookup operation in the hash table
    int width;
    int offset;

    char* usage; //stores the usage of the identifier, for example, if it is inparameter, outparameter, or variable

    struct SymbolTableEntry* next;
} SymbolTableEntry;


typedef struct SymbolTable {
    // header information
    char* tableID;          // should generally store the name of the function
    int currentOffset;
    int totalWidth;

    struct SymbolTable* returnTo;

    // actual table entries
    struct SymbolTableEntry* tableEntries[K_MAP_SIZE];

    struct SymbolTable* next;
} SymbolTable;

SymbolTable* listOfSymbolTables;    // a linked list of pointers to all symbol tables for the program

//astNode* aliasTemp;

#endif
