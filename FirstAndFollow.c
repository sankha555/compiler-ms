# include "FirstAndFollow.h"

/* 
int main(int argc, char *argv[]) {
	// exactly one input file required
	if (argc != 2) {
		printf("ERROR: exactly one file argument expected!\n");
		return -1;
	}

	computeFirstAndFollowSets(argv[1]);

	return 0;
}
*/

void computeFirstAndFollowSets(grammar G) {
	/* populate the rules data structure with the grammar */
	populateGrammar(G);

	/* compute the first sets for all non-terminals
	 * without adding epsilon to the first sets
	 * rather only marking the tokens nullable or not
	 */
	computeFirstAll();
	
	/* compute the follow sets for all non-terminals */
	computeFollowAll();
	
	//printTest();
	
	/* populate the prescribed FirstAndFollow data structure */
	populateFirstAndFollow();

	/* save results in respective files */
	writeFirstsToFile();
	writeFollowsToFile();
}

void populateGrammar(grammar G) {
	// open the grammar file
	FILE *fp = fopen(G, "r");
	
	if (!fp) {
		printf("ERROR: could not open the file!\n");
		return;
	}

	numRules = numNonTerminals = 0;

	char currLine[MAX_PROD_LEN * MAX_LEN];
	while (fgets(currLine, 500, fp) != NULL) {
		char head[MAX_LEN];
		char body[MAX_LEN * MAX_PROD_LEN];

		// get the current line from the file input stream
		// dividing it into head and body
		sscanf(currLine, "%s ===> %[^\n\t]", head, body);
		
		// if it is the first occurence of the non-terminal as a head
		// store the corresponding info
		if (whichNonTerminal(head) < 0) {
			strcpy(NonTerms[numNonTerminals].symbol, head);
			NonTerms[numNonTerminals].firstLen = NonTerms[numNonTerminals].followLen = 0;
			NonTerms[numNonTerminals].firstComputed = FALSE;
			NonTerms[numNonTerminals].nullable = FALSE;
			numNonTerminals++;
			printf("Symbol: %s, num NTs: %d\n", NonTerms[numNonTerminals-1].symbol, numNonTerminals);
		}

		// store the rule
		strcpy(Rules[numRules].head, head);

		char *token = strtok(body, " ");
		int j = 0;
		while (token != NULL) {
			strcpy(Rules[numRules].body[j++], token);
			token = strtok(NULL, " ");
		}
		Rules[numRules].bodyLen = j;

		numRules++;
	}
    printf("num of nts: %d", numNonTerminals);

	fclose(fp);
}

int whichNonTerminal(char *symbol) {
	// is token in the list of non-terminals stored in NonTerms
	for (int i = 0; i < numNonTerminals; i++)
		if (strcmp(symbol, NonTerms[i].symbol) == 0)
			return i; // return its in index in NonTerms array

	return -1;
}

void computeFirstAll() {
	/* go over the list of non-terminals
	 * computing the first sets
	 * if not computed already
	 */
	for (int i = 0; i < numNonTerminals; i++)
		if (!NonTerms[i].firstComputed)
			computeFirstOf(i);
}

void computeFirstOf(int index) {
	char *symbol = NonTerms[index].symbol;

	/* use the rules where non-terminal appears in head */
	for (int i = 0; i < numRules; i++)
		if (strcmp(symbol, Rules[i].head) == 0)
			computeFirstFromRule(index, i, 0);

	NonTerms[index].firstComputed = TRUE;	
}

