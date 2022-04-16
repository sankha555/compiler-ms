/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#include "parserDef.h"

typedef struct stackNode {
    struct stackNode *below;
    tnt *ele;
} StackNode;

typedef struct stack {
    StackNode *top;
    int size;
} Stack;

Stack* newStack();
int push(Stack* s, tnt *ele);
int pop(Stack* s);
tnt *top(Stack* s);
void printStack(Stack* s);
