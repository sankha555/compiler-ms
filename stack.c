#include "stack.h"

Stack* newStack() {
    Stack* newStack = (Stack*)malloc(sizeof(Stack));
    newStack->top = NULL;
    newStack->size = 0;
    return newStack;
}

int push(Stack* s, tnt *ele) {
    StackNode* temp= (StackNode*)malloc(sizeof(StackNode));
    if(!temp) {
        printf("malloc fail.\n");
        return -1;
    }
    temp->ele = ele;
    temp->below = NULL;

    if(s->top == NULL) {
        s->top = temp;
    } else {
        temp->below = s->top;
        s->top = temp;
    }
    s->size++;
    return 0;
}

//num of elements popped
int pop(Stack* s) {
    if(s->top == NULL) {
        return 0;
    }
    else {
        StackNode* ptr = s->top;
        s->top = s->top->below;
        // printf("popped an element.\n");
        // printf("isEpsilon:%d isTerminal:%d", ptr->ele->isEpsilon, ptr->ele->isTerminal);
        // if (ptr->ele->isTerminal){
        //     printf(", terminal %s\n", tokenNames[ptr->ele->terminal]);
        // } else {
        //     printf(", non-terminal %s\n", FirstAndFollowList[ptr->ele->nonTermIndex].symbol);
        // } 
        free(ptr);
        s->size--;
        return 1;
    }
}

tnt *top(Stack* s) {
    if(!s->top) {
        return NULL;
    }
    return s->top->ele;
}

void printStack(Stack* s) {
    printf("--------STACK--------\t%d\n",s->size);
    StackNode* ptr = s->top;
    while(ptr) {
        printf("isEpsilon:%d isTerminal:%d", ptr->ele->isEpsilon, ptr->ele->isTerminal);
        if (ptr->ele->isTerminal){
            printf(", terminal %s\n", tokenNames[ptr->ele->terminal]);
        } else {
            printf(", non-terminal %s\n", FirstAndFollowList[ptr->ele->nonTermIndex].symbol);
        } 
        ptr = ptr->below;
    }
    printf("--------------------\n\n");
}