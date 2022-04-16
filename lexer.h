/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

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
int removeComments(char* src);
void print_token(FILE* out, token t);

//converts "TK_IF" to TK_IF
tokenTag tokstrToToken(char * str);

#endif
