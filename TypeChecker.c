/*
    Group 18

    Team Members:
    1. Madhav Gupta (2019A7PS0063P)
    2. Meenal Gupta (2019A7PS0243P)
    3. Pratham Gupta (2019A7PS0051P)
    4. Sankha Das (2019A7PS0029P)
    5. Yash Gupta (2019A7PS1138P)
*/

# include "TypeChecker.h"
#include "globalDef.h"
/* inputs:
 * the ast node that will serve as root for the subtree to traverse
 * pointer to the local function's symbol table or record table
 * pointer to the base symbol table
 *
 * output:
 * the type of the expression
 * or void for statement
 * or TypeErr for type error
 *
 * uses preorder and linkedlist traversal of ast subtree
 */
struct TypeArrayElement* findType(astNode* root, SymbolTable* localTable, SymbolTable* baseTable) {

	int numNodesinLinkedList;
	astNode *trav ;
	int error_flag;
	
	if (root == NULL){
		// to remove
		return voidPtr;
	}

	// check to see if the looping variables
	// in a while loop are changed in the while body
	if (root->type == While) {
		VariableVisitedNode* toVisitLL = NULL;
		toVisitLL = extractVariablesFromBoolean(root->children[0], toVisitLL);

		if (checkVariableChanges(root->children[1], toVisitLL) == FALSE) {
			printf("Error: Looping variables in while not changed.\n");
			return typeErrPtr;
		}
	}

	// for storing types of operands
	struct TypeArrayElement *t1, *t2, *t3;
	
	// to store the symbol table entry pointer for the identifier
	SymbolTableEntry* entry;

	if (root->isLeafNode) {
		switch (root->type) {
			/* INTEGER LITERAL
			 * <var> ===> TK_NUM
			 */
			case Num:
				// to remove
				return intPtr;
			
			/* REAL LITERAL
			 * <var> ===> TK_RNUM
			 */
			case RealNum:
				return realPtr;

			/* a VARIABLE IDENTIFIER */
			case VariableId:
			case Id:
				// to remove
				// check in function's local symbol table
				entry = lookupSymbolTable(localTable, root->entry.lexeme);
				// check in global table
				if (entry == NULL){
					entry = lookupSymbolTable(baseTable, root->entry.lexeme);
				}

				// variable probably undeclared - unaccessible
				if (entry == NULL) {
					printf("Line %d : Identifier %s unrecognized - undeclared or unaccessible.\n", root->entry.linenumber, root->entry.lexeme);
					return typeErrPtr;
				}

				return entry->type->type != UnionType ? entry->type : typeErrPtr;
				break;
			
			/* all other leaves assumed typeless
			 * TypeErr used only for erroneous leaves
			 */
			default:
				return voidPtr;
		}
	}

