#include "keywordMap.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

int kmapHash(char *lexeme) {
    int hash = 7;
    for(int i = 0; i < strlen(lexeme); i++) {
        hash = (hash*31 + lexeme[i])%K_MAP_SIZE;
    }
    return hash;
}

char* search(keyMap* table, char* lexeme) {
    int hashIndex = kmapHash(lexeme);
    mapEntry* pointer = table->map[hashIndex];
    while(pointer != NULL) {
        if(strcmp(pointer->lexeme,lexeme) == 0) {
           return pointer->tag;
        }
        pointer = pointer->next;
    }
    return NULL;
}

//1: item added in the hash table
//0: item updated in the table
int insert(keyMap* table, char* lexeme, char* tag) {
    int hashIndex = kmapHash(lexeme);
    mapEntry* pointer = table->map[hashIndex];
    mapEntry* newptr = (mapEntry*)malloc(sizeof(mapEntry));
    strcpy(newptr->lexeme,lexeme);
    strcpy(newptr->tag,tag);
    newptr->next = NULL;
    if(pointer == NULL) {
        table->map[hashIndex] = newptr;
        // printf("Lexeme:%s Tag:%s HashIndex:%d inserted in map.\n",lexeme,tag,hashIndex);
        return 1;
    }
    while(pointer->next != NULL) {
        if(strcmp(pointer->lexeme,lexeme) == 0) {
            //entry already exists, update tag
            strcpy(pointer->tag,tag);
            free(newptr);
            // printf("Lexeme:%s Tag:%s HashIndex:%d updated in map.\n",lexeme,tag,hashIndex);
            return 0;    
        }
        pointer = pointer->next;
    }
    pointer->next = newptr;
    // printf("Lexeme:%s Tag:%s HashIndex:%d inserted in map.\n",lexeme,tag,hashIndex);
    return 1;
}

//1: item deleted
//0: item not found
//-1: item could not be deleted
int delete(keyMap *table, char* lexeme) {
    int hashIndex = kmapHash(lexeme);
    mapEntry* pointer = table->map[hashIndex];
    mapEntry* temp = NULL;
    if(pointer == NULL) {
        return 0;
    }
    if(strcmp(pointer->lexeme, lexeme) == 0) {
        temp = pointer;
        table->map[hashIndex] = pointer->next;
        free(temp);
        return 1;
    }
    mapEntry* prev = pointer;
    pointer = pointer->next;
    while(pointer != NULL) {
        if(strcmp(pointer->lexeme, lexeme) == 0) {
            prev->next = pointer->next;
            temp = pointer;
            free(temp);
            return 1;
        }
        prev = pointer;
        pointer = pointer->next;
    }
    return 0;
}

//-1: failure to load
//1: successfully loaded
int loadKeyMap(keyMap* table, char* filename) {
    FILE *fptr = fopen(filename, "r");
    if(fptr == NULL) {
        printf("Could not open keywords.txt");
        return -1;
    }
    char lexeme[21];
    char tag[21];
    while(fscanf(fptr,"%s%s",lexeme,tag) == 2) {
        printf("Inserting lexeme: %s \t\t tag: %s\n",lexeme,tag);
        insert(table,lexeme,tag);
    }
    return 1;
}