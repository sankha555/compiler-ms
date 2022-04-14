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