void computeFirstFromRule(int headIndex, int ruleIndex, int start) {
	char *currToken = Rules[ruleIndex].body[start];

	int currIndex;
	// if first body token is not a non-terminal
	if ((currIndex = whichNonTerminal(currToken)) < 0) {
		// if it is epsilon
		if (strcmp(currToken, epsilon) == 0)
			NonTerms[headIndex].nullable = TRUE;
		// a terminal
		else {
			if (!presentInFirst(headIndex, currToken)) {
				strcpy(NonTerms[headIndex].firsts[NonTerms[headIndex].firstLen], currToken);
				NonTerms[headIndex].firstLen++;
			}
		}
		return;
	}

	// calculate the first set of first non-terminal
	// if not already calculated
	if (!NonTerms[currIndex].firstComputed)
		computeFirstOf(currIndex);

	// add first set of first non-terminal in body
	for (int i = 0; i < NonTerms[currIndex].firstLen; i++) {
		if (!presentInFirst(headIndex, NonTerms[currIndex].firsts[i])) {
			strcpy(NonTerms[headIndex].firsts[NonTerms[headIndex].firstLen],
					NonTerms[currIndex].firsts[i]);
			NonTerms[headIndex].firstLen++;
		}
	}

	// if epsilon in first non-terminal of the body
	if (NonTerms[currIndex].nullable) {
		// if current symbol is the last in body
		if (start == Rules[ruleIndex].bodyLen - 1) {
			NonTerms[headIndex].nullable = TRUE;
		}
		else {
			computeFirstFromRule(headIndex, ruleIndex, start + 1);
		}
	}
}

boolean presentInFirst(int index, char* token) {
	/* linear search for token in the first set of non-terminal */
	for (int i = 0; i < NonTerms[index].firstLen; i++)
		if (strcmp(NonTerms[index].firsts[i], token) == 0)
			return TRUE;
	return FALSE;
}

void computeFollowAll() {
	/* add $ to the follow set of start symbol */
	int startSymIndex = whichNonTerminal(Rules[0].head);
	strcpy(NonTerms[startSymIndex].follows[0], "$");
	NonTerms[startSymIndex].followLen++;

	/* if A -> alpha B beta
	 * add first set of string beta to follow(B)
	 * if beta = epsilon or epsilon belongs to first(beta)
	 * mark the rule to add follow(A) to follow(B)
	 */

	/* to add follow(A) to follow(B)
	 * marked[indB][indA] = 1
	 */
	char marked[numNonTerminals][numNonTerminals];

	// important - garbage values give erroneous results
	for (int i = 0; i < numNonTerminals; i++)
		for (int j = 0; j < numNonTerminals; j++)
			marked[i][j] = 0;

	for (int i = 0; i < numNonTerminals; i++) {
		// search for all rules containing the non-terminal
		// in the production body
		for (int j = 0; j < numRules; j++) {
			int symPos; // position of non-terminal in the body

			// if non-terminal in the rule
			if ((symPos = findPosOf(j, NonTerms[i].symbol)) >= 0) {
				// case of A -> alpha B
				if (symPos == Rules[j].bodyLen - 1) {
					marked[i][whichNonTerminal(Rules[j].head)] = 1;
					continue;
				}

				// compute the first set of suffix of non-terminal in body
				// case of A -> alpha B beta
				char firstSet[MAX_TERMINALS][MAX_LEN];
				int firstSetLen = 0;
				int *ptr = &firstSetLen;
				boolean isNullable = getFirstOfString(j, symPos + 1, firstSet, ptr);

				// add new terminals from first set of beta to follow(B)
				for (int k = 0; k < firstSetLen; k++) {
					if (!presentInFollow(i, firstSet[k])) {
						strcpy(NonTerms[i].follows[NonTerms[i].followLen], firstSet[k]);
						NonTerms[i].followLen++;
					}
				}

				// if suffix can be made epsilon, mark the pair
				if (isNullable)
					marked[i][whichNonTerminal(Rules[j].head)] = 1;
			}
		}
	}

	/* for all the marked pairs
	 * add the follow(A) to follow(B)
	 */
	int epochs = 0;

	PASS:
	for (int i = 0; i < numNonTerminals; i++)
		for (int j = 0; j < numNonTerminals; j++) {
			if (!marked[i][j] || i == j)
				continue;
			for (int k = 0; k < NonTerms[j].followLen; k++)
				if (!presentInFollow(i, NonTerms[j].follows[k])) {
					strcpy(NonTerms[i].follows[NonTerms[i].followLen],
							NonTerms[j].follows[k]);
					NonTerms[i].followLen++;
				}
		}

	epochs++;
	if (epochs < EPOCHS)
		goto PASS;
}

