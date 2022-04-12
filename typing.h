#ifndef TYPING_H
#define TYPING_H

#include "globalDef.h"

//broad type category, used in the type expression
typedef enum Type {
    Integer,
    Real,
    Record,
    Union,
    Function, 
    Alias
    
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
 
    Type type;
    char* identifier; //name of the type
    int aliasTypeInfo; //stores the typeIndex of the actual type if the type is alias
    struct UnionOrRecordInfo* compositeVariableInfo; //points to the Record/Union type expression if Record/Union otherwise NULL
    struct FunctionType* functionInfo; //points to the function type expression if Record/Union otherwise NULL
    struct TypeArrayElement *next; //used in the LinkedList in the hash table Implementation fo the type table
} TypeArrayElement;


//the TypeArray will be implemented as a HashTable 
// basically, identifier of the type will be used to index into the Hash Table where each entry will be a linkedList(in order to deal with collisions)

typedef struct TypeTable{

char *tableID; //to store the name of the type table
struct TypeArrayElement* tableEntries[K_MAP_SIZE]; //type expression hash table for the program

} TypeTable;

int getWidth(Type);
int hashFunction(char* identifier);
int insert(TypeTable* typeTable, TypeArrayElement* entry);
TypeArrayElement* loopkup(TypeTable* typeTable, char* identifier);
TypeTable* createTypeTable(char* tableID);
TypeArrayElement* createTypeArrayElement(Type type, char *identifier);


#endif