	// to remove
	switch (root->type) {
		/* RELATIONAL OPERATORS
		 * <booleanExpression> ===> <var> <relationalOp> <var> 
		 */
		case relOp_LT:
		case relOp_LE:
		case relOp_EQ:
		case relOp_GE:
		case relOp_GT:
			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[1], localTable, baseTable);
			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}
			if (t1->type == t2->type && (t1->type == Integer || t1->type == Real)){
				return booleanPtr;
			} else if (t1->type == RecordType && (t1 == t2)) {
				return booleanPtr;
			} else {
				printf("Line %d : Relational operator - incompatible operands.\n", root->children[0]->data->entry.linenumber);
				return typeErrPtr;
			}

		/* CONJUNCTION or DISJUNCTION
		 * <booleanExpression> ===> TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL
		 */
		case logOp_AND:
		case logOp_OR:

			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[1], localTable, baseTable);

			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}
			if (t1->type == Boolean && t2->type == Boolean){
				return booleanPtr;
			} else {
				printf("Line %d : AND/OR - boolean operands required.\n", root->children[0]->data->entry.linenumber);
				return typeErrPtr;
			}

		/* COMPLEMENTATION
		 * <booleanExpression> ===> TK_NOT TK_OP <booleanExpression> TK_CL
		 */
		case logOp_NOT:
			t1 = findType(root->children[0], localTable, baseTable);
			if (t1->type == Boolean){
				return booleanPtr;
			}
			if (t1->type == TypeErr) {
				return typeErrPtr;
			} 
			else {
				printf("Line %d : Complementation - boolean operands required.\n", root->children[0]->data->entry.linenumber);
				return typeErrPtr;
			}
		
		/* ADDITION or SUBTRACTION
		 * <arithmeticExpression> ===> <term> <expPrime>
		 * <expPrime> ===> <lowPrecedenceOperators> <term> <expPrime>1
		 */
		case arithOp_PLUS:
		case arithOp_MINUS:

			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[1], localTable, baseTable);

			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}
			if (t1->type == Integer && t2->type == Integer){
				return intPtr;
			}else if ((t1->type == Integer && t2->type == Real) || (t1->type == Real && t2->type == Integer) ){
			
				printf("Warning Line : Addition/Subtraction is unsafe between Real and Integer operands. Currently converting result to Real\n");
				return realPtr;
			
			}
			else if(t1->type == Real && t2->type == Real){	
				return realPtr;
			}else if (t1->type == RecordType){

				t3 =  checkTypeEquality(t1, t2);
				if(t3->type== TypeErr)
				{
					
					printf("Error: Addition/Subtraction - incompatible types %s and %s.\n",  t1->identifier, t2->identifier);
				}
				return t3;

			}
			else if(t1->type == Alias && t1->aliasTypeInfo->type == RecordType ) {
				
				t3 = checkTypeEquality(t1, t2);
				if(t3->type== TypeErr)
				{
					
					printf("Error: Addition/Subtraction - incompatible types %s and %s.\n",  t1->identifier, t2->identifier);
				}
				return t3;
			}
			 else if (t2->type == RecordType ){

				t3 = checkTypeEquality(t1, t2);
				if(t3->type== TypeErr)
				{
					
					printf("Error: Addition/Subtraction - incompatible types %s and %s.\n",  t1->identifier, t2->identifier);				
				}
				return t3;
			}
			else if(t2->type == Alias && t2->aliasTypeInfo->type == RecordType ) {
			
				t3 = checkTypeEquality(t1, t2);
				if(t3->type== TypeErr)
				{
					
					printf("Error: Addition/Subtraction - incompatible types %s and %s.\n",  t1->identifier, t2->identifier);
				
				}
				return t3;

			} else {
				printf("Line %d : Addition/Subtraction - incompatible types %s and %s.\n", root->children[0]->data->entry.linenumber, t1->identifier, t2->identifier);
				return typeErrPtr;
			}
			break;

		/* MULTIPLICATION
		 * <term> ===> <factor> <termPrime>
		 * <termPrime> ===> <highPrecedenceOperators> <factor> <termPrime>1
		 */
		case arithOp_MUL:
			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[2], localTable, baseTable);
			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}
			else if (t1->type == Integer && t2->type == Integer){
				return intPtr;
			}
			else if ((t1->type == Integer && t2->type == Real) || 
				(t1->type == Real && t2->type == Integer)) {
				printf("Warning : Multiplication is unsafe between Real and Integer operands. Currently converting result to Real\n");
				return realPtr;
			}
			else if(t1->type == Real && t2->type == Real){	
				return realPtr;}
			else if (t1->type == RecordType && t2->type == Integer) {
				return t1;
			}
			else if (t2->type == RecordType && t1->type == Integer) {
				return t2;
			} else {
				printf("Line %d : Multiplication - real, integer or record operands required.\n", root->children[0]->data->entry.linenumber);
				return typeErrPtr;
			}

		/* DIVISION
		 * <term> ===> <factor> <termPrime>
		 * <termPrime> ===> <highPrecedenceOperators> <factor> <termPrime>1
		 */
		case arithOp_DIV:
			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[2], localTable, baseTable);
			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}

			if (t1->type == Integer && t2->type == Integer){
				return realPtr;
			}
			else if ((t1->type == Integer && t2->type == Real) || (t1->type == Real && t2->type == Integer)) {
				printf("Warning : Division is unsafe between Real and Integer operands. Currently converting result to Real\n");
				return realPtr;
			}
			else if(t1->type == Real && t2->type == Real){	
				return realPtr;
			} else {
				printf("Line %d : Division - real or integer operands required.\n", root->children[0]->data->entry.linenumber);
				return typeErrPtr;
			}

		/* ASSIGNMENT
		 * <assignmentStmt> ===> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
		 */
		case AssignmentOperation:
			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[1], localTable, baseTable);

			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}

			if (t1->type == Integer && t2->type == Integer)
			{
				return voidPtr;
			}	
			else if (t1->type == Real && t2->type == Real)
			{
				return voidPtr;
			}
			else if (t1->type == RecordType){

				t3 =  checkTypeEquality(t1, t2);
				if(t3->type== TypeErr)
				{
					printf("Error: Assignment Error - type mismatch %s and %s.\n",  t1->identifier, t2->identifier);
				}
				return t3;

			}
			else if(t1->type == Alias && t1->aliasTypeInfo->type == RecordType ) {
				
				t3 = checkTypeEquality(t1, t2);
				if(t3->type== TypeErr)
				{
					printf("Error: Assignment Error - type mismatch %s and %s.\n",  t1->identifier, t2->identifier);
				}
				return t3;
			}
			 else if (t2->type == RecordType ){

				t3 = checkTypeEquality(t1, t2);
				if(t3->type== TypeErr)
				{
					printf("Error: Assignment Error - type mismatch %s and %s.\n",  t1->identifier, t2->identifier);
				}
				return t3;
			}
			else if(t2->type == Alias && t2->aliasTypeInfo->type == RecordType ) {
			
				t3 = checkTypeEquality(t1, t2);
				if(t3->type== TypeErr)
				{
					printf("Error: Assignment Error - type mismatch %s and %s.\n",  t1->identifier, t2->identifier);
				}
				return t3;

			} else {
				printf("Line %d: Assignment Error - type mismatch %s and %s.\n", root->children[0]->data->entry.linenumber, t1->identifier, t2->identifier);
				return typeErrPtr;
			}

		/* accessing VARIABLES or FIELDS of RECORDS
		 * <singleOrRecId> ===> TK_ID <optionSingleConstructed>
		 */
		case SingleOrRecIdLinkedListNode:

			
			entry = lookupSymbolTable(localTable, root->data->entry.lexeme);

			// next lookup in the global table
			if (entry == NULL)
				entry = lookupSymbolTable(baseTable, root->data->entry.lexeme);

			// entry not found - probably undeclared / out of scope
			if (entry == NULL) {
				printf("Variable %s undeclared or out of scope.\n", root->data->entry.lexeme);
				return typeErrPtr; // type undefined for undefined variable
			}

			// the variable is a primitive
			if (entry->type->type == Real || entry->type->type == Integer){
				if(root->next == NULL){
					return entry->type;
				}else{
					return typeErrPtr; // a primitive can not have fields
				}
			}
		
			if (root->next == NULL){
				return entry->type;
			}

			// variable is a record or a union
			if (entry->type->compositeVariableInfo != NULL) {
				return findTypeField(root->next, entry->type->compositeVariableInfo->listOfFields);
			} else {
				return typeErrPtr;
			}

		/* FUNCTION CALL STATEMENT
		 * <funCallStmt> ===> <outputParameters> TK_CALL TK_FUNID TK_WITH
         * TK_PARAMETERS <inputParameters> TK_SEM
		 */
		case FuncCall:
			// function identifier should be present in the symbol table
			entry = lookupSymbolTable(baseTable, root->children[1]->entry.lexeme);
			SymbolTableEntry* callingFunctionEntry = lookupSymbolTable(baseTable, localTable->tableID);

			if (entry == NULL || entry->isFunction == 0) {
				printf("Line %d : Function %s not defined.\n", root->children[1]->entry.linenumber, root->children[1]->entry.lexeme);
				return typeErrPtr;
			}

			// function call should not be recursive
			if (strcmp(localTable->tableID, root->children[1]->entry.lexeme) == 0) {
				printf("Line %d : Recursion not allowed!\n", root->children[1]->entry.linenumber);
				return typeErrPtr;
			}


			// information about the input and output parameters
			// IMPORTANT: please ensure that this is not NULL for a function
			struct FunctionType* funcInfo = entry->type->functionInfo;
			struct FunctionType* callingFuncInfo = callingFunctionEntry->type->functionInfo;
			
			if(callingFuncInfo->declarationSeqNum < funcInfo->declarationSeqNum){

				printf("Line %d : function %s should be defined before %s\n", root->children[1]->entry.linenumber, funcInfo->identifier, callingFuncInfo->identifier);
				return typeErrPtr;
			}
			

			if (funcInfo == NULL) {
				printf("Erroneous function entry in symbol table.\n");
				return typeErrPtr;
			}

			// input parameters linked list from function prototype
			struct FunctionParameter* formalInput = funcInfo->inputParameters;
			// output parameters linked list from function prototype
			struct FunctionParameter* formalOutput = funcInfo->outputParameters;
		
			
			// actual input and output parameters used
			astNode* actualInput = root->children[2];
			astNode* actualOutput = root->children[0];

			int lenActualInput = findLengthActual(actualInput);
			int lenFormallInput = findLengthFormal(formalInput);
			int lenActualOutput = findLengthActual(actualOutput);
			int lenFormalOutput = findLengthFormal(formalOutput);
			
			int errorFlag = 0;
			if (lenActualInput != lenFormallInput){
				errorFlag = 1;
				printf("Line %d : function %s requires %d input parameters, but %d supplied\n", root->children[1]->entry.linenumber, root->children[1]->entry.lexeme, lenFormallInput, lenActualInput);
			}

			if (lenActualOutput != lenFormalOutput){
				errorFlag = 1;
				printf("Line %d : function %s has %d output parameters, but %d seen\n", root->children[1]->entry.linenumber, root->children[1]->entry.lexeme, lenFormalOutput, lenActualOutput);
			}

			if(errorFlag){
				return typeErrPtr;
			}

			int actualInputLen = findLengthActual(actualInput),
				actualOutputLen = findLengthActual(actualOutput);
			for (int i = 0; i < actualInputLen; i++) {
				t1 = findType(actualInput->data, localTable, baseTable);

				t2 = formalInput->datatype;
				
				if (t1->type == TypeErr || t2->type == TypeErr) {
					return typeErrPtr;
				}

				// passing union parameters not allowed1
				if (t1->type == UnionType || t2->type == UnionType){
					printf("Line %d : Input parameter cannot be of type union\n", root->children[1]->entry.linenumber);
					return typeErrPtr;
				}

				// parameters should match in type
				if (checkTypeEquality(t1, t2)->type == TypeErr) {
					printf("Line %d : Input parameter mismatch for %s of %s - expected %s but got %s.\n", root->children[1]->entry.linenumber, formalInput->identifier, funcInfo->identifier, t2->identifier, t1->identifier);
					return typeErrPtr;
				}

				actualInput = actualInput->next;
				formalInput = formalInput->next;
			}

			// checking the output parameters
			for (int i = 0; i < actualOutputLen; i++) {
				t1 = findType(actualOutput->data, localTable, baseTable);

				t2 = formalOutput->datatype;

				if (t1->type == TypeErr || t2->type == TypeErr) {
					return typeErrPtr;
				}

				// passing union parameters not allowed
				if (t1->type == UnionType || t2->type == UnionType){
					printf("Line %d : Output parameter cannot be of type union\n", root->children[1]->entry.linenumber);
					return typeErrPtr;
				}

				// parameters should match in type
				if (checkTypeEquality(t1, t2)->type == TypeErr) {
					printf("%s: Output parameter mismatch.\n", formalOutput->identifier);
					return typeErrPtr;
				}
				actualOutput = actualOutput->next;
				formalOutput = formalOutput->next;
			}

			return voidPtr;
			break;

		case Return: {
			SymbolTableEntry* currFuncEntry = lookupSymbolTable(baseTable, localTable->tableID);

			// information about the input and output parameters
			struct FunctionType* currFuncInfo = currFuncEntry->type->functionInfo;

			if (currFuncInfo == NULL) {
				printf("Erroneous function entry in symbol table.\n");
				return typeErrPtr;
			}

			
			// output parameters linked list from function prototype
			struct FunctionParameter* expectedOutput = currFuncInfo->outputParameters;

			// idList for the output vars the function is returning
			astNode* returnList = root->children[0];
			
			int expectedOutputLen = findLengthFormal(expectedOutput);
			int returnListLen = findLengthActual(returnList);
			
			if (expectedOutputLen != returnListLen){
				printf("Function %s should return %d values, but returns %d values.\n", 
						currFuncEntry->identifier, expectedOutputLen, returnListLen);
				return typeErrPtr;
			}

			for (int i = 0; i < returnListLen; i++) {
				t1 = findType(returnList->data, localTable, baseTable);

				t2 = expectedOutput->datatype;

				if (t1->type == TypeErr || t2->type == TypeErr) {
					return typeErrPtr;
				}

				// passing union parameters not allowed
				if (t1->type == UnionType || t2->type == UnionType){
					printf("Line %d : Passing unions not allowed.\n", 
							root->children[1]->entry.linenumber);
					return typeErrPtr;
				}

				// parameters should match in type
				if (checkTypeEquality(t1, t2)->type == TypeErr) {
					printf("%s: Mismatch in the return list.\n", expectedOutput->identifier);
					return typeErrPtr;
				}
				expectedOutput = expectedOutput->next;
				returnList = returnList->next;
			}
			
			return voidPtr;
			break;
		}

		default:
			/* advance ahead on the linkedlist to ensure it
			 * is completely type checked
			 */
			if (root->isLinkedListNode) {
		
				trav = root;
				error_flag =0;
				while(trav!= NULL){
					
				// earlier root->next
					if (trav->data != NULL) {
						if (findType(trav->data, localTable, baseTable)->type == TypeErr)
								{
									error_flag++;
								}
						
					}
					trav = trav->next;
				}
				
				if(error_flag) return typeErrPtr;
				else return voidPtr;
			}
				
			else {
				/* type check all the children */

				error_flag =0;
				int children = MAX_PROD_LEN;
				while (children > 0 && root->children[children-1] == NULL){
					children--;
				}
				// to remove
				boolean childErrFlag = FALSE;
				for (int i = 0; i < children; i++){

					// to remove
					boolean flag = FALSE;
					if (findType(root->children[i], localTable, baseTable)->type == TypeErr){
						childErrFlag = TRUE;
					}
				}
				return (childErrFlag == TRUE) ? typeErrPtr : voidPtr;
			}
	}
}

