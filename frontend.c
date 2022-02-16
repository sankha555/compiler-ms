#include<stdio.h>
#include<stdlib.h>
#include "token.h"
#include "lexer.h"

int main(int argc, char *argv[]) {
    // The first argument is the file name
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Open the file
    FILE *fp = fopen(argv[1], "r");

    // Check if the file is open
    if (fp == NULL) {
        printf("Error: File %s could not be opened\n", argv[1]);
        return 1;
    }

    // Initialize the lexer
    init_lexer(fp);

    token t;

    // Output file lex.yy.spl
    FILE *output = fopen("tests/lex.yy.spl", "w");

    do  {
        t = get_next_token();
        print_token(output, t);
    } while (t.type != TK_EOF);

    fclose(output);
    fclose(fp);

    return 0;
}