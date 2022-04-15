#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reloadBuffer(twinBuffer *tBuffer, int bufferID)
{
    memset(tBuffer->buffer[bufferID], EOF, BUFFER_SIZE);
    fread((tBuffer->buffer)[bufferID], sizeof(char), BUFFER_SIZE, tBuffer->fp);
    tBuffer->forward = 0;
    tBuffer->currentBufferID = bufferID;
    // printf("reloaded buffer\n");
}

twinBuffer *init_buffer(FILE *fp)
{
    twinBuffer *tBuffer = (twinBuffer *)malloc(sizeof(twinBuffer));
    if (tBuffer == NULL)
    {
        return tBuffer;
    }
    tBuffer->forward = 0;
    tBuffer->currentBufferID = 0;
    tBuffer->fp = fp;
    reloadBuffer(tBuffer, tBuffer->currentBufferID);
    return tBuffer;
}

char getch(twinBuffer *tBuffer)
{
    int currBufferID = tBuffer->currentBufferID;

    if (tBuffer->forward == BUFFER_SIZE - 1)
    {
        if (currBufferID == 0)
            reloadBuffer(tBuffer, 1);
        else
            reloadBuffer(tBuffer, 0);
    }

    currBufferID = tBuffer->currentBufferID;
    if ((tBuffer->buffer)[currBufferID][tBuffer->forward] == EOF)
    {
        // end of source program
        fclose(tBuffer->fp);
        return '\0';
    }

    return (tBuffer->buffer)[currBufferID][tBuffer->forward++];
}

void ungetch(twinBuffer *tBuffer)
{
    if(tBuffer->forward == 0)
    {
        tBuffer->currentBufferID = 1 - tBuffer->currentBufferID;
        tBuffer->forward = BUFFER_SIZE - 1;
    }
    else
    {
        (tBuffer->forward)--;
    }
}
