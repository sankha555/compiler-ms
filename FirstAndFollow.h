/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

# ifndef HEADER_FILE
# define HEADER_FILE

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
#include "globalDef.h"

/* string used for denoting epsilon throughout the module */
static char *epsilon = "epsilon";

/* files to write
 * first and follow sets to
 */
static char *firstFile = "./firstSets.txt";
static char *followFile = "./followSets.txt";

/* to contain a single rule */
struct Rule {
	char head[MAX_LEN];
	char body[MAX_PROD_LEN][MAX_LEN];
	int bodyLen;	// store the number of symbols in the production
};

/* a data structure to contain all rules in the grammar */
struct Rule Rules[MAX_RULES];

/* to hold a nonterminal
 * and its first and follow sets
 */
typedef struct NonTerm {
	char symbol[MAX_LEN];
	char firsts[MAX_TERMINALS][MAX_LEN];
	char follows[MAX_TERMINALS][MAX_LEN];
	int firstLen;	// count of entities in first set
	int followLen;	// count of entities in follow set
	boolean firstComputed;	// has the first set been computed
	boolean nullable;	// epsilon in first set
} NonTerm;

/* a data structure to hold all nonterminals
 * along with their first and follow sets
 */
NonTerm NonTerms[MAX_NT];

/* data structure as per stage-1 interface specifications */
typedef struct FirstAndFollowElement {
	char symbol[MAX_LEN];
	int first[MAX_TERMINALS];
	int follow[MAX_TERMINALS];
	int firstLen;
	int followLen;
	boolean nullable;
	boolean dollarInFollow;
} FirstAndFollowElement;

typedef struct FirstAndFollowElement FirstAndFollow[MAX_NT];

FirstAndFollow FirstAndFollowList;

// define the grammar file name as grammar type
typedef char* grammar;

/* total rules and nonterminals in the grammar */
int numNonTerminals;
int numRules;

char TerminalList[MAX_TERMINALS][MAX_LEN];

//invoke this function to use the module
/* populates the rules of the grammar from a file
 * also initializes all non-terminals
 * computes the first and follow sets
 * populates the prescribed FirstAndFollow data structure
 * writes the results to appropriate files
 */
FirstAndFollowElement* computeFirstAndFollowSets(grammar G);

/* creates rules from the grammar file G
 * also creates a list of non-terminals
 */
void populateGrammar(grammar G);

/* returns the index of non-terminal, if present
 * returns -1 if not found
 */
int whichNonTerminal(char *symbol);

/* --- functions for computation of first sets --- */
/* loop over the list of non-terminals
 * finding the first set for each
 * algo on pg. 221
 */
void computeFirstAll();

/* find the first set of a specific non-terminal */
void computeFirstOf(int nonTermIndex);

/* use the rule body beginning at a specific position
 * to populate the first set of head
 */
void computeFirstFromRule(int headIndex, int ruleIndex, int start);

/* helper function: token present in the first set already? */
boolean presentInFirst(int index, char* token);


/* --- functions for computation of follow sets --- */
/* pg. 221-222 algorithm to compute follow sets */
void computeFollowAll();

/* find the position of the non-terminal in the production body
 * if it is present
 */
int findPosOf(int ruleIndex, char *symbol);

/* to compute the first set of a suffix in a production body */
boolean getFirstOfString(int ruleIndex, int start, 
		char firstSet[][MAX_LEN], int* firstSetLen);

/* helper function: token present in first set of suffix already? */
boolean presentInArr(char firstSet[][MAX_LEN],
		int firstSetLen, char *token);

/* helper function: token present in follow set already */
boolean presentInFollow(int index, char* token);

/* populate the prescribed FirstAndFollow data structure */
void populateFirstAndFollow();

/* write the first and follow sets to respective files */
void writeFirstsToFile();
void writeFollowsToFile();

// for testing and debugging
void printTest();

// populate array of Terminals
void populateTerminals();
int searchTerminal();

# endif