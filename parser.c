/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

#include <stdio.h>
#include <string.h>
#include "parserDef.h"
#include "globalDef.h"
#include "lexerDef.h"
#include "lexer.h"
#include "FirstAndFollow.h"
#include "stack.h"
#include <sys/time.h>

char* parseTableFile = PARSE_TABLE_FILE;

tokenTag syncTokens[] = {
    TK_END,
    TK_ENDWHILE,
    TK_ENDIF,
    TK_ENDUNION,
    TK_ENDRECORD,
    TK_SEM
};


void printTnt(tnt* term){
    printf("isEpsilon:%d isTerminal:%d", term->isEpsilon, term->isTerminal);
    if (term->isTerminal){
        printf(", terminal %s\n", tokenNames[term->terminal]);
    } else {
        printf(", non-terminal %s\n", FirstAndFollowList[term->nonTermIndex].symbol);
    }   
}

int numSyncTokens = 6;

void printRule(FILE* fp, GrammarRule gRule){
    NonTerm A = NonTerms[gRule.head];
    tnt* alpha = gRule.body;

    fprintf(fp, "%s ===> ", A.symbol);
    for(int i = 0; i < gRule.bodyLength; i++){
        if(alpha[i].isEpsilon){
            fprintf(fp, " epsilon ");
        }else if(alpha[i].isTerminal){
            fprintf(fp, " %s ", tokenNames[alpha[i].terminal]);
        } else {
            fprintf(fp, " %s ", FirstAndFollowList[alpha[i].nonTermIndex].symbol);   
        }    
    }
    fprintf(fp, ",");
}

void printParseTableToFile(){
    FILE* fp = fopen(parseTableFile, "w+");
    fprintf(fp, ",");
    for(int terminalIndex = 0; terminalIndex < 58; terminalIndex++){
        fprintf(fp, "%s,", tokenNames[terminalIndex]);
    }
    fprintf(fp, "\n");
    for(int nonTerminalIndex = 0; nonTerminalIndex < numNonTerminals; nonTerminalIndex++){
        fprintf(fp, "%s,", FirstAndFollowList[nonTerminalIndex].symbol);
        for(int terminalIndex = 0; terminalIndex < NUMBER_OF_TOKENS; terminalIndex++){
            if(parseTable[nonTerminalIndex][terminalIndex] == -1){
                fprintf(fp, "Error,");
            } else if(parseTable[nonTerminalIndex][terminalIndex] == -2) {
                fprintf(fp, "Synch,");
            }
            else {
                printRule(fp, grammarRules[parseTable[nonTerminalIndex][terminalIndex]]);
            }
        }
        fprintf(fp, "\n");
    }
}

void populateRules(){
    // open the grammar file
    FILE *fp = fopen(GRAMMAR_FILE,"r");

    if (!fp) {
		printf("ERROR: could not open the file for populating rules!\n");
		exit(0);
	}

    char currLine[MAX_PROD_LEN * MAX_LEN];
    
    int grammarRulesIndex = 0;

    while(fgets(currLine, 500, fp) != NULL) {
        char head[MAX_LEN];
        char body[MAX_LEN * MAX_PROD_LEN];

        // get the current line from the file input stream
		// dividing it into head and body
		sscanf(currLine, "%s ===> %[^\n\t]", head, body);
        // printf("HEAD: %s ----> TAIL: %s\n",head,body);
        grammarRules[grammarRulesIndex].head = whichNonTerminal(head);
        
        char *token = strtok(body," ");
        int tailLength = 0;
        while(token != NULL) {
            //if it is a terminal
            int nonTermTailIndex = whichNonTerminal(token);
            if(strcmp(epsilon,token) == 0) {
                grammarRules[grammarRulesIndex].body[tailLength].isEpsilon = TRUE;
                grammarRules[grammarRulesIndex].body[tailLength].isTerminal = FALSE;
                grammarRules[grammarRulesIndex].body[tailLength].terminal = -1;
                grammarRules[grammarRulesIndex].body[tailLength].nonTermIndex = -1;
            }
            else if(nonTermTailIndex < 0) {
                grammarRules[grammarRulesIndex].body[tailLength].isEpsilon = FALSE;
                grammarRules[grammarRulesIndex].body[tailLength].isTerminal = TRUE;
                grammarRules[grammarRulesIndex].body[tailLength].terminal = tokstrToToken(token);
                grammarRules[grammarRulesIndex].body[tailLength].nonTermIndex = -1;
            } else {
                grammarRules[grammarRulesIndex].body[tailLength].isEpsilon = FALSE;
                grammarRules[grammarRulesIndex].body[tailLength].isTerminal = FALSE;
                grammarRules[grammarRulesIndex].body[tailLength].nonTermIndex = nonTermTailIndex;
                grammarRules[grammarRulesIndex].body[tailLength].terminal = -1;
            }
            token = strtok(NULL," ");
            tailLength++;
        }
        grammarRules[grammarRulesIndex].bodyLength = tailLength;
        grammarRulesIndex++;    
    }
    return;
}

