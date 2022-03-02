#include <stdio.h>
#include <string.h>
#include "parserDef.h"
#include "globalDef.h"
#include "lexerDef.h"
#include "lexer.h"
#include "FirstAndFollow.h"

char* parseTableFile = "parseTable.csv";

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
            } else {
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
            // printf("\t\tnonTerminalIndex: %d string: %s\n",nonTermTailIndex,token);
            if(strcmp(epsilon,token) == 0) {
                //printf("\t\tepsilon pushed.\n");
                grammarRules[grammarRulesIndex].body[tailLength].isEpsilon = TRUE;
                grammarRules[grammarRulesIndex].body[tailLength].isTerminal = FALSE;
                grammarRules[grammarRulesIndex].body[tailLength].terminal = -1;
                grammarRules[grammarRulesIndex].body[tailLength].nonTermIndex = -1;
            }
            else if(nonTermTailIndex < 0) {
                //printf("\t\tToken number for string : %s\n",token);
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

void createParseTable(){
    memset(parseTable, -1, sizeof(parseTable));
    for(int ruleIndex = 0; ruleIndex < numRules; ruleIndex++){
        GrammarRule* prodRule = &grammarRules[ruleIndex];

        printf("Body Length: %d\n", prodRule->bodyLength);

        int nonTerminalIndex = prodRule->head;
        printf("%d. ", nonTerminalIndex);

        FirstAndFollowElement fnf = FirstAndFollowList[nonTerminalIndex];
        printf("%s ===> ", fnf.symbol);

        tnt* alpha = prodRule->body;
        
        for(int j = 0; j < prodRule->bodyLength; j++){
            tnt var = alpha[j];
            
            if (var.isEpsilon) {
                printf("epsilon");
                for(int k = 0; k < fnf.followLen; k++){
                    int terminalIndex = fnf.follow[k];
                    parseTable[nonTerminalIndex][terminalIndex] = ruleIndex;
                }

                if (fnf.dollarInFollow){
                    parseTable[nonTerminalIndex][TK_EOF] = ruleIndex;
                }
            }else if(var.isTerminal){
                printf("%s ", tokenNames[var.terminal]);
                // first of a terminal is the terminal itself 
                int terminalIndex = var.terminal;
                parseTable[nonTerminalIndex][terminalIndex] = ruleIndex;
            } else {
                // var is a non-terminal
                FirstAndFollowElement nonTerm = FirstAndFollowList[var.nonTermIndex];
                printf("%s ", nonTerm.symbol);

                for(int k = 0; k < nonTerm.firstLen; k++){
                    int terminalIndex = nonTerm.first[k];
                    printf("%d; ", terminalIndex);
                    parseTable[nonTerminalIndex][terminalIndex] = ruleIndex;
                }

                if (nonTerm.nullable) {
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
        printf("\n");
    }
}


void writeParseTableToFile(){
    computeFirstAndFollowSets(GRAMMAR_FILE);
    printf("computed first follow sets.\n");

    for(int i = 0; i < numNonTerminals; i++){
        FirstAndFollowElement fnf = FirstAndFollowList[i];
        printf("First Set: [");
        for(int j = 0; j < fnf.firstLen; j++){
            printf("%d, ", fnf.first[j]);
        }
        printf("]\n");

        printf("Follow Set: [");
        for(int j = 0; j < fnf.followLen; j++){
            printf("%d, ", fnf.follow[j]);
        }
        printf("]\n");
    }

    populateRules();
    printf("populated Rules\n");
    createParseTable();
    printf("created parse table.\n");
    printParseTableToFile();
}

int main(){
    writeParseTableToFile();

    return 0;
}