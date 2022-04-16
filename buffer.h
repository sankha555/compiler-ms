/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include "globalDef.h"

typedef struct b {
    char buffer[2][BUFFER_SIZE];
    int forward;
    int currentBufferID;
    FILE* fp;
} twinBuffer;

// creates the twin buffer for the input file
twinBuffer* init_buffer(FILE *fp);

// returns the next character from the buffer
char getch(twinBuffer* tBuffer);

// places the input character back into the buffer returns the previous character from the buffer
void ungetch(twinBuffer* tBuffer);


#endif