/* inputs:
 * ast node for field
 * a linked list containing the fields in the record
 * basetable
 */

struct TypeArrayElement* findTypeField(astNode* root, struct Field* fieldLL) {
	// control flow should not reach this point otherwise
	if (root == NULL || fieldLL == NULL) {
		printf("Field not found.\n");
		return typeErrPtr;
	}
	
	struct Field* fieldSelected = searchInFieldLL(root->data->entry.lexeme, fieldLL);

	if (fieldSelected == NULL) {
		printf("Field not found.\n");
		return typeErrPtr;
	}

	
	/* FIELD of a CONSTRUCTED DATATYPE
	 * <optionSingleConstructed> ===> <oneExpansion> <moreExpansions>
	 */
	if (root->type == FieldIdLinkedListNode) {
		struct TypeArrayElement* entryType = fieldSelected->datatype;
			
		if (entryType == NULL) {
			printf("Field not found.\n");
			return typeErrPtr; // type undefined for undefined variable
		}

		// the field is a primitive
		if (entryType->type == Real || entryType->type == Integer){
			if (root->next == NULL){
				return entryType;
			}else{
				return typeErrPtr; // a primitive can not have fields
			}
		}

		if (root->next == NULL){
			return entryType;
		}

		/* replace the localTable here with the mini-table for record */
		if (entryType->compositeVariableInfo != NULL) {
			return findTypeField(root->next, entryType->compositeVariableInfo->listOfFields);
		} else {
			
			return typeErrPtr;
		}
	}

