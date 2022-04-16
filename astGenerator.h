/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#ifndef AST_GENERATOR_H
#define AST_GENERATOR_H

#include "astDef.h"
#include "parserDef.h"

astNode* createAbstractSyntaxTree (ParseTreeNode* root);

int printAbstractSyntaxTree (astNode* root, FILE* fp);

astNode *newASTnode(ASTtag tag);

astNode *newASTleafNode(ASTtag tag);

int freeChildren(ParseTreeNode *root, int low, int high);

char* getStatmType(int typeIndex);

#endif