#ifndef TYPING_H
#define TYPING_H

#include "globalDef.h"

typedef enum type {
    Integer,
    Real,
    Record,
    Union,
    // TypeDefinition - controversial, should add or not? I will think later
    //don't need to, as entire structure will be stored in the typeOrRecordInfo anyway
} Type;

typedef struct Field {
    char* identifier;

    int typeIndex;
    int width;
    int offset;

    struct Field* next;
} Field;

typedef struct UnionOrRecordInfo {
    char* identifier;
    
    int offset; 
    Field* listOfFields;
    int totalWidth;
} UnionOrRecordInfo;

typedef struct Parameter {
    int data;
    struct Parameter* next;
} Parameter;

typedef struct FunctionType {
    char* identifier;
    struct Parameter* inputParameters;
    struct Parameter* outputParameters;
} FunctionType;

typedef struct TypeArrayElement {
    boolean isPrimitive;
    boolean isUnion;
    boolean isRecord;
    boolean isFunction;
    boolean isAlias;

    char* identifier;
    struct UnionOrRecordInfo* compositeVariableInfo;
    struct FunctionType functionInfo;
} TypeArrayElement;

int getWidth(Type);

#endif