int findPosOf(int ruleIndex, char *symbol) {
	// scan the production body to find the non-terminal, if present
	for (int i = 0; i < Rules[ruleIndex].bodyLen; i++)
		if (strcmp(symbol, Rules[ruleIndex].body[i]) == 0)
			return i;
	return -1;
}

boolean getFirstOfString(int ruleIndex, int start, char firstSet[][MAX_LEN], int *firstSetLen) {
	char *token = Rules[ruleIndex].body[start];
	int ntIndex = whichNonTerminal(token);
	
	// string starts does not start with a non-terminal
	if (ntIndex < 0) {
		if (strcmp(token, epsilon) == 0) {
			return TRUE;
		}
		else {
			// add the terminal to the first set
			if (!presentInArr(firstSet, *firstSetLen, token)) {
				strcpy(firstSet[*firstSetLen], token);
				*firstSetLen = *firstSetLen + 1;
			}
			return FALSE;
		}
	}
	
	// adding the first set of non-terminal to first set of string
	for (int i = 0; i < NonTerms[ntIndex].firstLen; i++)
		if (!presentInArr(firstSet, *firstSetLen, NonTerms[ntIndex].firsts[i])) {
			strcpy(firstSet[*firstSetLen], NonTerms[ntIndex].firsts[i]);
			*firstSetLen = *firstSetLen + 1;
		}

	// if epsilon in first set of first non-terminal, go to the next symbol
	if (NonTerms[ntIndex].nullable) {
		if (start == Rules[ruleIndex].bodyLen - 1)
			return TRUE;
		return getFirstOfString(ruleIndex, start + 1, firstSet, firstSetLen);
	}
	
	return FALSE;
}

boolean presentInArr(char firstSet[][MAX_LEN], int firstSetLen, char *token) {
	// scan the first set of string linearly for a token
	for (int i = 0; i < firstSetLen; i++)
		if (strcmp(firstSet[i], token) == 0)
			return TRUE;
	return FALSE;
}

boolean presentInFollow(int index, char* token) {
	// scan the follow set of non-terminal linearly for a token
	for (int i = 0; i < NonTerms[index].followLen; i++)
		if (strcmp(NonTerms[index].follows[i], token) == 0)
			return TRUE;
	return FALSE;
}

/*
void printTest() {
	for (int i = 0; i < numNonTerminals; i++)
		printf("%s\n", NonTerms[i].symbol);
	for (int i = 0; i < numNonTerminals; i++) {
		printf("%s\t%d\n", NonTerms[i].symbol, NonTerms[i].firstComputed);
		for (int j = 0; j < NonTerms[i].firstLen; j++)
			printf("%s ", NonTerms[i].firsts[j]);
		if (NonTerms[i].nullable)
			printf("%s", epsilon);
		printf("\n\n");
	}

	printf("\n\n\n");
	
	for (int i = 0; i < numNonTerminals; i++) {
		printf("%s\t%d\n", NonTerms[i].symbol, NonTerms[i].followLen);
		for (int j = 0; j < NonTerms[i].followLen; j++)
			printf("%s ", NonTerms[i].follows[j]);
		printf("\n\n");
	}
}
*/

