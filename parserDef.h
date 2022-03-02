#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include "lexer.h"
#include "lexerDef.h"
#include "FirstAndFollow.h"
#include "globalDef.h"

typedef struct tnt {
    boolean isTerminal;
    tokenTag terminal;
    int nonTermIndex;
} tnt;

typedef struct r {
    int head;
    tnt body[MAX_PROD_LEN];
    int bodyLength;
} GrammarRule;

GrammarRule grammarRules[MAX_RULES];

int parseTable [MAX_NT][MAX_TERMINALS];

#endif