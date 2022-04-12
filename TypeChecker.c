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
struct TypeArrayElement* findType(astNode* root, 
		SymbolTable* localTable, SymbolTable* baseTable) {
	if (root == NULL);
		return Void;

	// for storing types of operands
	struct TypeArrayElement* t1, t2;
	
	// to store the symbol table entry pointer for the identifier
	SymbolTableEntry* entry;
	if (root->isLeafNode) {
		switch (root->type->type) {
			/* INTEGER LITERAL
			 * <var> ===> TK_NUM
			 */
			case Num:
				return intPtr;
			
			/* REAL LITERAL
			 * <var> ===> TK_RNUM
			 */
			case RealNum:
				return realPtr;

			// likely redundant case
			/* a VARIABLE IDENTIFIER */
			case VariableId:
				// check in function's local symbol table
				entry = lookupSymbolTable(localTable, root->entry);

				// check in global table
				if (entry == NULL)
					entry = lookupSymbolTable(baseTable, root->entry);

				// variable probably undeclared - unaccessible
				if (entry == NULL) {
					printf("Identifier unrecognized - undeclared or unaccessible.\n");
					return typeErrPtr;
				}

				return entry->type->type != Union ? 
					entry->type : typeErrPtr;
			
			/* all other leaves assumed typeless
			 * TypeErr used only for erroneous leaves
			 */
			default:
				return voidPtr;
		}
	}

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
			if (t1->type == t2->type && 
					(t1->type == Integer || t1->type == Real))
				return t1;
			else {
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
			if (t1->type == Boolean && t2->type == Boolean)
				return booleanPtr;
			else {
				printf("And/or: boolean operands required.\n");
				return typeErrPtr;
			}

		/* COMPLEMENTATION
		 * <booleanExpression> ===> TK_NOT TK_OP <booleanExpression> TK_CL
		 */
		case logOp_NOT:
			if ((findType(root->children[0], localTable, baseTable))->type == Boolean)
				return booleanPtr;
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
			if (t1->type == Integer && t2->type == Integer)
				return intPtr;
			else if ((t1->type == Integer && t2->type == Real)
					|| (t1->type == Real && t2->type == Integer)
					|| (t1->type == Real && t2->type == Real))
				return realPtr;
			else if (t1->type == Record && (t1 == t2-))
				return t1;
			else {
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
			if (t1->type == Integer && t2->type == Integer)
				return intPtr;
			else if ((t1->type == Integer && t2->type == Real)
					|| (t1->type == Real && t2->type == Integer)
					|| (t1->type == Real && t2->type == Real))
				return realPtr;
			else if (t1->type == Record && (t1 == t2))
				return t1;
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
			if ((t1->type == Integer || t1->type == Real)
					&& (t2->type == Integer || t2->type == Real))
				return realPtr;
			else if (t1->type == Record && (t1 == t2))
				return t1;
			else {
				printf("Division: real or integer operands required.\n");
				return typeErrPtr;
			}

		/* ASSIGNMENT
		 * <assignmentStmt> ===> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
		 */
		case AssignmentOperation:
			t1 = findType(root->children[0], localTable, baseTable);
			t2 = findType(root->children[1], localTable, baseTable);
			if ((t1->type == Integer && t2->type == Integer)
					|| (t1->type == Real && (t2->type == Integer || t2->type == Real)))
				return voidPtr;
			/*--TO DO: assignment for whole records?--*/
			else if (t1->type == Record && (t1 == t2))
				return t1;

			else {
				printf("Assignment: incompatible types.\n");
				return typeErrPtr;
			}

		/* accessing VARIABLES or FIELDS of RECORDS
		 * <singleOrRecId> ===> TK_ID <optionSingleConstructed>
		 */
		case SingleOrRecIdLinkedListNode:
			// lookup in the most local scope - scope of function
			entry = lookupSymbolTable(localTable, root->data->entry);

			// next lookup in the global table
			if (entry == NULL)
				entry = lookupSymbolTable(baseTable, root->data->entry);

			// entry not found - probably undeclared / out of scope
			if (entry == NULL) {
				printf("Variable undeclared or out of scope.\n");
				return typeErrPtr; // type undefined for undefined variable
			}

			// the variable is a primitive
			if (entry->type->type == Real || entry->type->type == Integer)
				if (root->next == NULL)
					return entry->type;
				else
					return typeErrPtr; // a primitive can not have fields
		
			if (root->next == NULL)
				return entry->type;

			// variable is a record or a union
			if (entry->compositeVariableInfo != NULL) {
				return findTypeField(root->next, entry->compositeVariableInfo->listOfFields);
			}

			else {
				printf("Empty composite!\n");
				return typeErrPtr;
			}


		default:
			/* advance ahead on the linkedlist to ensure it
			 * is completely type checked
			 */
			if (root->isLinkedListNode) {
				if (root->next == NULL)
					return voidPtr;
				return findType(root->next, localTable, baseTable);
			}

			/* type check all the children */
			else {
				int children = MAX_PROD_LEN;
				while (children > 0 && root->children[children-1] == NULL)
					children--;
				for (int i = 0; i <= children; i++)
					if (findType(children, localTable, baseTable)->type == TypeErr)
						return typeErrPtr;
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

	struct Field* fieldSelected = searchInFieldLL(root->entry, fieldLL);

	if (fieldSelected == NULL) {
		printf("Field not found.\n");
		return typeErrPtr;
	}

	
	/* FIELD of a CONSTRUCTED DATATYPE
	 * <optionSingleConstructed> ===> <oneExpansion> <moreExpansions>
	 */
	if (root->type == FieldIdLinkedListNode) {
		struct TypeArrayElement* entry = fieldSelected->datatype;
			
		if (entry == NULL) {
			printf("Field not found.\n");
			return TypeErr; // type undefined for undefined variable
		}

		// the field is a primitive
		if (entry->type->type == Real || entry->type->type == Integer)
			if (root->next == NULL)
				return entry.type;
			else
				return typeErrPtr; // a primitive can not have fields

		if (root->next == NULL)
			return entry->type;

		/* replace the localTable here with the mini-table for record */
		if (entry->compositeVariableInfo != NULL) {
				return findTypeField(root->next, entry->compositeVariableInfo->listOfFields);
		}

		else {
			printf("Empty record!\n");
			return typeErrPtr;
		}
	}

	// not supposed to be here otherwise according to AST
	return typeErr;
}

struct Field* searchInFieldLL(char* fieldLexeme, struct Field* fieldLL) {
	Field* curr = fieldLL;
	while (curr != NULL) {
		if (strcmp(curr->identifier, fieldLexeme) == 0)
			return curr;
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
		char* funcLexeme = funcNode->children[0]->entry;

		// obtain the symbol table for the current function
		SymbolTableEntry* currFunc = lookupSymbolTable(baseTable, funcLexeme);
		localTable = currFunc->tablePointer;

		if (findType(children, localTable, baseTable)->type != Void) {
			printf("TYPE ERROR DETECTED IN %s.\n", funcLexeme);
			return -1;
		}

		funcNode = funcNode->next;
	}

	/* traverse <mainFunction> */
	char *mainLexeme = "_main"
	localTable = (lookupSymbolTable(baseTable, mainLexeme))->tablePointer;
	if (findType(children, localTable, baseTable)->type != Void) {
		printf("TYPE ERROR DETECTED IN MAIN.\n");
		return -1;
	}

	printf("TYPE CHECKING SUCCESSFUL - ALL TESTS CLEARED!");
	return 0;
}
