#include<stdio.h>
#include<stdlib.h>
#include "lexer.h"
#include "buffer.h"
#include "FirstAndFollow.h"
#include "parserDef.h"
#include "parser.h"


int main(int argc, char *argv[]) {
    // The first argument is the file name
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    twinBuffer *buffer;
    FILE *fp;

    while (1) {
        printf("CS F303 Compiler\n");
        printf("Enter a command: ");
        printf("0 - quit\n");
        printf("1 - removal of comments and print to console\n");
        printf("2 - print token list to console\n");

        int option;

        scanf("%d", &option);

        switch (option)
        {
        case 0:
            return 0;
        case 1:
            // Open the file
            fp = fopen(argv[1], "r");

            // Check if the file is open
            if (fp == NULL) {
                printf("Error: File %s could not be opened\n", argv[1]);
                return 1;
            }

            buffer = init_buffer(fp);
            remove_comments(buffer);

            // Close the file
            fclose(fp);
            break;
        case 2:
            // Open the file
            fp = fopen(argv[1], "r");

            // Check if the file is open
            if (fp == NULL) {
                printf("Error: File %s could not be opened\n", argv[1]);
                return 1;
            }

            buffer = init_lexer(fp);
            token t;
            do {
                t = get_next_token(buffer);
                print_token(stdout, t);
            } while (t.type != TK_EOF);

            // Close the file
            fclose(fp);
            break;
        case 3:
            //open the file
            fp = fopen(argv[1],"r");

            // Check if the file is open
            if (fp == NULL) {
                printf("Error: File %s could not be opened\n", argv[1]);
                return 1;
            }

            //load key map, intiate twin buffer and wait for request of a token
            buffer = init_lexer(fp);

            //compute first and follow sets for each non terminal in the grammar and store the information in NonTerms[MAX_NT]
            computeFirstAndFollowSets(GRAMMAR_FILE);

            //populate the grammar rules from the file -> grammarRules 
            populateRules();
            

            break;
        default:
            break;
        }
    }
    return 0;
}