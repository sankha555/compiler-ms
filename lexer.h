#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"
#include "buffer.h"
#include "globalDef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

token get_next_token();
twinBuffer* init_lexer(char* filename);
int removeComments(char* src, char* dest);
void print_token(FILE* out, token t);

//converts "TK_IF" to TK_IF
tokenTag tokstrToToken(char * str);

#endif
