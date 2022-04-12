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
    char* identifier; //name of the field

    int typeIndex; //is an index of the type array, denoting the type of that field
    //the type can be primitive/record/union
    
    int width; //width of the type
    int offset; //offset of the type
    //used in the symbol table


    struct Field* next; //for the Field LinkedList
} Field;

typedef struct UnionOrRecordInfo {
    char* identifier; //name of the Union/Record
    boolean isUnion;
    boolean isRecord;
    //done to differentiate between the two types -> further used in type checking
    //int offset;
    Field* listOfFields; // is a linkedList of the fields, data structure defines above
    int totalWidth; //stores the total width of the record/union -> data occupied in memory
} UnionOrRecordInfo;

typedef struct Parameter {
    int datatype; //stores the type of the parameter as an index of the type array
    char *identifier; //name of the parameter in the function declaration
    struct Parameter* next; //for LinkedList implementation
} Parameter;
//for a parameter list, used in the Function type expression as follows

typedef struct FunctionType {
    char* identifier; //name of the function
    struct Parameter* inputParameters; //Parameter List of input parameters
    struct Parameter* outputParameters;//Parameter List of output parameters
} FunctionType;

//element of the dynamic type array 
//datatypes possible : primitive, Union, Record, Function, Alias
typedef struct TypeArrayElement {
    boolean isPrimitive;
    boolean isUnion;
    boolean isRecord;
    boolean isFunction;
    boolean isAlias;

    char* identifier; //name of the type
    struct UnionOrRecordInfo* compositeVariableInfo; //points to the Record/Union type expression if Record/Union otherwise NULL
    struct FunctionType functionInfo; //points to the function type expression if Record/Union otherwise NULL
} TypeArrayElement;

int getWidth(Type);

#endif





