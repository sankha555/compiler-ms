#include<stdio.h>
#include<stdlib.h>
#include "lexer.h"
#include "buffer.h"

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
            token t = get_next_token(buffer);
            while (t.type != TK_EOF) {
                print_token(stdout, t);
                t = get_next_token(buffer);
            }

            // Close the file
            fclose(fp);
            break;
        default:
            break;
        }
    }
    return 0;
}