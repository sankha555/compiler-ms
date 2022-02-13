#ifndef BUFFER_H
#define BUFFER_H

// returns the next character from the buffer
char getch();

// places the input character back into the buffer returns the previous character from the buffer
char ungetch(char c);

// creates the twin buffer for the input file
void init_buffer(FILE *fp);

#endif
