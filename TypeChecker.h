# ifndef TYPE_CHECKER_HEADER
# define TYPE_CHECKER_HEADER

# include <stdio.h>
# include "globaldef.h"
# include "astGenerator.h"
# include "astDef.h"
# include "parserDef.h"
# include "lexerDef.h"
# include "parser.h"
# include "typing.h"
# include "symbolTableDef.h"
# include "symbolTable.h"

Type findType(astNode* root, SymbolTable* localTable, SymbolTable* globalTable, SymboleTable* baseTable);

Type findTypeField(astNode* root, SymbolTable* localTable, SymboleTable* baseTable)

void typeCheck(astNode* root, SymbolTable* baseTable);

# endif
