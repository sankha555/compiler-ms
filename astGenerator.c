#include "astGenerator.h"
#include "astDef.h"
#include "parserDef.h"
#include "lexerDef.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


astNode* newASTnode(ASTtag tag) {
    astNode* ptr = (astNode*)malloc(sizeof(astNode));
    ptr->type = tag;
    ptr->leafType = -1;
    ptr->isLeafNode = FALSE;
    ptr->next = NULL;
    ptr->data = NULL;
    ptr->isLinkedListNode = FALSE;
    for(int i = 0; i < MAX_PROD_LEN; i++) {
        ptr->children[i] = NULL;
    }
    return ptr;
    ptr->intValue = 0;
    ptr->realValue = 0;
}

astNode* newASTleafNode(ASTLeafTag tag) {
    astNode* ptr = (astNode*)malloc(sizeof(astNode));
    ptr->type = -1;
    ptr->leafType = tag;
    
    ptr->isLinkedListNode = FALSE;
    ptr->isLeafNode = TRUE;

    ptr->intValue = 0;
    ptr->realValue = 0;

    ptr->next = NULL;
    ptr->data = NULL;
    for(int i = 0; i < MAX_PROD_LEN; i++) {
        ptr->children[i] = NULL;
    }

    return ptr;
}

int freeChildren(ParseTreeNode* root, int low, int high) {
    if((low > high) || (high >= root->numberOfChildren)) {
        printf("Error deleting children [%d,%d] of node:\n",low,high);
        printTreeNode(stdout,root);
        return -1;
    }
    for(int i = low; i <= high; i++) {
        free(root->children[i]);
    }
    return 1;
}

astNode* createAbstractSyntaxTree (ParseTreeNode* root) {

    //generally we have to traverse the child nodes first before
    //applying rules, but not for some cases, in which inhptrs are required

    astNode* ptr = NULL;

    switch(root->ruleIndex) {
        case 0: ptr = newASTnode(Program);
                createAbstractSyntaxTree(root->children[0]);
                createAbstractSyntaxTree(root->children[1]);
                ptr->children[0] = root->children[0]->ptr;
                ptr->children[1] = root->children[1]->ptr;
                root->ptr = ptr;
                freeChildren(root,0,1);
                return ptr;
                break;
        case 1: ptr = newASTnode(MainFunc);
                createAbstractSyntaxTree(root->children[1]);
                ptr->children[0] = root->children[1]->ptr;
                root->ptr = ptr;
                if(freeChildren(root,0,2) < 0)
                return ptr;
                break;
        case 2: ptr = newASTnode(FuncLinkedListNode);
                createAbstractSyntaxTree(root->children[0]);
                createAbstractSyntaxTree(root->children[1]);
                ptr->isLinkedListNode = TRUE;
                ptr->data = root->children[0]->ptr;
                ptr->next = root->children[1]->ptr;
                root->ptr = ptr;
                freeChildren(root,0,1);
                return ptr;
                break;
        case 3: root->ptr = NULL;
                freeChildren(root,0,0);
                return NULL;
                break;
        case 4: ptr = newASTnode(FuncDef);
                createAbstractSyntaxTree(root->children[1]);
                createAbstractSyntaxTree(root->children[2]);
                createAbstractSyntaxTree(root->children[4]);
                ptr->children[0] = newASTleafNode(FuncId);
                ptr->children[0]->entry = root->children[0]->terminal;
                ptr->children[1] = root->children[1]->ptr;
                ptr->children[2] = root->children[2]->ptr;
                ptr->children[4] = root->children[4]->ptr;
                root->ptr = ptr;
                freeChildren(root,0,5);
                return ptr;
                break;
        case 5: createAbstractSyntaxTree(root->children[4]);
                root->ptr = root->children[4]->ptr;
                freeChildren(root,0,5);
                return ptr;
                break;
        case 6: createAbstractSyntaxTree(root->children[4]);
                root->ptr = root->children[4]->ptr;
                freeChildren(root,0,5);
                return ptr;
                break;
        default:
                printf("Unmatched rule encountered. Exiting!!\n");
                exit(-1);
                break;

    }

    return NULL;
}
