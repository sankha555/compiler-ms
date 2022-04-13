# include "SemanticAnalyzer.h"
# include "TypeChecker.h"

/* 0: success
 * -1: failure
 * */
int semanticAnalyzer(astNode* root) {
	/* checks done :
	 * identifier cannot be declared multiple times in same scope
	 * global identifier cannot be declared again
	 * function overloading not allowed
	 */
	// populate the symbol table
	SymbolTable* baseTable = NULL; // get the base symbol table here

	/* checks done:
	 * type checking of variables, records, and expressions
	 * occuring in expressions and statements
	 * identifier declared before it is used
	 *
	 * function call statements
	 * correct number and types of parameters
	 * no recursion allowed
	 *
	 * 
	 */
	int typeFlag = typeCheck(root, baseTable);

	/* still left:
	 * while loop changes the looping variable
	 * support for variant records
	 */
	
	return typeFlag;
}
