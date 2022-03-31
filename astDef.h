#ifndef ASTDEF_H
#define ASTDEF_H

#include "globalDef.h"
#include "lexerDef.h"

typedef enum ASTtag {
    Program,
    MainFunc,
    FuncLinkedListNode,
    FuncDef
} ASTtag;

typedef enum ASTLeafTag {
    FuncId
} ASTLeafTag;

typedef struct astNode {
    ASTtag type;
    ASTLeafTag leafType;
    
    boolean isLinkedListNode;
    boolean isLeafNode;

    int intValue;
    float realValue;
    token entry;

    struct astNode* next;
    struct astNode* data;
    struct astNode* children[MAX_PROD_LEN];
} astNode;


#endif