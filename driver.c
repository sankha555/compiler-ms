#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lexer.h"
#include "buffer.h"
#include "FirstAndFollow.h"
#include "parserDef.h"
#include "parser.h"
#include "globalDef.h"
#include "astDef.h"
#include "astGenerator.h"
#include "symbolTableDef.h"
#include "symbolTable.h"
#include "TypeChecker.h"
#include "icgGeneratorDef.h"
#include "icgGenerator.h"
#include "codeGeneratorDef.h"
#include "codeGenerator.h"
#include <time.h>
#include <stdlib.h>

SymbolTable* performPrelims(twinBuffer* buffer, char* testcaseFile, ParseTreeNode *root, astNode *astRoot, SymbolTable* globalSymbolTable){
    buffer = init_lexer(testcaseFile);
    if (buffer == NULL)
    {
        return globalSymbolTable;
    }

    FirstAndFollowElement *FirstAndFollowAll = computeFirstAndFollowSets(GRAMMAR_FILE);

    populateRules();

    createParseTable(FirstAndFollowAll, parseTable);

    root = parseInputSourceCode(buffer, 0);

    astRoot = createAbstractSyntaxTree(root);

    globalTypeTable = createTypeTable("GLOBAL_TYPE_TABLE");

    globalSymbolTable = initializeSymbolTableNew(astRoot); //made a change

    return globalSymbolTable;
}