void createParseTable(FirstAndFollow FirstAndFollowList){
    memset(parseTable, -1, sizeof(parseTable));

    for(int i = 0; i < numSyncTokens; i++) {
        for(int j = 0; j < numNonTerminals; j++) {
            parseTable[j][syncTokens[i]] = -2;
        }
    }

    for(int ruleIndex = 0; ruleIndex < numRules; ruleIndex++){
        // GrammarRule* prodRule = &grammarRules[ruleIndex];

        int nonTerminalIndex = grammarRules[ruleIndex].head;

        FirstAndFollowElement fnf = FirstAndFollowList[nonTerminalIndex];

        // tnt* alpha = prodRule->body;
        
        for(int j = 0; j < grammarRules[ruleIndex].bodyLength; j++){
            tnt var = grammarRules[ruleIndex].body[j];
            
            if (var.isEpsilon) {
                // printf("epsilon");
                for(int k = 0; k < fnf.followLen; k++){
                    int terminalIndex = fnf.follow[k];
                    parseTable[nonTerminalIndex][terminalIndex] = ruleIndex;
                }

                if (fnf.dollarInFollow){
                    parseTable[nonTerminalIndex][TK_EOF] = ruleIndex;
                }
                break;
            }else if(var.isTerminal){
                // printf("%s ", tokenNames[var.terminal]);
                // first of a terminal is the terminal itself 
                int terminalIndex = var.terminal;
                parseTable[nonTerminalIndex][terminalIndex] = ruleIndex;
                break;
            } else {
                // var is a non-terminal
                FirstAndFollowElement nonTerm = FirstAndFollowList[var.nonTermIndex];
                // printf("%s ", nonTerm.symbol);

                for(int k = 0; k < nonTerm.firstLen; k++){
                    int terminalIndex = nonTerm.first[k];
                    // printf("%d; ", terminalIndex);
                    parseTable[nonTerminalIndex][terminalIndex] = ruleIndex;
                    
                }

                if(!nonTerm.nullable) {
                    break;
                }

                if (nonTerm.nullable && j == grammarRules[ruleIndex].bodyLength-1) {
                    // if the first set of the non-terminal contains epsilon
                    for(int k = 0; k < fnf.followLen; k++){
                        int terminalIndex = fnf.follow[k];
                        parseTable[nonTerminalIndex][terminalIndex] = ruleIndex;
                    }     

                    if (fnf.dollarInFollow){
                        parseTable[nonTerminalIndex][TK_EOF] = ruleIndex;
                    }
                }
            } 
        }
        // printf("\n");
    }
}


void insertRuleBodyIntoStack(Stack* inputStack, GrammarRule gRule){
    // printf("starting to insert rule body into stack\n");
    for(int i = gRule.bodyLength - 1; i >= 0; i--){
        tnt* alpha = (tnt*)malloc(sizeof(tnt));
        alpha->isEpsilon = gRule.body[i].isEpsilon;
        alpha->isTerminal = gRule.body[i].isTerminal;
        alpha->nonTermIndex = gRule.body[i].nonTermIndex;
        alpha->terminal = gRule.body[i].terminal;
        // printf("Inserting new symbol in the stack.\n");
        // printTnt(alpha);
        push(inputStack, alpha);
    }
}

void printTreeNode(FILE* fp, ParseTreeNode* treeNode){
    if (treeNode == NULL) return;   
    fprintf(fp, "Node details:: Is a terminal? %d; ", treeNode->isLeafNode);
    if(treeNode->isLeafNode){
        fprintf(fp, "Terminal ID: %s\n", tokenNames[(treeNode->terminal).type]);
    } else {
        fprintf(fp, "Non terminal ID: %s\n", FirstAndFollowList[treeNode->nonTermIndex].symbol);
    }
}

