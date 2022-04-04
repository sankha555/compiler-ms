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
    Return,
    SingleOrRecIdLinkedListNode,
    FieldIdLinkedListNode,
    IdLinkedListNode,
    DefineType
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
    Num,
    RealNum,
    logOp_AND,
    logOp_OR,
    relOp_LT,
    relOp_LE,
    relOp_EQ,
    relOp_GT,
    relOp_GE,
    relOp_NE,
    Id,
    RecUnionId,
    Record,
    Union
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