int main(int argc, char *argv[])
{
    // The first argument is the file name
    if (argc != 2)
    {
        printf("Incorrect num of ARGUMENTS. Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // char *treeFile;

    // user gives custom name for parsetree file
    // treeFile = argv[2];

    twinBuffer *buffer;
    FILE *fp;
    ParseTreeNode *root;
    astNode *astRoot;
    SymbolTable* globalSymbolTable;

    printf("CS F303 Compiler Construction\n");
    printf("=======================================================\n");
    printf("Group Members");
    printf("\n\t1. Pratham Neeraj Gupta (2019A7PS0051P)");
    printf("\n\t2. Madhav Gupta (2019A7PS0063P)");
    printf("\n\t3. Sankha Das (2019A7PS0029P)");
    printf("\n\t4. Yash Gupta (2019A7PS1138P)");
    printf("\n\t5. Meenal Gupta (2019A7PS0243P)");
    printf("\n=====================================================\n");
    printf("\n\nProject Status:\n\n");
    printf("1. \tBoth Lexical and Syntactic Analyser are implemented.\n");
    printf("2. \tLexical Analyser is implemented using a finite state machine.\n");
    printf("3. \tSyntactic Analyser is implemented using a non-recursive descent parser.\n");
    printf("4. \tFirst and Follow sets automated.\n");
    printf("5. \tParse Table construction automated\n");
    printf("6. \tParse Tree generated successfully\n");
    printf("7. \tError recovery done using Panic mode\n");
    printf("8. \tAll modules compile successfully and work without segmentation fault.\n");
    printf("9. \tModules work with all test cases provided (t1-t6).\n");
    printf("10.\tTested with GCC 7.5.0 on Ubuntu 18.04 LTS\n");
    printf("=======================================================\n");

    while (1)
    {
        printf("\n\n\nTest a module\n\n");
        printf("0  - Quit\n");
        printf("1  - Print token list to console\n");
        printf("2  - Parse the source code to generate parse tree\n");
        printf("3  - Generate Abstract Syntax Tree\n");
        printf("4  - Find no. of nodes and allocated memory in Parse Tree and AST\n");
        printf("5  - Print symbol table\n");
        printf("6  - Find global variables\n");
        printf("7  - Find activation record sizes\n");
        printf("8  - Find record types and sizes\n");
        printf("9  - Type checking and semantic analysis\n");
        printf("10 - Intermediate Code generation and dynamic type checking\n");
        printf("11 - Assembly Code generation\n");
        printf("\nEnter a command: ");

        int option;

        scanf("%d", &option);

        switch (option)
        {
            case 0:
                return 0;

            case 1:
                buffer = init_lexer(argv[1]);
                if (buffer == NULL)
                {
                    printf("Lexer could not be initialised for \"%s\".\n", argv[1]);
                    break;
                }

                token receivedToken = get_next_token(buffer);

                printf("\n\n\n========================== LIST OF TOKENS ============================\n\n");

                while (receivedToken.type != TK_EOF)
                {
                    print_token(stdout, receivedToken);
                    receivedToken = get_next_token(buffer);
                }

                printf("\n=========================================================================\n\n");
                break;

            case 2:

                // load key map, intiate twin buffer and wait for request of a token
                buffer = init_lexer(argv[1]);
                if (buffer == NULL)
                {
                    printf("Lexer could not be initialised for \"%s\".\n", argv[1]);
                    break;
                }

                // compute first and follow sets for each non terminal in the grammar and store the information in NonTerms[MAX_NT]
                // First and Follow sets.txt files are generated by the writeFirstsToFile() and writeFollowsToFile() functions
                FirstAndFollowElement *FirstAndFollowAll = computeFirstAndFollowSets(GRAMMAR_FILE);

                // populate the grammar rules from the file -> grammarRules
                populateRules();

                // fill up the parse table
                createParseTable(FirstAndFollowAll, parseTable);

                // print and store the parse table in a .csv file
                printParseTableToFile();

                root = parseInputSourceCode(buffer, 1);

                if (printParseTree(root) == -1)
                {
                    printf("\nCould not print the parse tree.\n");
                }
                else
                {
                    printf("\nSuccessfully printed the parse tree.\n\n\n");
                }
                break;

            case 3:
                /**
                 * @brief AST generation
                 *
                 */
                buffer = init_lexer(argv[1]);
                if (buffer == NULL)
                {
                    break;
                }

                FirstAndFollowAll = computeFirstAndFollowSets(GRAMMAR_FILE);

                populateRules();

                createParseTable(FirstAndFollowAll, parseTable);

                root = parseInputSourceCode(buffer, 0);

                astRoot = createAbstractSyntaxTree(root);

                if (printAbstractSyntaxTree(astRoot, stdout) == -1)
                {
                    printf("\nCould not print the Abstract Syntax Tree.\n");
                }
                else
                {
                    printf("\nSuccessfully printed the Abstract Syntax Tree");
                } 
                break;

            case 4:
                buffer = init_lexer(argv[1]);
                if (buffer == NULL)
                {
                    break;
                }

                FirstAndFollowAll = computeFirstAndFollowSets(GRAMMAR_FILE);

                populateRules();

                createParseTable(FirstAndFollowAll, parseTable);


                numParseTreeNodes = 0;
                parseTreeMemory = 0;
                root = parseInputSourceCode(buffer, 0);
                
                printf("================================================================================================\n");
                printf("%20s%25s%30s\n", "\t\t", "NUMBER OF NODES", "MEMORY ALLOCATED");
                printf("------------------------------------------------------------------------------------------------\n");

                printf("%20s%30d%30d Bytes\n", "Parse Tree", numParseTreeNodes, parseTreeMemory);
                printf("------------------------------------------------------------------------------------------------\n");

                numASTNodes = 0;
                ASTMemory = 0;
                astRoot = createAbstractSyntaxTree(root);

                printf("%20s%30d%30d Bytes\n", "Abstract Syntax Tree", numASTNodes, ASTMemory);
                printf("------------------------------------------------------------------------------------------------\n");

                float compression = ((1.0*(numParseTreeNodes - numASTNodes))/numParseTreeNodes)*100;
                printf("Compression = %f %%\n", compression);
                printf("================================================================================================\n");

                break;

            case 5:
                /**
                 * @brief Symbol Table
                 * 
                 */
                //globalSymbolTable = performPrelims(buffer, argv[1], root, astRoot, globalSymbolTable);
                
                buffer = init_lexer(argv[1]);
                if (buffer == NULL)
                {
                    return 0;
                }

                FirstAndFollowAll = computeFirstAndFollowSets(GRAMMAR_FILE);

                populateRules();

                createParseTable(FirstAndFollowAll, parseTable);

                root = parseInputSourceCode(buffer, 0);

                //aliasTemp = NULL;
                astRoot = createAbstractSyntaxTree(root);

                globalTypeTable = createTypeTable("GLOBAL_TYPE_TABLE");

                globalSymbolTable = initializeSymbolTableNew(astRoot);

                //printGlobalTypeTable(stdout);

                printSymbolTables(stdout);
                
                break;

            case 6:
                globalSymbolTable = performPrelims(buffer, argv[1], root, astRoot, globalSymbolTable);
            
                printf("================== LIST OF GLOBAL VARIABLES ================\n\n");
                printf("%10s%20s%20s\n", "NAME", "TYPE", "OFFSET");
                printf("-----------------------------------------------------------\n");
                for(int i = 0; i < K_MAP_SIZE; i++){
                    SymbolTableEntry* tableEntry = globalSymbolTable->tableEntries[i];
                    while(tableEntry != NULL){
                        printf("%10s%20s%20d\n", tableEntry->identifier, getType(tableEntry), tableEntry->offset);
                        tableEntry = tableEntry->next;
                    }
                }
                printf("==========================================================\n");

                break;

            case 7:
                globalSymbolTable = performPrelims(buffer, argv[1], root, astRoot, globalSymbolTable);

                printf("============ FUNCTION ACTIVATION RECORD SIZES ============\n\n");
                printf("%10s%35s\n", "NAME", "SIZE");
                printf("---------------------------------------------------------\n");

                SymbolTable* head = listOfSymbolTables;
                while(head != NULL){
                    if(strcmp(head->tableID, "GLOBAL")){    // do not print the global symbol table details
                        printf("%10s%30d Bytes\n", head->tableID, head->totalWidth);
                    }

                    head = head->next;
                }

                printf("==========================================================\n");
                break;

            case 8:
                globalSymbolTable = performPrelims(buffer, argv[1], root, astRoot, globalSymbolTable);
                printGlobalTypeTableRecordsAndUnions(stdout);
                break;

            case 9:
                
                funcSeqNum = 0;

                buffer = init_lexer(argv[1]);
                if (buffer == NULL)
                {
                    return 0;
                }

                FirstAndFollowAll = computeFirstAndFollowSets(GRAMMAR_FILE);

                populateRules();

                createParseTable(FirstAndFollowAll, parseTable);

                root = parseInputSourceCode(buffer, 0);

                astRoot = createAbstractSyntaxTree(root);

                globalTypeTable = createTypeTable("GLOBAL_TYPE_TABLE");

                globalSymbolTable = initializeSymbolTableNew(astRoot);
                
                int typeCheckingResult = typeCheck(astRoot, globalSymbolTable);

                break;

            case 10:

                funcSeqNum = 0;

                buffer = init_lexer(argv[1]);
                if (buffer == NULL)
                {
                    return 0;
                }

                FirstAndFollowAll = computeFirstAndFollowSets(GRAMMAR_FILE);

                populateRules();

                createParseTable(FirstAndFollowAll, parseTable);

                root = parseInputSourceCode(buffer, 0);

                astRoot = createAbstractSyntaxTree(root);

                globalTypeTable = createTypeTable("GLOBAL_TYPE_TABLE");

                globalSymbolTable = initializeSymbolTable(astRoot);

                printf("Starting to generate ICG code ya all.\n");

                generateCompleteICGcode(astRoot,globalSymbolTable);

                printICG(stdout);

                break;

            case 11:

                funcSeqNum = 0;

                buffer = init_lexer(argv[1]);
                if (buffer == NULL)
                {
                    return 0;
                }

                FirstAndFollowAll = computeFirstAndFollowSets(GRAMMAR_FILE);

                populateRules();

                createParseTable(FirstAndFollowAll, parseTable);

                root = parseInputSourceCode(buffer, 0);

                astRoot = createAbstractSyntaxTree(root);

                globalTypeTable = createTypeTable("GLOBAL_TYPE_TABLE");

                globalSymbolTable = initializeSymbolTable(astRoot);

                printf("Starting to generate ICG code ya all.\n");

                generateCompleteICGcode(astRoot,globalSymbolTable);

                printICG(stdout);

                printf("\n\n\n\n");

                generateAssemblyCode(stdout,globalSymbolTable);

            default:
                break;
        }
    }
    return 0;
}