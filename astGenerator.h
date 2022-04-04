#ifndef AST_GENERATOR_H
#define AST_GENERATOR_H

#include "astDef.h"
#include "parserDef.h"

astNode* createAbstractSyntaxTree (ParseTreeNode* root);
astNode *newASTnode(ASTtag tag);
astNode *newASTleafNode(ASTtag tag);
int freeChildren(ParseTreeNode *root, int low, int high);

#endif