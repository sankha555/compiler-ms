#include<stdio.h>
#include<stdlib.h>
#include "keywordMap.h"

int main() {
    keyMap* keywordTable = (keyMap*)malloc(sizeof(keyMap));
    loadKeyMap(keywordTable,"keywords.txt");
    return 0;
}