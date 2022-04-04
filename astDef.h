#ifndef ASTDEF_H
#define ASTDEF_H

#include "globalDef.h"
#include "lexerDef.h"

typedef enum ASTtag
{
    Program,
    MainFunc,
    FuncLinkedListNode,
    FuncDef,
    ParameterLinkedListNode,
    Parameter,
    Stmts,
    TypeDefLinkedListNode,
    TypeRecordDefinition,
    TypeUnionDefinition,
    FieldDefLinkedListNode,
    FieldDefinition,
    DeclarationLinkedListNode,
    Declaration,
    StmtLinkedListNode,
    AssignmentOperation,
    SingleOrRecIdLinkedListNode,
    FieldIdLinkedListNode,
} ASTtag;

typedef enum ASTLeafTag
{
    FuncId,
    VariableId,
    TypeInt,
    TypeReal,
    TypeRecord,
    TypeUnion,
    TypeRecordUnionId,
    FieldId,
    FieldTypeRUID,
    Global,
} ASTLeafTag;

typedef struct astNode
{
    ASTtag type;
    ASTLeafTag leafType;

    boolean isLinkedListNode;
    boolean isLeafNode;

    int intValue;
    float realValue;
    token entry;

    struct astNode *next;
    struct astNode *data;
    struct astNode *children[MAX_PROD_LEN];
} astNode;

#endif
