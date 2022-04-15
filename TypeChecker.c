
# include "TypeChecker.h"
#include "globalDef.h"
/* inputs:
 * the ast node that will serve as root for the subtree to traverse
 * pointer to the local function's symbol table or record table
 * pointer to symbol table of global vars
 * pointer to the base symbol table (listOfSymbolTables)
 *
 * output:
 * the type of the expression
 * or void for statement
 * or TypeErr for type error
 *
 * uses preorder and linkedlist traversal of ast subtree
 */
struct TypeArrayElement* findType(astNode* root, SymbolTable* localTable, SymbolTable* baseTable) {


	astNode *trav ;
	int error_flag;
	
	if (root == NULL){
		// to remove
		//printf("NULL node\n");
		return voidPtr;
	}

	// check to see if the looping variables
	// in a while loop are changed in the while body
	if (root->type == While) {
		VariableVisitedNode* toVisitLL = NULL;
		toVisitLL = extractVariablesFromBoolean(root->children[0], toVisitLL);

		VariableVisitedNode* head = toVisitLL;
		while (head != NULL) {
			head = head->next;
		}

		if (checkVariableChanges(root->children[1], toVisitLL) == FALSE) {
			printf("Looping variables in while not changed.\n");
			return typeErrPtr;
		}else{
			printf("Something was changed...\n");
		}
	}

	// for storing types of operands
	struct TypeArrayElement *t1, *t2;
	
	// to store the symbol table entry pointer for the identifier
	SymbolTableEntry* entry;

	if (root->isLeafNode) {
		switch (root->type) {
			/* INTEGER LITERAL
			 * <var> ===> TK_NUM
			 */
			case Num:
				// to remove
				//printf("Entered.\n");
				return intPtr;
			
			/* REAL LITERAL
			 * <var> ===> TK_RNUM
			 */
			case RealNum:
				return realPtr;

			// likely redundant case
			/* a VARIABLE IDENTIFIER */
			case VariableId:
			case Id:
				// to remove
				//printf("Entered Id.");
				// check in function's local symbol table
				entry = lookupSymbolTable(localTable, root->entry.lexeme);
				//printf("%s %s\n", entry->identifier, entry->type->identifier);
				// check in global table
				if (entry == NULL){
					entry = lookupSymbolTable(baseTable, root->entry.lexeme);
				}

				// variable probably undeclared - unaccessible
				if (entry == NULL) {
					printf("Identifier unrecognized - undeclared or unaccessible.\n");
					return typeErrPtr;
				}

				//printf("Exiting id\n");
				return entry->type->type != Union ? entry->type : typeErrPtr;
				break;
			
			/* all other leaves assumed typeless
			 * TypeErr used only for erroneous leaves
			 */
			default:
				return voidPtr;
		}
	}

