#ifndef ASTDEF_H
#define ASTDEF_H

#include "globalDef.h"
#include "lexerDef.h"
#include "symbolTableDef.h"

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
    DefineType,
    FuncCall,
    While,
    If,
    Else,
    Read,
    Write,
    logOp_NOT,
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
    Id, // Seems redundant to VariableId
    RecUnionId,
    Record,
    Union,
    arithOp_MUL,
    arithOp_DIV,
    arithOp_PLUS,
    arithOp_MINUS
} ASTtag;

typedef struct astNode
{
    ASTtag type;

    boolean isLinkedListNode;
    boolean isLeafNode; // only this used to identify if leaf or not

    int intValue;
    float realValue;
    token entry;

    char* code;
    char* dataPlace;

    struct astNode *next;
    struct astNode *data;
    struct astNode *children[MAX_PROD_LEN];
    
} astNode;

#endif
