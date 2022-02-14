#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reloadBuffer(twinBuffer* tBuffer, int bufferID){
    if(!feof(fp)){
        fread((tBuffer->buffer)[bufferID], 1, BUFFER_SIZE, fp);
    }

    tBuffer->forward = 0;
    tBuffer->lexemeBegin = 0;
    tBuffer->currentBufferID = bufferID;
}

twinBuffer* init_buffer(FILE* fp){
    twinBuffer* tBuffer = (twinBuffer*) malloc(sizeof(twinBuffer));
    tBuffer->buffer[0][BUFFER-1] = '\0';
    tBuffer->buffer[1][BUFFER-1] = '\0';
    tBuffer->fp = fp;

    return tBuffer;
}

char getch(twinBuffer* tBuffer){
    int currForward = tBuffer->forward;
    int currBufferID = tBuffer->currentBufferID;

    if(currForward == BUFFER_SIZE-1){
        // if forward pointer is at end of this buffer, reload the other buffer 
        reloadBuffer(tBuffer, !(currBufferID));
    }
    
    currForward = tBuffer->forward;
    currBufferID = tBuffer->currentBufferID;
    if((tBuffer->buffer)[currBufferID][currForward] == '\0'){
        // end of source program
        return '\0';
    } 

    // return normal character
    (tBuffer->forward)++;
    return (tBuffer->buffer)[currBufferID][currForward];
}

char ungetch(twinBuffer* tBuffer){
    (tBuffer->forward)--;
    return (tBuffer->buffer)[tBuffer->currentBufferID][tBuffer->forward];
}