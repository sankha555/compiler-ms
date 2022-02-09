#ifndef KEYWORD_MAP_H
#define KEYWORD_MAP_H

#define K_MAP_SIZE 31

typedef struct mapEntry {
    char lexeme[21];
    char tag[20];
    struct mapEntry *next;
} mapEntry;

typedef struct keyMap {
    mapEntry *map[K_MAP_SIZE];
} keyMap;

// returns tag on success, NULL when lexeme not found
char* search(keyMap* table, char* lexeme);
int insert(keyMap* table, char* lexeme, char* tag);
int delete(keyMap *table, char* lexeme);
int loadKeyMap(keyMap *table, char* filename);

#endif