//1: correctly displayed this subtree, -1: could not display this subtree
int inorderTraversalParseTree (FILE* fp, ParseTreeNode *root) {
    
    if(root == NULL) {
        return 1;
    }

    int i = 0;
    int result = 1;
    char* dash = "----";
    char* epsilon = "Epsilon";
    char* noParent = "<NOPARENT>";
    char* yes = "Yes";
    char* no = "No";
    
    result *= inorderTraversalParseTree(fp,root->children[0]);

    if (root->isEpsilon) {
        if(root->parent == NULL) {
            // printf("----\t\t----\t\tEpsilon\t\t----\t\t<NOPARENT>\t\tYes\t\t----\n");
            fprintf(fp,"%20s%10s%15s%15s%30s%20s%30s\n",dash,dash,epsilon,dash,noParent,yes,dash);
        } else {
            // printf("----\t\t----\t\tEpsilon\t\t----\t\t%s\t\tYes\t\t----\n",FirstAndFollowList[root->parent->nonTermIndex].symbol);
            fprintf(fp,"%20s%10s%15s%15s%30s%20s%30s\n",dash,dash,epsilon,dash,FirstAndFollowList[root->parent->nonTermIndex].symbol,yes,dash);
        }
    }
    else if (root->isLeafNode){
        if(root->terminal.type == TK_NUM || root->terminal.type == TK_RNUM) {
            fprintf(fp,"%20s%10d%15s%15s%30s%20s%30s\n",root->terminal.lexeme, root->terminal.linenumber, tokenNames[root->terminal.type],root->terminal.lexeme,FirstAndFollowList[root->parent->nonTermIndex].symbol,yes,dash);
        }
        // printf("%s\t\t%d:\t\t%s\t\t----\t\tYes\t\t----\n",root->terminal.lexeme, root->terminal.linenumber, tokenNames[root->terminal.type]);
        else { 
            fprintf(fp,"%20s%10d%15s%15s%30s%20s%30s\n",root->terminal.lexeme, root->terminal.linenumber, tokenNames[root->terminal.type],dash,FirstAndFollowList[root->parent->nonTermIndex].symbol,yes,dash);
        }
    } else {
        if(root->parent == NULL) {
            // printf("----\t\t----\t\t----\t\t----\t\t<NOPARENT>\t\tNo\t\t%s\n",FirstAndFollowList[root->nonTermIndex].symbol);
            fprintf(fp,"%20s%10s%15s%15s%30s%20s%30s\n",dash,dash,dash,dash,noParent,no,FirstAndFollowList[root->nonTermIndex].symbol);
        } else {
            // printf("----\t\t----\t\t----\t\t----\t\t%s\t\tNo\t\t%s\n",FirstAndFollowList[root->parent->nonTermIndex].symbol, FirstAndFollowList[root->nonTermIndex].symbol);
            fprintf(fp,"%20s%10s%15s%15s%30s%20s%30s\n",dash,dash,dash,dash,FirstAndFollowList[root->parent->nonTermIndex].symbol,no,FirstAndFollowList[root->nonTermIndex].symbol);
        }
    } 

    // fprintf(fp,"----\t\t----\t\t----\t\t----\t\t%s\t\tNo\t\t%s\n",FirstAndFollowList[root->parent->nonTermIndex].symbol, FirstAndFollowList[root->nonTermIndex].symbol);

    for(i = 1; i < ( root->numberOfChildren ); i++) {
        result *= inorderTraversalParseTree(fp,root->children[i]);
    }

    return result;
}

ParseTreeNode* newParseTreeNode() {
    ParseTreeNode* newNode = (ParseTreeNode*)malloc(sizeof(ParseTreeNode));
    numParseTreeNodes++;
    parseTreeMemory += sizeof(ParseTreeNode);

    // printf("okay\n");
    newNode->parent = NULL;
    newNode->isLeafNode = FALSE;
    newNode->isEpsilon = FALSE;
    token temp;
    temp.linenumber = 0;
    temp.lexeme = "";
    temp.type = TK_ERROR;
    newNode->numberStatus = 0;
    newNode->terminal = temp;
    newNode->nonTermIndex = -1;
    newNode->integerValue = 69;
    newNode->floatValue = 69.69;
    for(int i = 0; i < MAX_PROD_LEN; i++) {
        newNode->children[i] = NULL;
    }
    newNode->ptr = NULL;
    newNode->inhptr = NULL;
    newNode->nextSibling = NULL;
    newNode->numberOfChildren = 0;
    newNode->ruleIndex = -1;
    return newNode;
}

