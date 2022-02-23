# include <stdio.h>
# include <string.h>
# include <stdlib.h>

// the maximum number of productions in grammar
# define MAX_RULES 100

// the maximum number of non-terminals and terminals
# define MAX_NT 100
# define MAX_TERM 100

// the maximum length of a terminal or non-terminal string
# define MAX_LEN 50

// the maximum length of a production body
// in terms of number of symbols
# define MAX_PRODLEN 10

// passes for follow set computation
# define EPOCHS 2

typedef enum {
	FALSE = 0,
	TRUE = 1
} boolean;

/* string used for denoting epsilon throughout the module */
const char *epsilon = "epsilon";

/* files to write
 * first and follow sets to
 */
const char *firstFile = "./firstSets.txt";
const char *followFile = "./followSets.txt";

/* to contain a single rule */
struct Rule {
	char head[MAX_LEN];
	char body[MAX_PRODLEN][MAX_LEN];
	int bodyLen;	// store the number of symbols in the production
};

/* a data structure to contain all rules in the grammar */
struct Rule Rules[MAX_RULES];

/* to hold a nonterminal
 * and its first and follow sets
 */
struct NonTerm {
	char symbol[MAX_LEN];
	char firsts[MAX_TERM][MAX_LEN];
	char follows[MAX_TERM][MAX_LEN];
	int firstLen;	// count of entities in first set
	int followLen;	// count of entities in follow set
	boolean firstComputed;	// has the first set been computed
	boolean nullable;	// epsilon in first set
};

/* a data structure to hold all nonterminals
 * along with their first and follow sets
 */
struct NonTerm NonTerms[MAX_NT];

/* data structure as per stage-1 interface specifications */
struct FirstAndFollowElement {
	char symbol[MAX_LEN];
	char first[MAX_TERM][MAX_LEN];
	char follow[MAX_TERM][MAX_LEN];
};

typedef struct FirstAndFollowElement FirstAndFollow[MAX_NT];

FirstAndFollow FNF;

// define the grammar file name as grammar type
typedef char* grammar;

/* total rules and nonterminals in the grammar */
int numNonTerminals;
int numRules;

//invoke this function to use the module
/* populates the rules of the grammar from a file
 * also initializes all non-terminals
 * computes the first and follow sets
 * populates the prescribed FirstAndFollow data structure
 * writes the results to appropriate files
 */
void computeFirstAndFollowSets(grammar G);

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