void populateFirstAndFollow() {
		for (int i = 0; i < numNonTerminals; i++) {
		strcpy(FirstAndFollowList[i].symbol, NonTerms[i].symbol);

		FirstAndFollowList[i].firstLen = NonTerms[i].firstLen;
		FirstAndFollowList[i].followLen = NonTerms[i].followLen;

		// copy the first set elements
		int j;
		for (j = 0; j < NonTerms[i].firstLen; j++)
			FirstAndFollowList[i].first[j] = searchTerminal(NonTerms[i].firsts[j]);

		FirstAndFollowList[i].nullable = NonTerms[i].nullable;
		
		FirstAndFollowList[i].dollarInFollow = FALSE;
		for (j = 0; j < NonTerms[i].followLen; j++) {
			if (strcmp(NonTerms[i].follows[j], "$") == 0) {
				FirstAndFollowList[i].dollarInFollow = TRUE;
				FirstAndFollowList[i].followLen--;
			}
			else
				FirstAndFollowList[i].follow[j] = searchTerminal(NonTerms[i].follows[j]);
		}
	}
}

int searchTerminal(char *terminal) {
	int n = sizeof(TerminalList) / sizeof(TerminalList[0]);
	for (int i = 0; i < n; i++)
		if (strcmp(terminal, TerminalList[i]) == 0)
			return i;
	return -1;
}

void writeFirstsToFile() {
	FILE *fp = fopen(firstFile, "w");
	
	if (!fp) {
		printf("ERROR: unable to open the file: %s!\n", firstFile);
		return;
	}

	for (int i = 0; i < numNonTerminals; i++) {
		fprintf(fp, "%s ", NonTerms[i].symbol);
		for (int j = 0; j < NonTerms[i].firstLen; j++)
			fprintf(fp, "%s ", NonTerms[i].firsts[j]);
		if (NonTerms[i].nullable)
			fprintf(fp, "%s ", epsilon); // add epsilon to end of line, if applicable
		fprintf(fp, "\n");
	}

	fclose(fp);
}

void writeFollowsToFile() {
	FILE *fp = fopen(followFile, "w");
	
	if (!fp) {
		printf("ERROR: unable to open the file: %s!\n", followFile);
		return;
	}

	for (int i = 0; i < numNonTerminals; i++) {
		fprintf(fp, "%s ", NonTerms[i].symbol);
		for (int j = 0; j < NonTerms[i].followLen; j++)
			fprintf(fp, "%s ", NonTerms[i].follows[j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void populateTerminals() {
	char *tokenNames[] = {
        "TK_WITH",
        "TK_PARAMETERS",
        "TK_END",
        "TK_WHILE",
        "TK_UNION",
        "TK_ENDUNION",
        "TK_DEFINETYPE",
        "TK_AS",
        "TK_TYPE",
        "TK_GLOBAL",
        "TK_PARAMETER",
        "TK_LIST",
        "TK_INPUT",
        "TK_OUTPUT",
        "TK_INT",
        "TK_REAL",
        "TK_ENDWHILE",
        "TK_IF",
        "TK_THEN",
        "TK_ENDIF",
        "TK_READ",
        "TK_WRITE",
        "TK_RETURN",
        "TK_CALL",
        "TK_RECORD",
        "TK_ENDRECORD",
        "TK_ELSE",
		"TK_ASSIGNOP",
		"TK_COMMENT",
        "TK_FIELDID",
        "TK_ID",
        "TK_NUM",
        "TK_RNUM",
        "TK_FUNID",
        "TK_RUID",
        "TK_MAIN",
        "TK_SQL",
        "TK_SQR",
        "TK_COMMA",
        "TK_SEM",
        "TK_COLON",
        "TK_DOT",
        "TK_OP",
        "TK_CL",
        "TK_PLUS",
        "TK_MINUS",
        "TK_MUL",
        "TK_DIV",
        "TK_AND",
        "TK_OR",
        "TK_NOT",
        "TK_LT",
        "TK_LE",
        "TK_EQ",
        "TK_GT",
        "TK_GE",
        "TK_NE",
        "TK_EOF"
	};

	int n = sizeof(tokenNames) / sizeof(tokenNames[0]);
		
	for (int i = 0; i < n; i++)
		strcpy(TerminalList[i], tokenNames[i]);
}