	// to remove
	//printf("%d", root->type);
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
			t2 = findType(root->children[2], localTable, baseTable);
			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}
			if (t1->type == t2->type && (t1->type == Integer || t1->type == Real)){
				return t1;
			} else if (t1->type == Record && (t1 == t2)) {
				return t1;
			} else {
				printf("Relational operator: incompatible operands.\n");
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
				printf("And/or: boolean operands required.\n");
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
				printf("Complementation: boolean operand required.\n");
				return typeErrPtr;
			}
		
		/* ADDITION or SUBTRACTION
		 * <arithmeticExpression> ===> <term> <expPrime>
		 * <expPrime> ===> <lowPrecedenceOperators> <term> <expPrime>1
		 */
		case arithOp_PLUS:
		case arithOp_MINUS:
			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[2], localTable, baseTable);
			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}
			if (t1->type == Integer && t2->type == Integer){
				return intPtr;
			}else if ((t1->type == Integer && t2->type == Real) || (t1->type == Real && t2->type == Integer) || (t1->type == Real && t2->type == Real)){
				return realPtr;
			} else if (t1->type == Record) {
				return checkTypeEquality(t1, t2);
			} else {
				printf("Addition/Subtraction: real or integer operands required.\n");
				return typeErrPtr;
			}

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
				(t1->type == Real && t2->type == Integer) || 
				(t1->type == Real && t2->type == Real)) {
				return realPtr;
			}
			else if (t1->type == Record && t2->type == Integer) {
				return t1;
			}
			else if (t2->type == Record && t1->type == Integer) {
				return t2;
			}
			else if (t1->type == Record) {
				return checkTypeEquality(t1, t2);
			}
			else {
				printf("Multiplication: real or integer operands required.\n");
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
			if ((t1->type == Integer || t1->type == Real) && (t2->type == Integer || t2->type == Real)){
				return realPtr;
				//else if (t1->type == Record)
				//	return checkTypeEquality(t1, t2);
			} else {
				printf("Division: real or integer operands required.\n");
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
			/*
			if ((t1->type == Integer && t2->type == Integer) || (t1->type == Real && (t2->type == Integer || t2->type == Real))){
				return voidPtr;
				//TO DO: assignment for whole records?
			}
			*/
			if (t1->type == Integer && t2->type == Integer)
				return voidPtr;
			else if (t1->type == Real && t2->type == Real)
				return voidPtr;
			else if (t1->type == Record) {
				return checkTypeEquality(t1, t2);
			} else {
				printf("Assignment: incompatible types.\n");
				return typeErrPtr;
			}

		/* accessing VARIABLES or FIELDS of RECORDS
		 * <singleOrRecId> ===> TK_ID <optionSingleConstructed>
		 */
		case SingleOrRecIdLinkedListNode:
			// lookup in the most local scope - scope of function
			entry = lookupSymbolTable(localTable, root->data->entry.lexeme);

			// next lookup in the global table
			if (entry == NULL)
				entry = lookupSymbolTable(baseTable, root->data->entry.lexeme);

			// entry not found - probably undeclared / out of scope
			if (entry == NULL) {
				printf("Variable undeclared or out of scope.\n");
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
				printf("Empty composite!\n");
				return typeErrPtr;
			}

		/* FUNCTION CALL STATEMENT
		 * <funCallStmt> ===> <outputParameters> TK_CALL TK_FUNID TK_WITH
         * TK_PARAMETERS <inputParameters> TK_SEM
		 */
		case FuncCall:
			// function identifier should be present in the symbol table
			entry = lookupSymbolTable(baseTable, root->children[1]->entry.lexeme);
			//printf("%s\n", root->children[1]->entry.lexeme);
			//printf("%s\n", baseTable->tableID);
			
			if (entry == NULL || entry->isFunction == 0) {
				printf("Function not defined.\n");
				return typeErrPtr;
			}

			// function call should not be recursive
			if (strcmp(localTable->tableID, root->children[1]->entry.lexeme) == 0) {
				printf("Recursion not allowed!\n");
				return typeErrPtr;
			}



			// information about the input and output parameters
			// IMPORTANT: please ensure that this is not NULL for a function
			struct FunctionType* funcInfo = entry->type->functionInfo;

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
			
			if (findLengthActual(actualInput) != findLengthFormal(formalInput)
					|| findLengthActual(actualOutput) != findLengthFormal(formalOutput)) {
				printf("Number of parameters in function call do not match.\n");
				return typeErrPtr;
			}

			int actualInputLen = findLengthActual(actualInput),
				actualOutputLen = findLengthActual(actualOutput);
			for (int i = 0; i < actualInputLen; i++) {
				//printf("Entered input loop.\n");
				t1 = findType(actualInput->data, localTable, baseTable);
				//printf("Returned back to funct loop.\n");
				t2 = formalInput->datatype;
				
				if (t1->type == TypeErr || t2->type == TypeErr) {
					return typeErrPtr;
				}

				// passing union parameters not allowed
				if (t1->type == Union || t2->type == Union){
					return typeErrPtr;
				}

				// parameters should match in type
				if (checkTypeEquality(t1, t2)->type == TypeErr) {
					printf("%s %s\n", t1->identifier, t2->identifier);
					printf("%s: Input parameter mismatch.\n", formalInput->identifier);
					return typeErrPtr;
				}
				/*else {
					printf("Type correct.\n");
				}*/
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
				if (t1->type == Union || t2->type == Union){
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

				int children = MAX_PROD_LEN;
				while (children > 0 && root->children[children-1] == NULL){
					children--;
				}
				// to remove
				//printf("%d\n", children);
				for (int i = 0; i < children; i++){
					// to remove
					//printf("-%d-", root->children[i]->type);
					if (findType(root->children[i], localTable, baseTable)->type == TypeErr){
						return typeErrPtr;
					}
				}
				return voidPtr;
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

	struct Field* fieldSelected = searchInFieldLL(root->entry.lexeme, fieldLL);

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
			printf("Empty record!\n");
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
		printf("Entering Function: %s...\n", funcLexeme);
		SymbolTableEntry* currFunc = lookupSymbolTable(baseTable, funcLexeme);
		localTable = currFunc->tablePointer;

		if (findType(funcNode->data, localTable, baseTable)->type != Void) {
			printf("TYPE ERROR DETECTED IN %s.\n", funcLexeme);
			return -1;
		}

		funcNode = funcNode->next;
	}

	/* traverse <mainFunction> */
	char *mainLexeme = MAIN_NAME;
	printf("Entering Function: %s...\n", mainLexeme);
	localTable = (lookupSymbolTable(baseTable, mainLexeme))->tablePointer;
	if (findType(root->children[1], localTable, baseTable)->type != Void) {
		printf("TYPE ERROR DETECTED IN MAIN.\n");
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
struct TypeArrayElement* checkTypeEquality(struct TypeArrayElement* t1, 
		struct TypeArrayElement* t2) {
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
		return NULL;
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

				//printf("%s\n", newNode->lexeme);
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
				//printf("%s-%s\n", toVisitLL->lexeme, newNode->lexeme);
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
		case arithOp_DIV:
		case arithOp_MINUS:
		case arithOp_PLUS:
		case arithOp_MUL:
			//printf("In operator\n");
			toVisitLL = extractVariablesFromBoolean(root->children[0], toVisitLL);
			toVisitLL = extractVariablesFromBoolean(root->children[2], toVisitLL);
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
	printf("in check changes\n");
	if (markVariableChanges(root, toVisitLL) == FALSE)
		return FALSE;
	printf("Here again in check\n");
	return TRUE;
	// VariableVisitedNode* curr = toVisitLL;
	// while (curr != NULL) {
	// 	if (curr->visited == TRUE)
	// 		return TRUE;
	// 	curr = curr->next;
	// }
	// return FALSE;
}

boolean markVariableChanges(astNode* root, VariableVisitedNode* toVisitLL) {
	printf("in mark changes\n");

	VariableVisitedNode *curr, *toVisitLLNested;
	astNode* actualOutput;
	char* lhsName;
	astNode* readVariable;

	printf("Statement type: %s\n", getStatmType(root->type));

	switch (root->type) {
		case While:
			toVisitLLNested = NULL;
			extractVariablesFromBoolean(root->children[0], toVisitLLNested);
			if (checkVariableChanges(root->children[1],
											toVisitLLNested) == FALSE) {
				return FALSE;
			}
			printf("Here in default 0\n");
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
		
		case If:
			printf("here in if.\n");
			if (markVariableChanges(root->children[1], toVisitLL) == TRUE) {
				printf("if first statement.\n");
				return TRUE;
			}
			if (markVariableChanges(root->children[2], toVisitLL) == TRUE) {
				printf("if other statements\n");
				return TRUE;
			}
			return FALSE;

		default:
			if (root->isLinkedListNode) {
				// earlier root->next
				if (root->data != NULL) {
					printf("Here in default 1\n");
					printf("ASTNode var: %s\n", root->data->entry.lexeme);
					if (markVariableChanges(root->data, toVisitLL) == TRUE)
						return TRUE;
				}
				if (root->next == NULL){
					printf("Here in defAULT 2\n");
					return FALSE;
				}

				printf("Here in defAULT 4\n");
				return markVariableChanges(root->next, toVisitLL);
			} 
			else {
				/* visit all the children */
				printf("Here in default 3\n");
				int children = MAX_PROD_LEN;
				while (children > 0 && root->children[children-1] == NULL){
					children--;
				}
				// to remove
				printf("%d\n", children);
				for (int i = 0; i < children; i++){
					// to remove
					//printf("-%d-", root->children[i]->type);
					printf("ASTNode var: %s\n", root->children[i]->entry.lexeme);
					if (markVariableChanges(root->children[i], toVisitLL) == TRUE){
						return TRUE;
					}
				}
				return FALSE;
			}
			return FALSE;
	}
}