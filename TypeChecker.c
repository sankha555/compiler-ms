
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
		//printf("Root is Null\n");
		return voidPtr;
	}

	//if (root->type == Return ) printf("return!\n");


	// check to see if the looping variables
	// in a while loop are changed in the while body
	
	if (root->type == While) {
		VariableVisitedNode* toVisitLL = NULL;
		toVisitLL = extractVariablesFromBoolean(root->children[0], toVisitLL);

		/*
		VariableVisitedNode* head = toVisitLL;
		while (head != NULL) {
			printf("to visit: %s\n", head->lexeme);
			head = head->next;
		}
		*/
		

		if (checkVariableChanges(root->children[1], toVisitLL) == FALSE) {
			printf("Error: Looping variables in while not changed.\n");
			return typeErrPtr;
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
				//printf("Entered Num.\n");
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
				//printf("Entered Id.\n");
				// check in function's local symbol table
				entry = lookupSymbolTable(localTable, root->entry.lexeme);
				//printf("%s %s\n", entry->identifier, entry->type->identifier);
				// check in global table
				if (entry == NULL){
					//printf("The entry was not found in Symbol Table\n");
					entry = lookupSymbolTable(baseTable, root->entry.lexeme);
				}

				// variable probably undeclared - unaccessible
				if (entry == NULL) {
					printf("Line %d : Identifier %s unrecognized - undeclared or unaccessible.\n", root->entry.linenumber, root->entry.lexeme);
					return typeErrPtr;
				}
				//else printf("%s %s\n",root->entry.lexeme,entry->type->identifier );

				//printf("Exiting id\n");
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
			//printf("Entered Relational Operators.\n");
			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[1], localTable, baseTable);
			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}
			if (t1->type == t2->type && (t1->type == Integer || t1->type == Real)){
				return booleanPtr;
			}
			else if ((t1->type == VariantRecord && t2->type == VariantRecord)
					|| (t1->type == VariantRecord && (t2->type == Integer || t2->type == Real))
					|| (t2->type == VariantRecord && (t1->type == Integer || t1->type == Real))) {
				return booleanPtr;
			}
			else if (t1->type == RecordType && (t1 == t2)) {
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
			//printf("Entered Logical Operators.\n");

			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[1], localTable, baseTable);
			//printf("logical print: %s %s\n", t1->identifier, t2->identifier);
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
			//printf("Entered Logical Not.\n");
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
			//for(int i=-0;i<10;i++ )printf("Entered Arithmetic Plus/Minus.");
			//printf("\n");

			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[1], localTable, baseTable);
			//printf("%d", t1->type);
			//printf("Hello! :%s %s\n", t1->identifier, t2->identifier);
			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}
			if (t1->type == Integer && t2->type == Integer){
				return intPtr;
			}
			else if ((t1->type == Integer && t2->type == Real) || (t1->type == Real && t2->type == Integer) || (t1->type == Real && t2->type == Real)){
				return realPtr;
<<<<<<< HEAD
			}
			else if ((t1->type == VariantRecord && t2->type == VariantRecord)
					|| (t1->type == VariantRecord && (t2->type == Integer || t2->type == Real))
					|| (t2->type == VariantRecord && (t1->type == Integer || t1->type == Real))) {
				return variantRecordPtr;
			}
			else if (t1->type == RecordType) {
=======
			} else if (t1->type == RecordType || (t1->type == Alias && t1->aliasTypeInfo->type == RecordType)) {
>>>>>>> 8670500be5b95092eefc5dda3504b96acc6c8569
				//printf("entered here ...%s %s\n", t1->identifier, t2->identifier);
				return checkTypeEquality(t1, t2);
			} else {
				printf("Line %d : Addition/Subtraction - real or integer operands required.\n", root->children[0]->data->entry.linenumber);
				return typeErrPtr;
			}

		/* MULTIPLICATION
		 * <term> ===> <factor> <termPrime>
		 * <termPrime> ===> <highPrecedenceOperators> <factor> <termPrime>1
		 */
		case arithOp_MUL:
			//printf("Entered Arithmetic Multiply.\n");
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
			else if ((t1->type == VariantRecord && t2->type == VariantRecord)
					|| (t1->type == VariantRecord && (t2->type == Integer || t2->type == Real))
					|| (t2->type == VariantRecord && (t1->type == Integer || t1->type == Real))) {
				return variantRecordPtr;
			}
			else if (t1->type == RecordType && t2->type == Integer) {
				return t1;
			}
			else if (t2->type == RecordType && t1->type == Integer) {
				return t2;
			}
			/*
			else if (t1->type == RecordType) {
				return checkTypeEquality(t1, t2);
			}
			*/
			else {
				printf("Line %d : Multiplication - real, integer or record operands required.\n", root->children[0]->data->entry.linenumber);
				return typeErrPtr;
			}

		/* DIVISION
		 * <term> ===> <factor> <termPrime>
		 * <termPrime> ===> <highPrecedenceOperators> <factor> <termPrime>1
		 */
		case arithOp_DIV:
			//printf("Entered Arithmetic Division.\n");
			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[2], localTable, baseTable);
			if (t1->type == TypeErr || t2->type == TypeErr) {
				return typeErrPtr;
			}
			if ((t1->type == Integer || t1->type == Real) && (t2->type == Integer || t2->type == Real)){
				return realPtr;
				//else if (t1->type == RecordType)
				//	return checkTypeEquality(t1, t2);
			}
			else if ((t1->type == VariantRecord && t2->type == VariantRecord)
					|| (t1->type == VariantRecord && (t2->type == Integer || t2->type == Real))
					|| (t2->type == VariantRecord && (t1->type == Integer || t1->type == Real))) {
				return variantRecordPtr;
			}
			else {
				printf("Line %d : Division - real or integer operands required.\n", root->children[0]->data->entry.linenumber);
				return typeErrPtr;
			}

		/* ASSIGNMENT
		 * <assignmentStmt> ===> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
		 */
		case AssignmentOperation:
			//printf("Entered Assignment Operation.\n");
			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[1], localTable, baseTable);
			//printf("Found types\n");
			//if(t1!=NULL && t2!= NULL) printf("I am not Null %s %s \n", t1->identifier, t2->identifier);
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
			{
				return voidPtr;
			}	
			else if (t1->type == Real && t2->type == Real)
				return voidPtr;
			else if (t1->type == RecordType) {
				return checkTypeEquality(t1, t2);
			}
			else if ((t1->type == VariantRecord && t2->type == VariantRecord)
					|| (t1->type == VariantRecord && (t2->type == Integer || t2->type == Real))
					|| (t2->type == VariantRecord && (t1->type == Integer || t1->type == Real))) {
				return voidPtr;
			}
			else {
				printf("Line %d: Assignment Error - type mismatch %s and %s.\n",
					root->children[0]->data->entry.linenumber, t1->identifier, t2->identifier);
				//printf("Line %d: Assignment error - incompatible types %s of type %s and %s of type %s.\n", root->children[0]->data->entry.linenumber, root->children[0]->data->entry.lexeme , t1->identifier, root->children[1]->data->entry.lexeme, t2->identifier);
				return typeErrPtr;
			}

		/* accessing VARIABLES or FIELDS of RECORDS
		 * <singleOrRecId> ===> TK_ID <optionSingleConstructed>
		 */
		case SingleOrRecIdLinkedListNode:

			//printf("Entered singleOrRecIdLinkedListNode.\n");
			// lookup in the most local scope - scope of function
			//printf("    %s  \n", root->data->entry.lexeme);

			//printf("%s\n", localTable->tableID);

			entry = lookupSymbolTable(localTable, root->data->entry.lexeme);

			//if(entry != NULL) printf("%s\n", entry->identifier);
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

			if (isVariantRecord(root, entry) == TRUE) {
				printf("Line Number %d: Variant record %s detected - dynamic type checking to be handled at run time.\n", 
					root->data->entry.linenumber, root->data->entry.lexeme);
				return variantRecordPtr;
			}

			// variable is a record or a union
			if (entry->type->compositeVariableInfo != NULL) {
				//printf("Entered field gate\n");
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
			//printf("Entered FuncCall\n.");
			// function identifier should be present in the symbol table
			entry = lookupSymbolTable(baseTable, root->children[1]->entry.lexeme);
			SymbolTableEntry* callingFunctionEntry = lookupSymbolTable(baseTable, localTable->tableID);

			//printf("%s\n", root->children[1]->entry.lexeme);
			//printf("%s\n", baseTable->tableID);
			
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
			
			//printf("%d %d \n", callingFuncInfo->declarationSeqNum, funcInfo->declarationSeqNum);
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
				//printf("Entered input loop.\n");
				t1 = findType(actualInput->data, localTable, baseTable);

				//printf("Returned back to funct loop.\n");
				t2 = formalInput->datatype;
				
				if (t1->type == TypeErr || t2->type == TypeErr) {
					return typeErrPtr;
				}

				// if(lookupSymbolTable(baseTable, actualInput->data->entry.lexeme) != NULL){
				// 	printf("Line %d : input parameter %s cannot be redefined in %s as it is a global variable \n", root->children[1]->entry.linenumber, actualInput->data->entry.lexeme, callingFuncInfo->identifier);
				// 	return typeErrPtr;
				// }

				// passing union parameters not allowed
				if (t1->type == UnionType || t2->type == UnionType){
					printf("Line %d : Input parameter cannot be of type union\n", root->children[1]->entry.linenumber);
					return typeErrPtr;
				}

				// parameters should match in type
				if (checkTypeEquality(t1, t2)->type == TypeErr) {
					printf("Line %d : Input parameter mismatch for %s of %s - expected %s but got %s.\n", root->children[1]->entry.linenumber, formalInput->identifier, funcInfo->identifier, t2->identifier, t1->identifier);
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
				
				// if(lookupSymbolTable(baseTable, actualOutput->data->entry.lexeme) != NULL){
				// 	printf("Line %d : output parameter %s cannot be redefined in %s as it is a global variable \n", root->children[1]->entry.linenumber, actualOutput->data->entry.lexeme, callingFuncInfo->identifier);
				// 	return typeErrPtr;
				// }

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

		// -----
		// please Return case ke braces mat hatana
		// otherwise declarations lambi padengi
		// -----
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
			//printf("Entered Default %d\n", root->type);
			if (root->isLinkedListNode) {
				//if(root->type == Return) for(int i=0;i<10;i++)printf("Hello!\n");
				// if(root->type == )
				// numNodesinLinkedList =0;
				// trav = root;


				// while(trav!= NULL){
				// 	numNodesinLinkedList++;
				// 	trav = trav->next;
				// }
				// if(numNodesinLinkedList==1) {
				// 	printf("Hey!\n");
				// 	if(trav->data != NULL){

				// 		return findType(trav->data, localTable, baseTable);

				// 	}
				// 	else return voidPtr;

				// }
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

				//if(root->type == Return) for(int i=0;i<10;i++)printf("Hello!\n");
				error_flag =0;
				int children = MAX_PROD_LEN;
				while (children > 0 && root->children[children-1] == NULL){
					children--;
				}
				// to remove
				//printf("%d\n", children);
				boolean childErrFlag = FALSE;
				for (int i = 0; i < children; i++){

					// to remove
					//printf("-%d-", root->children[i]->type);
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
	//printf("Reaching in fields\n");
	// control flow should not reach this point otherwise
	if (root == NULL || fieldLL == NULL) {
		printf("Field not found.\n");
		return typeErrPtr;
	}
	
	//printf("Reached in fields0%s\n", root->data->entry.lexeme);

	struct Field* fieldSelected = searchInFieldLL(root->data->entry.lexeme, fieldLL);
	//printf("Reached in fields1\n");

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
			//printf("Entered here.\n");
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
		//printf("%s\n", currFunc->identifier);
		localTable = currFunc->tablePointer;

		Type elem = findType(funcNode->data->children[3], localTable, baseTable)->type;

		if (elem != Void) {
			printf("TYPE ERROR DETECTED IN %s.\n", funcLexeme);
			//return -1;
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
	printf("Entering Function: %s...\n", mainLexeme);
	localTable = (lookupSymbolTable(baseTable, mainLexeme))->tablePointer;
	if (findType(root->children[1]->children[0], localTable, baseTable)->type != Void) {
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
	//printf("Entered type check equality.\n");
	//printf("%s %s\n", t1->identifier, t2->identifier);
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
	//printf("%s %s\n", t1->identifier, t2->identifier);
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

				//printf("%s\n", toVisitLL->lexeme);
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
			toVisitLL = extractVariablesFromBoolean(root->children[0], toVisitLL);
			toVisitLL = extractVariablesFromBoolean(root->children[1], toVisitLL);

		case arithOp_DIV:
		case arithOp_MINUS:
		case arithOp_PLUS:
		case arithOp_MUL:
			//printf("In operator\n");
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
	//printf("in check changes\n");
	//if (markVariableChanges(root, toVisitLL) == FALSE)
	//	return FALSE;
	//printf("Here again in check\n");
	//return TRUE;
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
	//printf("in mark changes\n");

	VariableVisitedNode *curr, *toVisitLLNested;
	astNode* actualOutput;
	char* lhsName;
	astNode* readVariable;

	//printf("Statement type: %s\n", getStatmType(root->type));

	if (root == NULL) {
		//printf("In null mark.\n");
		return FALSE;
	}

	switch (root->type) {
		case While:
			toVisitLLNested = NULL;
			toVisitLLNested = extractVariablesFromBoolean(root->children[0], toVisitLLNested);
			//printf("Here in default 0\n");
			if (checkVariableChanges(root->children[1], toVisitLLNested) == FALSE) {
				return FALSE;
			}
			return markVariableChanges(root->children[1], toVisitLL);

		case AssignmentOperation:
			//printf("In assignment mark\n");
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
			//printf("Entered mark read.\n");
			readVariable = root->children[0];
			if (readVariable->type == Num || readVariable->type == RealNum) {
				printf("Can not read into a literal!\n");
				return FALSE;
			}
			lhsName = readVariable->data->entry.lexeme;
			
			curr = toVisitLL;
			while (curr != NULL) {
				//printf("mark read %s-%s\n", lhsName, curr->lexeme);
				if (strcmp(curr->lexeme, lhsName) == 0) {
					curr->visited = TRUE;
					return TRUE;
				}
				curr = curr->next;
			}
			return FALSE;
		
		case If: {
			//printf("here in if.\n");
			boolean flag = FALSE;
			if (markVariableChanges(root->children[1], toVisitLL) == TRUE) {
				//printf("if first statement.\n");
				flag = TRUE;
			}
			if (markVariableChanges(root->children[2], toVisitLL) == TRUE) {
				//printf("if other statements\n");
				flag = TRUE;
			}
			return flag;
		}

		/*
		case logOp_AND:
		case logOp_OR:
			return markVariableChanges(root->children[0], toVisitLL) ||
				markVariableChanges(root->children[1], toVisitLL);
		*/

		default:
			if (root->isLinkedListNode) {
				boolean flag = FALSE;
				// earlier root->next
				if (root->data != NULL) {
					//printf("Here in default 1\n");
					//printf("ASTNode var: %s\n", root->data->entry.lexeme);
					if (markVariableChanges(root->data, toVisitLL) == TRUE)
						flag = TRUE;
				}
				if (root->next == NULL){
					//printf("Here in defAULT 2\n");
					return FALSE;
				}

				//printf("Here in defAULT 4\n");
				return markVariableChanges(root->next, toVisitLL) || flag;
			} 
			else {
				/* visit all the children */
				//printf("Here in default 3\n");
				int children = MAX_PROD_LEN;
				while (children > 0 && root->children[children-1] == NULL){
					children--;
				}
				// to remove
				//printf("%d\n", children);
				boolean flag = FALSE;
				for (int i = 0; i < children; i++){
					// to remove
					//printf("-%d-", root->children[i]->type);
					//printf("ASTNode var: %s\n", root->children[i]->entry.lexeme);
					if (markVariableChanges(root->children[i], toVisitLL) == TRUE){
						flag = TRUE;
					}
				}
				return flag;
			}
			return FALSE;
	}
}

boolean isVariantRecord(astNode* root, SymbolTableEntry* entry) {
	if (entry == NULL || entry->type == NULL) {
		return FALSE;
	}

	// should be a record first to qualify as a variant record
	if (entry->type->type != RecordType) {
		return FALSE;
	}

	if (root == NULL || root->next == NULL || root->next->next == NULL
		|| root->data == NULL || root->next->data == NULL || root->next->next->data == NULL) {
		return FALSE;
	}

	Field* fieldLL = entry->type->compositeVariableInfo->listOfFields;

	Field *head = fieldLL;
	Field* unionField = NULL;
	boolean flagTag = FALSE, flagUnion = FALSE, flagDuplicate = FALSE;
	while (head != NULL) {
		if (strcmp(head->identifier, "tagvalue") == 0
			&& (head->datatype->type == Real || head->datatype->type == Integer)) {
			flagTag = TRUE;
		}
		if ((head->datatype->type == UnionType || 
			(head->datatype->aliasTypeInfo != NULL && 
			head->datatype->aliasTypeInfo->type == UnionType)) && flagUnion == FALSE) {
			unionField = head;
			flagUnion = TRUE;
		}
		else if (head->datatype->type == UnionType 
			&& (head->datatype->aliasTypeInfo != NULL && head->datatype->aliasTypeInfo->type == UnionType) 
			&& flagUnion == FALSE) {
			flagDuplicate = TRUE;
		}
		head = head->next;
	}

	if (flagTag == FALSE || flagUnion == FALSE || flagDuplicate == TRUE) {
		return FALSE;
	}

	
	Field* unionFieldLL = NULL;
	if(unionField->datatype->type == UnionType){
		unionFieldLL = unionField->datatype->compositeVariableInfo->listOfFields;
	}else if(unionField->datatype->type == Alias){
		unionFieldLL = unionField->datatype->aliasTypeInfo->compositeVariableInfo->listOfFields;
	}else{
		return FALSE;
	}

	// Field* unionFieldLL = unionField->datatype->compositeVariableInfo->listOfFields;
	head = unionFieldLL;
	while (head != NULL) {
		if (head->datatype != NULL && head->datatype->type != Integer && head->datatype->type != Real) {
			return FALSE;
		}
		head = head->next;
	}

	return TRUE;
}