	// not supposed to be here otherwise according to AST
	return typeErrPtr;
}

struct Field* searchInFieldLL(char* fieldLexeme, struct Field* fieldLL) {
	Field* curr = fieldLL;
	while (curr != NULL) {
		if (strcmp(curr->identifier, fieldLexeme) == 0){
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

/* inputs:
 * a root node - the Prog ast node
 * pointer to the base symbol table
 * */
int typeCheck(astNode* root, SymbolTable* baseTable) {
	/* <program> ===> <otherFunctions> <mainFunction>*/
	if (root->type != Program) {
		printf("AST should start at Program.");
		return -1;
	}

	SymbolTable* localTable = NULL;

	/* traverse <otherFunctions> */
	astNode* funcNode = root->children[0];
	while (funcNode != NULL) {
		// name of the function
		char* funcLexeme = funcNode->data->children[0]->entry.lexeme;

		// obtain the symbol table for the current function
		printf("Entering Function: %s...\n\n\n", funcLexeme);
		SymbolTableEntry* currFunc = lookupSymbolTable(baseTable, funcLexeme);
		localTable = currFunc->tablePointer;

		Type elem = findType(funcNode->data->children[3], localTable, baseTable)->type;

		if (elem != Void) {
			printf("\nTYPE ERRORS DETECTED IN %s.\n\n\n", funcLexeme);
		}

		struct VariableVisitedNode* visitOutParLL = NULL;
		struct VariableVisitedNode* headOutParLL = NULL;

		FunctionParameter* outputParLL = currFunc->type->functionInfo->outputParameters;

		while (outputParLL != NULL) {
			struct VariableVisitedNode* newNode = 
				(VariableVisitedNode*) malloc(sizeof(VariableVisitedNode));
				newNode->lexeme = (char*) malloc(strlen(outputParLL->identifier)*sizeof(char));
				strcpy(newNode->lexeme, outputParLL->identifier);
				newNode->visited = FALSE;
				newNode->next = NULL;

				if (visitOutParLL == NULL) {
					visitOutParLL = newNode;
					headOutParLL = newNode;
				}
				else {
					headOutParLL->next = newNode;
					headOutParLL = newNode;
				}
				outputParLL = outputParLL->next;
		}

		// traverse the function body and mark all output parameters visited
		markVariableChanges(funcNode->data->children[3], visitOutParLL);
		
		headOutParLL = visitOutParLL;
		while (headOutParLL != NULL) {
			if (headOutParLL->visited == FALSE) {
				printf("Function %s: does not update output parameter - %s.\n", 
					funcLexeme, headOutParLL->lexeme);
			}
			headOutParLL = headOutParLL->next;
		}
		funcNode = funcNode->next;
	}

	/* traverse <mainFunction> */
	char *mainLexeme = MAIN_NAME;
	printf("Entering Function: %s...\n\n\n", mainLexeme);
	localTable = (lookupSymbolTable(baseTable, mainLexeme))->tablePointer;
	if (findType(root->children[1]->children[0], localTable, baseTable)->type != Void) {
		printf("\nTYPE ERROR DETECTED IN MAIN.\n\n\n");
		return -1;
	}

	printf("TYPE CHECKING SUCCESSFUL - ALL TESTS CLEARED!\n");
	return 0;
}

/* find the length of actual parameters linked list */
int findLengthActual(astNode* head) {
	astNode* curr = head;
	int count = 0;
	while (curr != NULL) {
		count++;
		curr = curr->next;
	}
	return count;
}

/* find the length of formal parameters linked list */
int findLengthFormal(FunctionParameter* head) {
	FunctionParameter* curr = head;
	int count = 0;
	while (curr != NULL) {
		count++;
		curr = curr->next;
	}
	return count;
}

/* useful for checking record type equality */
struct TypeArrayElement* checkTypeEquality(struct TypeArrayElement* t1, struct TypeArrayElement* t2) {

	if (t1 == typeErrPtr || t2 == typeErrPtr)
		return typeErrPtr;
	if (t1 == t2) {
		return t1;
	}
	else if (t1->aliasTypeInfo != NULL && t1->aliasTypeInfo == t2)
		return t2;
	else if (t2->aliasTypeInfo != NULL && t1 == t2->aliasTypeInfo)

		return t1;
	else if (t1->aliasTypeInfo != NULL && t1->aliasTypeInfo == t2->aliasTypeInfo)
		return t1->aliasTypeInfo;

	return typeErrPtr;
}

VariableVisitedNode* extractVariablesFromBoolean(astNode* root, VariableVisitedNode* toVisitLL) {
	if (root == NULL)
		return toVisitLL;
	switch(root->type) {
		case SingleOrRecIdLinkedListNode:
			toVisitLL = extractVariablesFromBoolean(root->data, toVisitLL);
			break;

		case VariableId:
		case Id:
			if (toVisitLL == NULL) {
				VariableVisitedNode* newNode = (VariableVisitedNode*) malloc(sizeof(VariableVisitedNode));

				newNode->lexeme = (char*) malloc(strlen(root->entry.lexeme)*sizeof(char));
				strcpy(newNode->lexeme, root->entry.lexeme);
				newNode->visited = FALSE;
				newNode->next = NULL;

				toVisitLL = newNode;
			}

			else {
				VariableVisitedNode* newNode = 
					(VariableVisitedNode*) malloc(sizeof(VariableVisitedNode));
				VariableVisitedNode* curr = toVisitLL;
				while (curr->next != NULL)
					curr = curr->next;
				curr->next = newNode;

				newNode->lexeme = (char*) malloc(strlen(root->entry.lexeme)*sizeof(char));
				strcpy(newNode->lexeme, root->entry.lexeme);
				newNode->visited = FALSE;
				newNode->next = NULL;
			}
			break;
		
		case logOp_AND:
		case logOp_OR:
			toVisitLL = extractVariablesFromBoolean(root->children[0], toVisitLL);
			toVisitLL = extractVariablesFromBoolean(root->children[1], toVisitLL);
			break;

		case relOp_EQ:
		case relOp_GE:
		case relOp_GT:
		case relOp_LE:
		case relOp_LT:
			toVisitLL = extractVariablesFromBoolean(root->children[0], toVisitLL);
			toVisitLL = extractVariablesFromBoolean(root->children[1], toVisitLL);

		case arithOp_DIV:
		case arithOp_MINUS:
		case arithOp_PLUS:
		case arithOp_MUL:
			toVisitLL = extractVariablesFromBoolean(root->children[0], toVisitLL);
			toVisitLL = extractVariablesFromBoolean(root->children[1], toVisitLL);
			break;

		case logOp_NOT:
			extractVariablesFromBoolean(root->children[0], toVisitLL);
			break;
		
		default:
			break;
	}

	return toVisitLL;
}

boolean checkVariableChanges(astNode* root, VariableVisitedNode* toVisitLL) {
	markVariableChanges(root, toVisitLL);
	VariableVisitedNode* curr = toVisitLL;
	while (curr != NULL) {
		if (curr->visited == TRUE) {
 			return TRUE;
		}
		curr = curr->next;
	}
	return FALSE;
}

boolean markVariableChanges(astNode* root, VariableVisitedNode* toVisitLL) {
	VariableVisitedNode *curr, *toVisitLLNested;
	astNode* actualOutput;
	char* lhsName;
	astNode* readVariable;

	if (root == NULL) {
		return FALSE;
	}

	switch (root->type) {
		case While:
			toVisitLLNested = NULL;
			toVisitLLNested = extractVariablesFromBoolean(root->children[0], toVisitLLNested);

			if (checkVariableChanges(root->children[1], toVisitLLNested) == FALSE) {
				return FALSE;
			}
			return markVariableChanges(root->children[1], toVisitLL);

		case AssignmentOperation:
			lhsName = root->children[0]->data->entry.lexeme;
			curr = toVisitLL;
			while (curr != NULL) {
				if (strcmp(curr->lexeme, lhsName) == 0) {
					curr->visited = TRUE;
					return TRUE;
				}
				curr = curr->next;
			}
			return FALSE;
		
		case FuncCall:
			actualOutput = root->children[0];
			int actualOutputLen = findLengthActual(actualOutput);
			
			for (int i = 0; i < actualOutputLen; i++) {
				char* actualName = actualOutput->data->entry.lexeme;
				curr = toVisitLL;
				while (curr != NULL) {
					if (strcmp(curr->lexeme, actualName) == 0) {
						curr->visited = TRUE;
						return TRUE;
					}
					curr = curr->next;
					
				}
				actualOutput = actualOutput->next;
			}
			return FALSE;
		
		case Read:
			readVariable = root->children[0];
			if (readVariable->type == Num || readVariable->type == RealNum) {
				printf("Can not read into a literal!\n");
				return FALSE;
			}
			lhsName = readVariable->data->entry.lexeme;
			
			curr = toVisitLL;
			while (curr != NULL) {
				if (strcmp(curr->lexeme, lhsName) == 0) {
					curr->visited = TRUE;
					return TRUE;
				}
				curr = curr->next;
			}
			return FALSE;
		
		case If: {
			boolean flag = FALSE;
			if (markVariableChanges(root->children[1], toVisitLL) == TRUE) {
				flag = TRUE;
			}
			if (markVariableChanges(root->children[2], toVisitLL) == TRUE) {
				flag = TRUE;
			}
			return flag;
		}

		default:
			if (root->isLinkedListNode) {
				boolean flag = FALSE;
				// earlier root->next
				if (root->data != NULL) {
					if (markVariableChanges(root->data, toVisitLL) == TRUE)
						flag = TRUE;
				}
				if (root->next == NULL){
					return FALSE;
				}

				return markVariableChanges(root->next, toVisitLL) || flag;
			} 
			else {
				/* visit all the children */
				int children = MAX_PROD_LEN;
				while (children > 0 && root->children[children-1] == NULL){
					children--;
				}
				// to remove
				boolean flag = FALSE;
				for (int i = 0; i < children; i++){
					// to remove
					if (markVariableChanges(root->children[i], toVisitLL) == TRUE){
						flag = TRUE;
					}
				}
				return flag;
			}
			return FALSE;
	}
}