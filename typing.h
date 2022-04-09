#ifndef TYPING_H
#define TYPING_H

typedef enum type {
    Integer,
    Real,
    Record,
    Union,
    // TypeDefinition - controversial, should add or not? I will think later
    //don't need to, as entire structure will be stored in the typeOrRecordInfo anyway
} Type;

typedef struct Field {
    Type type;
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

#endif