int printParseTree(ParseTreeNode* root) {

    FILE* fp = stdout;

    if(!fp) {
        printf("Could not open file for printing tree.\n");
        return -1;
    }

    fprintf(fp, "============================================================ PARSE TREE ======================================================================\n\n");
    fprintf(fp,"%20s%10s%15s%15s%30s%20s%30s\n","LEXEME","LINE NO.","TOKEN NAME","VALUE IF NUM","PARENT SYMBOL","IS LEAF NODE","NODE SYMBOL");
    fprintf(fp, "----------------------------------------------------------------------------------------------------------------------------------------------\n");

    int result = inorderTraversalParseTree(fp,root);

    fprintf(fp, "==============================================================================================================================================\n\n\n");

    if(fp != stdout){
        fclose(fp);
    }
    return result;
}

ParseTreeNode* findNextSibling(ParseTreeNode* current){
    while(current->nextSibling == NULL){
        current = current->parent;
    }
    return current->nextSibling;
}


ParseTreeNode* createTreeNodeForToken(tnt* var){
    if(!(var->isTerminal)){
        printf("Bruh, this thing not a terminal\n");
        return NULL;
    }

    ParseTreeNode* treeNode = newParseTreeNode();
    treeNode->isLeafNode = TRUE;
    treeNode->terminal.type = var->terminal;

    return treeNode;
}

ParseTreeNode* createTreeNodesFromRule(GrammarRule gRule, ParseTreeNode* parentNode, int parseTableEntry){
    tnt* alpha = gRule.body;

    ParseTreeNode* next = NULL;
    for(int i = gRule.bodyLength - 1; i >= 0; i--){
        tnt var = alpha[i];
        
        ParseTreeNode* treeNode = newParseTreeNode();;
        if(var.isEpsilon){
            treeNode->isEpsilon = TRUE;
            treeNode->isLeafNode = TRUE;
        } else if (var.isTerminal){
            treeNode->isLeafNode = TRUE;
            treeNode->terminal.type = var.terminal;
        } else {
            treeNode->nonTermIndex = var.nonTermIndex;
        }
        treeNode->parent = parentNode; 
        treeNode->nextSibling = next;
        
        // insert this new node as child of parent
        parentNode->children[i] = treeNode;
        parentNode->ruleIndex = parseTableEntry;
        next = treeNode;
    }    
    parentNode->numberOfChildren = gRule.bodyLength;

    return parentNode;
}  

