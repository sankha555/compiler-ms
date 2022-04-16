/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#include "lexerDef.h"
#ifndef KEYWORD_MAP_H
#define KEYWORD_MAP_H

#include "globalDef.h"

typedef struct mapEntry {
    char lexeme[21];
    tokenTag tag;
    struct mapEntry *next;
} mapEntry;

typedef struct keyMap {
    mapEntry *map[K_MAP_SIZE];
} keyMap;

// returns tag on success, -1 when lexeme not found
tokenTag search(keyMap* table, char* lexeme);
int insert(keyMap* table, char* lexeme, tokenTag tag);
int delete(keyMap *table, char* lexeme);
int loadKeyMap(keyMap *table, char* filename);

#endif