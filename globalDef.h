#ifndef GLOBAL_DEF_H
#define GLOBAL_DEF_H

/* total rules and nonterminals in the grammar */
#define NUMBER_OF_TOKENS 58
int numNonTerminals;
int numRules;

//size of keyword map used in loading keywords 
#define K_MAP_SIZE 31

//buffer size for twin buffer
#define BUFFER_SIZE 4096    // buffer size

//name of the file in which grammar is written
#define GRAMMAR_FILE "finalgrammar.txt"

#define TREE_FILE "parsetree.txt"

#define CLEAN_FILE "cleanedcode.spl"

// the maximum number of productions in grammar
# define MAX_RULES 100

// the maximum number of non-terminals and terminals
# define MAX_NT 100
# define MAX_TERMINALS 100

// the maximum length of a terminal or non-terminal string
# define MAX_LEN 50

// the maximum length of a production body
// in terms of number of symbols
# define MAX_PROD_LEN 10

// passes for follow set computation
# define EPOCHS 2



//to denote false an true anywhere required
typedef enum {
	FALSE = 0,
	TRUE = 1
} boolean;

#define STACK_SIZE 100

//updated by lexical analyser whenever \n is encountered
static int linenumber = 0;

#endif