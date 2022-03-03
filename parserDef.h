#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include "lexer.h"
#include "lexerDef.h"
#include "FirstAndFollow.h"
#include "globalDef.h"

//the node used inside a parse tree
typedef struct ParseTreeNode {
    struct ParseTreeNode* parent;
    boolean isLeafNode;   //TRUE -> it is a terminal, FALSE -> it is a non terminal
    int numberStatus; //0: not a number, 1: integer, 2: decimal
    token terminal; 
    int nonTermIndex;
    int integerValue; 
    float floatValue; 
    struct ParseTreeNode* children[MAX_PROD_LEN];
    int numberOfChildren;
    struct ParseTreeNode* nextSibling;
} ParseTreeNode;


typedef struct tnt {
    boolean isTerminal;
    tokenTag terminal;
    int nonTermIndex;
    boolean isEpsilon;
} tnt;

typedef struct r {
    int head;
    tnt body[MAX_PROD_LEN];
    int bodyLength;
} GrammarRule;

GrammarRule grammarRules[MAX_RULES];

int parseTable [MAX_NT][MAX_TERMINALS];

/* Stack for parser */
typedef struct stack {
    tnt* stackElements[STACK_SIZE];
    int stackPointer;
} Stack;

#endif