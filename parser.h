/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#ifndef PARSER_H
#define PARSER_H

#include "FirstAndFollow.h"
#include "parserDef.h"
#include "globalDef.h"

void printRule(FILE* fp, GrammarRule gRule);

void printParseTableToFile();

void populateRules();

void createParseTable();

ParseTreeNode* parseInputSourceCode();

int printParseTree(ParseTreeNode* root);

void printTreeNode(FILE* fp, ParseTreeNode* treeNode);

#endif

