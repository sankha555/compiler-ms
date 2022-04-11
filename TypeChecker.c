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
Type findType(astNode* root, SymbolTable* localTable,
		SymbolTable* globalTable, SymboleTable* baseTable) {
	if (root == NULL)
		return Void;

	if (root->isLeafNode) {
		switch (root->type) {
			/* INTEGER LITERAL
			 * <var> ===> TK_NUM
			 */
			case Num:
				return Integer;
			
			/* REAL LITERAL
			 * <var> ===> TK_RNUM
			 */
			case RealNum:
				return Real;

			// likely redundant case
			/* a VARIABLE IDENTIFIER */
			case VariableId:
				entry = lookup(localTable, root->entry);
				if (entry == NULL)
					entry = lookup(globalTable, root->entry);
				if (entry == NULL)
					return TypeErr;
				return entry.type != Union ? entry.type : TypeErr;
			
			/* all other leaves assumed typeless
			 * TypeErr used only for erroneous leaves
			 */
			default:
				return Void;
		}
	}

	// for storing types of operands
	Type t1, t2;
	
	// to store the symbol table entry pointer for the identifier
	SymbolTableEntry* entry;

	switch (root->type) {
		/* RELATIONAL OPERATORS
		 * <booleanExpression> ===> <var> <relationalOp> <var> 
		 */
		case relOp_LT:
		case relOp_LE:
		case relOp_EQ:
		case relOp_GE:
		case relOp_GT:
			t1 = findType(root->children[0], localTable, globalTable, baseTable);
			t2 = findType(root->children[2], localTable, globalTable, baseTable);
			if (t1 == t2 && 
					(t1 == Integer || t1 == Real))
				return Boolean;
			else {
				printf("Relational operator: incompatible operands.\n");
				return TypeErr;
			}

		/* CONJUNCTION or DISJUNCTION
		 * <booleanExpression> ===> TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL
		 */
		case logOp_AND:
		case logOp_OR:
			t1 = findType(root->children[0], localTable, globalTable, baseTable);
			t2 = findType(root->children[1], localTable, globalTable, baseTable);
			if (t1 == Boolean && t2 == Boolean)
				return Boolean;
			else {
				printf("And/or: boolean operands required.\n");
				return TypeErr;
			}

		/* COMPLEMENTATION
		 * <booleanExpression> ===> TK_NOT TK_OP <booleanExpression> TK_CL
		 */
		case logOp_NOT:
			if (findType(root->children[0], localTable, globalTable, baseTable) == Boolean)
				return Boolean;
			else {
				printf("Complementation: boolean operand required.\n");
				return TypeErr;
			}
		
		/* ADDITION or SUBTRACTION
		 * <arithmeticExpression> ===> <term> <expPrime>
		 * <expPrime> ===> <lowPrecedenceOperators> <term> <expPrime>1
		 */
		case arithOp_PLUS:
		case arithOp_MINUS:
			t1 = findType(root->children[0], localTable, globalTable, baseTable);
			t2 = findType(root->children[2], localTable, globalTable, baseTable);
			if (t1 == Integer && t2 == Integer)
				return Integer;
			else if ((t1 == Integer && t2 == Real)
					|| (t1 == Real && t2 == Integer)
					|| (t1 == Real && t2 == Real))
				return Real;
			else {
				printf("Addition/Subtraction: real or integer operands required.\n");
				return TypeErr;
			}

		/* MULTIPLICATION
		 * <term> ===> <factor> <termPrime>
		 * <termPrime> ===> <highPrecedenceOperators> <factor> <termPrime>1
		 */
		case arithOp_MUL:
			t1 = findType(root->children[0], localTable, globalTable, baseTable);
			t2 = findType(root->children[2], localTable, globalTable, baseTable);
			if (t1 == Integer && t2 == Integer)
				return Integer;
			else if ((t1 == Integer && t2 == Real)
					|| (t1 == Real && t2 == Integer)
					|| (t1 == Real && t2 == Real))
				return Real;
			else {
				printf("Multiplication: real or integer operands required.\n");
				return TypeErr;
			}

		/* DIVISION
		 * <term> ===> <factor> <termPrime>
		 * <termPrime> ===> <highPrecedenceOperators> <factor> <termPrime>1
		 */
		case arithOp_DIV:
			t1 = findType(root->children[0], localTable, globalTable, baseTable);
			t2 = findType(root->children[2], localTable, globalTable, baseTable);
			if ((t1 == Integer || t1 == Real)
					&& (t2 == Integer || t2 == Real))
				return Real;
			else {
				printf("Division: real or integer operands required.\n");
				return TypeErr;
			}

		/* ASSIGNMENT
		 * <assignmentStmt> ===> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
		 */
		case AssignmentOperation:
			t1 = findType(root->children[0], localTable, globalTable, baseTable);
			t2 = findType(root->children[1], localTable, globalTable, baseTable);
			if ((t1 == Integer && t2 == Integer)
					|| (t1 == Real && (t2 == Integer || t2 == Real)))
				return Void;
			/*--TO DO: assignment for whole records?--*/
			else {
				printf("Assignment: incompatible types.\n");
				return TypeErr;
			}

		/* accessing VARIABLES or FIELDS of RECORDS
		 * <singleOrRecId> ===> TK_ID <optionSingleConstructed>
		 */
		case SingleOrRecIdLinkedListNode:
			// lookup in the most local scope - scope of function
			entry = lookup(funcTable, root->data->entry);

			// next lookup in the global table
			if (entry == NULL)
				entry = lookup(globalTable, root->data->entry);

			if (entry == NULL)
				return TypeErr; // type undefined for undefined variable

			// the variable is a primitive
			if (entry.type == Real || entry.type == Integer)
				if (root->next == NULL)
					return entry.type;
				else
					return TypeErr; // a primitive can not have fields
		
			if (root->next == NULL)
				return entry.type;

			/* replace the localTable here with the mini-table for record */
			return findTypeField(root->next, localTable, globalTable, baseTable);

		default:
			/* advance ahead on the linkedlist to ensure it
			 * is completely type checked
			 */
			if (root->isLinkedListNode) {
				if (root->next == NULL)
					return Void;
				return findType(root->next, localTable, globalTable, baseTable);
			}

			/* type check all the children */
			else {
				int children = MAX_PROD_LEN;
				while (children > 0 && root->children[children-1] == NULL)
					children--;
				for (int i = 0; i <= children; i++)
					if (findType(children, localTable, globalTable, baseTable) == TypeErr)
						return TypeErr;
				return Void;
			}
	}
}

