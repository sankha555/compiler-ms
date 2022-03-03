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
