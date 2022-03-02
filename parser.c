#include <stdio.h>
#include <string.h>
#include "parserDef.h"
#include "globalDef.h"
#include "lexerDef.h"
#include "lexer.h"

char* parseTableFile = "parseTable.txt";

void printRule(FILE* fp, GrammarRule gRule){
    NonTerm A = NonTerms[gRule.head];
    tnt* alpha = gRule.body;

    fprintf(fp, "%s ===> ", A.symbol);
    for(int i = 0; i < gRule.bodyLength; i++){
        if(alpha[i].isTerminal){
            fprintf(fp, " %s ", tokenNames[alpha[i].terminal]);
        } else {
            fprintf(fp, " %s ", FirstAndFollowList[alpha[i].nonTermIndex].symbol);   
        }    
    }
    fprintf(fp, "\n");
}

void printParseTableToFile(){
    FILE* fp = fopen("parseTable.txt", "w+");
    for(int terminalIndex = 0; terminalIndex < NUMBER_OF_TOKENS; terminalIndex++){
        fprintf(fp, "\t\t\t%s\t\t\t", tokenNames[terminalIndex]);
    }
    for(int nonTerminalIndex = 0; nonTerminalIndex < MAX_NT; nonTerminalIndex++){
        fprintf(fp, "%s \t\t\t", FirstAndFollowList[nonTerminalIndex].symbol);
        for(int terminalIndex = 0; terminalIndex < MAX_TERMINALS; terminalIndex++){
            if(parseTable[nonTerminalIndex][terminalIndex] == -1){
                fprintf(fp, "Error");
            } else {
                printRule(fp, grammarRules[parseTable[nonTerminalIndex][terminalIndex]]);
            }
        }
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

        grammarRules[grammarRulesIndex].head = whichNonTerminal(head);
        
        char *token = strtok(body," ");
        int tailLength = 0;
        while(token != NULL) {
            //if it is a terminal
            int nonTermTailIndex = whichNonTerminal(token);
            if(nonTermTailIndex < 0) {
                grammarRules[grammarRulesIndex].body[tailLength].isTerminal = TRUE;
                grammarRules[grammarRulesIndex].body[tailLength].terminal = tokstrToToken(token);
                grammarRules[grammarRulesIndex].body[tailLength].nonTermIndex = -1;
            } else {
                grammarRules[grammarRulesIndex].body[tailLength].isTerminal = FALSE;
                grammarRules[grammarRulesIndex].body[tailLength].nonTermIndex = nonTermTailIndex;
                grammarRules[grammarRulesIndex].body[tailLength].terminal = -1;
            }
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

        NonTerm A = NonTerms[prodRule->head];
        tnt* alpha = prodRule->body;

        int nonTerminalIndex = whichNonTerminal(A.symbol);
        for(int j = 0; j < prodRule->bodyLength; j++){
            tnt var = alpha[j];
            if(var.isTerminal){
                // first of a terminal is the terminal itself 
                int terminalIndex = var.terminal;
                parseTable[nonTerminalIndex][terminalIndex] = ruleIndex;
            } else {
                // fetch each 'a' in the follow set of the non terminal
                FirstAndFollowElement fnf = FirstAndFollowList[var.nonTermIndex];
                
                if (fnf.nullable) {
                    // epsilon is present in first set of the non-terminal
                    for(int terminalIndex = 0; terminalIndex < A.followLen; terminalIndex++){
                        parseTable[nonTerminalIndex][terminalIndex] = ruleIndex;
                    }        

                    if (fnf.dollarInFollow){
                        parseTable[nonTerminalIndex][TK_EOF] = ruleIndex;
                    }
                }
            
            }
        }
    }
}


void writeParseTableToFile(){
    populateRules();
    createParseTable();
    printParseTableToFile();
}

int main(){
    writeParseTableToFile();

    return 0;
}