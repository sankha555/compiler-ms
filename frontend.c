#include<stdio.h>
#include<stdlib.h>
#include "keywordMap.h"
#include "token.h"

int main() {
    keyMap* keywordTable = (keyMap*)malloc(sizeof(keyMap));
    loadKeyMap(keywordTable,"keywords.txt");
    if(search(keywordTable,"global") == TK_GLOBAL) {
        printf("TK_GLOBAL\n");
    }
    return 0;
}