# ifndef TYPE_CHECKER_HEADER
# define TYPE_CHECKER_HEADER

# include <stdio.h>
//# include "globaldef.h"
# include "astGenerator.h"
# include "astDef.h"
# include "parserDef.h"
# include "lexerDef.h"
# include "parser.h"
# include "typing.h"
# include "symbolTableDef.h"
# include "symbolTable.h"

struct TypeArrayElement* findType(astNode* root, 
		SymbolTable* localTable, SymbolTable* baseTable);

struct TypeArrayElement* findTypeField(astNode* root, 
		struct Field* fieldLL);

struct Field* searchInFieldLL(char* fieldLexeme,
		struct Field* fieldLL);

int typeCheck(astNode* root, SymbolTable* baseTable);

struct TypeArrayElement* checkTypeEquality(struct TypeArrayElement* t1, 
		struct TypeArrayElement* t2);

int findLengthFormal(FunctionParameter* head);

int findLengthActual(astNode* head);

# endif