ParseTreeNode* parseInputSourceCode(twinBuffer* buffer, int printStatus){
    // initialize stack
    Stack* inputStack = newStack();

    // pushing '$' to stack
    tnt* stackBottom = (tnt*) malloc(sizeof(tnt));
    stackBottom->isTerminal = TRUE;
    stackBottom->terminal = TK_EOF;
    stackBottom->isEpsilon = FALSE;
    stackBottom->nonTermIndex = -1;
    push(inputStack, stackBottom);

    // pushing <program> to stack
    stackBottom = (tnt*) malloc(sizeof(tnt));
    stackBottom->isTerminal = FALSE;
    stackBottom->terminal = -1;
    stackBottom->isEpsilon = FALSE;
    stackBottom->nonTermIndex = 0;
    push(inputStack, stackBottom);

    // initialize parseTree
    ParseTreeNode* root = newParseTreeNode();

    ParseTreeNode* current = root;

    //a root node for <program> and a root node for <$> needs to be made
    current->nonTermIndex = 0; //corresponding to <program> 
    current->nextSibling = newParseTreeNode(); //node for $ symbol in stack 
    current->nextSibling->isLeafNode = TRUE;
    current->nextSibling->terminal.type = TK_EOF;

    tnt* topOfStack = top(inputStack);

    token currentInputToken;

    // skipping the tokens in input buffer while they are errors
    do {
        currentInputToken = get_next_token(buffer);
        // print_token(stdout,currentInputToken);  
        if(currentInputToken.type == TK_ERROR) {
            print_token(stdout,currentInputToken);
        }  
    } while(currentInputToken.type == TK_ERROR);


    while(topOfStack->terminal != TK_EOF && currentInputToken.type != TK_EOF){
        topOfStack = top(inputStack);

        if(topOfStack->isEpsilon){
            pop(inputStack);
            current = findNextSibling(current);
            continue;
        }
        
        //checking if present token is terminal and same as top of the stack
        else if(topOfStack->isTerminal) {
            if (topOfStack->terminal == currentInputToken.type) {
                current->terminal = currentInputToken;
                pop(inputStack);
                topOfStack = top(inputStack);
                do {
                    currentInputToken = get_next_token(buffer);
                    if(currentInputToken.type == TK_ERROR) {
                        print_token(stdout,currentInputToken);
                    }  
                } while(currentInputToken.type == TK_ERROR);
                current = findNextSibling(current);
            } else {
                // input token and token in top of stack do not match
                printf("Line %4d Error: The token %s for lexeme \"%s\" does not match with the expected token %s\n",currentInputToken.linenumber,tokenNames[currentInputToken.type],currentInputToken.lexeme,tokenNames[topOfStack->terminal]);
                current->terminal.linenumber = currentInputToken.linenumber;
                pop(inputStack);
                topOfStack = top(inputStack);
                current = findNextSibling(current);
            } 
        }

        else if(!(topOfStack->isTerminal)){
            // if top of stack is a non-terminal

            int parseTableEntry = parseTable[topOfStack->nonTermIndex][currentInputToken.type];
            if(parseTableEntry < 0){
                // error condition. Pop the top of stack non-terminal and continue.
                printf("Line %4d Error: Invalid token %s encountered with value \"%s\", stack top -> %s\n",currentInputToken.linenumber,tokenNames[currentInputToken.type],currentInputToken.lexeme,FirstAndFollowList[topOfStack->nonTermIndex].symbol);
                // getchar();
                int flag = 0;
                do {
                    for(int i = 0; (i < FirstAndFollowList[topOfStack->nonTermIndex].firstLen) && (flag == 0); i++) {
                        if(FirstAndFollowList[topOfStack->nonTermIndex].first[i] == currentInputToken.type) {
                            flag = 1;
                        }
                    }
                    for(int i = 0; (i < FirstAndFollowList[topOfStack->nonTermIndex].followLen) && (flag == 0); i++) {
                        if(FirstAndFollowList[topOfStack->nonTermIndex].follow[i] == currentInputToken.type) {
                            flag = 2;
                            pop(inputStack);
                            topOfStack = top(inputStack);
                            current = findNextSibling(current);
                        }
                    }
                    if(flag == 0) {
                        do {
                            currentInputToken = get_next_token(buffer);
                            if(currentInputToken.type == TK_ERROR) {
                                print_token(stdout,currentInputToken);
                            }  
                        } while(currentInputToken.type == TK_ERROR);
                    }
                } while(flag == 0 && currentInputToken.type != TK_EOF);
            }else{
                // there is a rule match between the non-terminal and input token
                // pop current top of stack symbol
                pop(inputStack);

                // push the children of the current stack symbol into the stack
                insertRuleBodyIntoStack(inputStack, grammarRules[parseTableEntry]);

                topOfStack = top(inputStack);
                
                current = createTreeNodesFromRule(grammarRules[parseTableEntry], current, parseTableEntry);
                current = current->children[0];
            }
        }
    }

    if (printStatus) {
        if(currentInputToken.type == TK_EOF && topOfStack->terminal == TK_EOF && printStatus) {
            printf("\nSource code is syntactically correct.\n");
        } else if(currentInputToken.type != TK_EOF) {
            printf("Line %4d Error: Invalid token %s encountered with value \"%s\", stack top -> $\n",currentInputToken.linenumber,tokenNames[currentInputToken.type],currentInputToken.lexeme);
        } else {
            printf("Line %4d Error: Invalid token %s encountered with value \"%s\", stack top -> %s\n",currentInputToken.linenumber,tokenNames[currentInputToken.type],currentInputToken.lexeme,FirstAndFollowList[topOfStack->nonTermIndex].symbol);
        }
    }
    return root;
}
    
