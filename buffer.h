#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include "globalDef.h"

typedef struct b {
    char buffer[2][BUFFER_SIZE];
    int forward;
    int lexemeBegin; 
    int currentBufferID;
    FILE* fp;
} twinBuffer;

// creates the twin buffer for the input file
twinBuffer* init_buffer(FILE *fp);

// returns the next character from the buffer
char getch(twinBuffer* tBuffer);

// places the input character back into the buffer returns the previous character from the buffer
char ungetch(twinBuffer* tBuffer);


#endif
