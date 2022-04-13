# ifndef SEMANTIC_ANALYZER_HEADER
# define SEMANTIC_ANALYZER_HEADER
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

int semanticAnalyzer(astNode* root);

# endif
