#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

token get_next_token();
twinBuffer* init_lexer(FILE *fp);
void remove_comments(twinBuffer *buffer);
void print_token(FILE* out, token t);

#endif