/* inputs:
 * ast node for field
 * record symbol table supposed to contain the field
 * basetable
 */

Type findTypeField(astNode* root, SymbolTable* localTable,
		SymboleTable* baseTable) {
	if (root == NULL)
		return Void;

	/* FIELD of a CONSTRUCTED DATATYPE
	 * <optionSingleConstructed> ===> <oneExpansion> <moreExpansions>
	 */
	if (root->type == FieldIdLinkedListNode) {
		entry = lookup(funcTable, root->data->entry);
			
		if (entry == NULL)
			return TypeErr; // type undefined for undefined variable

		// the field is a primitive
		if (entry.type == Real || entry.type == Integer)
			if (root->next == NULL)
				return entry.type;
			else
				return TypeErr; // a primitive can not have fields

		if (root->next == NULL)
			return entry.type;

		/* replace the localTable here with the mini-table for record */
		return findType(root->next, localTable, globalTable, baseTable);
	}

	return typeErr;
}

/* inputs:
 * a root node - the Prog ast node
 * pointer to the base symbol table
 * */
void typeCheck(astNode* root, SymbolTable* baseTable) {
	/* <program> ===> <otherFunctions> <mainFunction>*/
	int children = MAX_PROD_LEN;
	while (children > 0 && root->children[children-1] == NULL)
		children--;

	SymbolTable* globalTable = NULL;
	// obtain global symbol table
	//

	/* traverse the functions with their specific local symbol tables */
	SymbolTable* localTable = NULL;
	for (int i = 0; i < children; i++) {
		// obtain function's local symbol table into localTable
		//
		if (findType(children, localTable, globalTable, baseTable) != Void)
			return TypeErr;
	}

	return Void;
}
