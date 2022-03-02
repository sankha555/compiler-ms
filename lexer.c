#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexerDef.h"
#include "keywordMap.h"
#include "buffer.h"
#include "globalDef.h"

keyMap *table;

char* tokenNames[] = {
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
    "TK_ELSE",     // till here all keywords tokens are mentioned first to
    "TK_ASSIGNOP", // make loading of hasmap easier, keep this in mind while modifying keywords
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

/*
 * Function: remove_comments
 * --------------------
 *  removes comments from the input file (buffer)
 *
 *  buffer: buffer to be processed
 *
 *  returns: void / prints ouptut to console
 */
void remove_comments(twinBuffer *buffer)
{
    while (1)
    {
        char c = getch(buffer);
        if (c == '%')
        {
            while (c != '\n' && c != '\0')
            {
                c = getch(buffer);
            }
        }
        else if (c == '\0')
        {
            break;
        }
        else
        {
            printf("%c", c);
        }
    }
}

token get_next_token(twinBuffer *buffer)
{
    token t;
    char c;

    int state = 0;

    while (1)
    {
        c = getch(buffer);
        switch (state)
        {
        case 0:
            switch (c)
            {
            case ' ':
            case '\t':
                break;
            case '\n':
                linenumber++;
                break;
            case '%':
                while (1)
                {
                    c = getch(buffer);
                    if (c == '\n')
                    {
                        linenumber++;
                        break;
                    }
                    if (c == '\0')
                        break;
                }
                break;
            case '\0':
                t.type = TK_EOF;
                t.lexeme = "$";
                t.linenumber = linenumber;
                return t;
            case '~':
                t.type = TK_NOT;
                t.lexeme = "~";
                t.linenumber = linenumber;
                return t;
            case '@':
                state = 1;
                break;
            case '&':
                state = 3;
                break;
            case '/':
                t.type = TK_DIV;
                t.lexeme = "/";
                t.linenumber = linenumber;
                return t;
            case '*':
                t.type = TK_MUL;
                t.lexeme = "*";
                t.linenumber = linenumber;
                return t;
            case '+':
                t.type = TK_PLUS;
                t.lexeme = "+";
                t.linenumber = linenumber;
                return t;
            case '-':
                t.type = TK_MINUS;
                t.lexeme = "-";
                t.linenumber = linenumber;
                return t;
            case '(':
                t.type = TK_OP;
                t.lexeme = "(";
                t.linenumber = linenumber;
                return t;
            case ')':
                t.type = TK_CL;
                t.lexeme = ")";
                t.linenumber = linenumber;
                return t;
            case '.':
                t.type = TK_DOT;
                t.lexeme = ".";
                t.linenumber = linenumber;
                return t;
            case ':':
                t.type = TK_COLON;
                t.lexeme = ":";
                t.linenumber = linenumber;
                return t;
            case ';':
                t.type = TK_SEM;
                t.lexeme = ";";
                t.linenumber = linenumber;
                return t;
            case ',':
                t.type = TK_COMMA;
                t.lexeme = ",";
                t.linenumber = linenumber;
                return t;
            case '[':
                t.type = TK_SQL;
                t.lexeme = "[";
                t.linenumber = linenumber;
                return t;
            case ']':
                t.type = TK_SQR;
                t.lexeme = "]";
                t.linenumber = linenumber;
                return t;
            case '=':
                state = 5;
                break;
            case '!':
                state = 6;
                break;
            case '<':
                state = 7;
                break;
            case '>':
                state = 10;
                break;
            case '#':
                state = 11;
                break;
            case '_':
                state = 12;
                break;
            default:
                if (isdigit(c)) // NUM and RNUM
                {
                    char *lexeme = malloc(sizeof(char) * 2);
                    lexeme[0] = c;

                    while (c = getch(buffer), isdigit(c))
                    {
                        strncat(lexeme, &c, 1);
                    }

                    if (c != '.')
                    {
                        t.type = TK_NUM;
                        t.lexeme = lexeme;
                        t.linenumber = linenumber;
                        ungetch(buffer);
                        return t;
                    }

                    strncat(lexeme, &c, 1);
                    c = getch(buffer);

                    // the case of (0-9)(0-9)*.(other) requires two retractions
                    if (!isdigit(c))
                    {
                        ungetch(buffer);

                        t.type = TK_ERROR;
                        int len = strlen(lexeme) + 19;
                        t.lexeme = (char *) malloc(sizeof(char) * len);
                        snprintf(t.lexeme, len, "Unknown Pattern <%s>", lexeme);
                        t.linenumber = linenumber;
                        return t;
                    }

                    strncat(lexeme, &c, 1);
                    c = getch(buffer);

                    // the case of (0-9)(0-9)*.(0-9)(other) requires two retractions
                    if (!isdigit(c))
                    {
                        ungetch(buffer);

                        t.type = TK_ERROR;
                        int len = strlen(lexeme) + 19;
                        t.lexeme = (char *) malloc(sizeof(char) * len);
                        snprintf(t.lexeme, len, "Unknown Pattern <%s>", lexeme);
                        t.linenumber = linenumber;
                        return t;
                    }

                    strncat(lexeme, &c, 1);

                    c = getch(buffer);

                    if (c != 'E')
                    {
                        t.type = TK_RNUM;
                        t.lexeme = lexeme;
                        t.linenumber = linenumber;
                        ungetch(buffer);
                        return t;
                    }

                    strncat(lexeme, &c, 1);

                    c = getch(buffer);

                    if (c == '+' || c == '-')
                    {
                        strncat(lexeme, &c, 1);
                        c = getch(buffer);
                    }

                    if (!isdigit(c))
                    {
                        ungetch(buffer);

                        t.type = TK_ERROR;
                        int len = strlen(lexeme) + 19;
                        t.lexeme = (char *) malloc(sizeof(char) * len);
                        snprintf(t.lexeme, len, "Unknown Pattern <%s>", lexeme);
                        t.linenumber = linenumber;
                        return t;
                    }

                    strncat(lexeme, &c, 1);

                    c = getch(buffer);

                    if (!isdigit(c))
                    {
                        ungetch(buffer);

                        t.type = TK_ERROR;
                        int len = strlen(lexeme) + 19;
                        t.lexeme = (char *) malloc(sizeof(char) * len);
                        snprintf(t.lexeme, len, "Unknown Pattern <%s>", lexeme);
                        t.linenumber = linenumber;
                        return t;
                    }

                    strncat(lexeme, &c, 1);

                    t.type = TK_RNUM;
                    t.lexeme = lexeme;
                    t.linenumber = linenumber;
                    return t;
                }
                else if (isalpha(c) && islower(c)) // keywords, identifiers and field names
                {
                    char *lexeme = malloc(sizeof(char) * 100);
                    lexeme[0] = c;

                    if (c == 'b' || c == 'c' || c == 'd')
                    {
                        c = getch(buffer);

                        if (isdigit(c) && c >= '2' && c <= '7')
                        {
                            strncat(lexeme, &c, 1);

                            while (c = getch(buffer), isalpha(c) && c >= 'b' && c <= 'd')
                            {
                                strncat(lexeme, &c, 1);
                            }

                            if (isdigit(c) && c >= '2' && c <= '7')
                            {
                                strncat(lexeme, &c, 1);

                                while (c = getch(buffer), isdigit(c) && c >= '2' && c <= '7')
                                {
                                    strncat(lexeme, &c, 1);
                                }
                            }

                            if (strlen(lexeme) > 20)
                            {
                                t.type = TK_ERROR;
                                t.lexeme = "Variable Identifier is longer than the prescribed length of 20 characters.";
                                t.linenumber = linenumber;   
                                ungetch(buffer);
                                return t;
                            }

                            t.type = TK_ID;
                            t.lexeme = lexeme;
                            t.linenumber = linenumber;
                            ungetch(buffer);
                            return t;
                        }
                    }

                    while (c = getch(buffer), isalpha(c) && islower(c))
                    {
                        strncat(lexeme, &c, 1);
                    }

                    int tag = search(table, lexeme);

                    if (tag != -1)
                        t.type = tag;
                    else
                        t.type = TK_FIELDID;

                    t.lexeme = lexeme;
                    t.linenumber = linenumber;
                    ungetch(buffer);
                    return t;
                }
                else
                {
                    t.lexeme = (char *) malloc(sizeof(char)*20);
                    snprintf(t.lexeme, 19, "Unknown Symbol <%c>", c);
                    t.type = TK_ERROR;
                    t.linenumber = linenumber;
                    return t;
                }
            }
            break;
        case 1:
            switch (c)
            {
            case '@':
                state = 2;
                break;
            default:
                t.type = TK_ERROR;
                t.lexeme = "Unknown Symbol <@>";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 2:
            switch (c)
            {
            case '@':
                t.type = TK_OR;
                t.lexeme = "@@@";
                t.linenumber = linenumber;
                return t;
            default:
                t.type = TK_ERROR;
                t.lexeme = "Unknown Pattern <@@>";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 3:
            switch (c)
            {
            case '&':
                state = 4;
                break;
            default:
                t.type = TK_ERROR;
                t.lexeme = "Unknown Symbol <&>";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 4:
            switch (c)
            {
            case '&':
                t.type = TK_AND;
                t.lexeme = "&&&";
                t.linenumber = linenumber;
                return t;
            default:
                t.type = TK_ERROR;
                t.lexeme = "Unknown Pattern <&&>";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 5:
            switch (c)
            {
            case '=':
                t.type = TK_EQ;
                t.lexeme = "==";
                t.linenumber = linenumber;
                return t;
            default:
                t.type = TK_ERROR;
                t.lexeme = "Unknown Symbol <=>";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 6:
            switch (c)
            {
            case '=':
                t.type = TK_NE;
                t.lexeme = "!=";
                t.linenumber = linenumber;
                return t;
            default:
                t.type = TK_ERROR;
                t.lexeme = "Unknown Symbol <!>";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 7:
            switch (c)
            {
            case '=':
                t.type = TK_LE;
                t.lexeme = "<=";
                t.linenumber = linenumber;
                return t;
            case '-':
                state = 8;
                break;
            default:
                t.type = TK_LE;
                t.lexeme = "<";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 8:
            switch (c)
            {
            case '-':
                state = 9;
                break;
            default:
                t.type = TK_ERROR;
                t.lexeme = "Unknown Pattern <<->";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 9:
            switch (c)
            {
            case '-':
                t.type = TK_ASSIGNOP;
                t.lexeme = "<---";
                t.linenumber = linenumber;
                return t;
            default:
                t.type = TK_ERROR;
                t.lexeme = "Unknown Pattern <<-->";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 10:
            switch (c)
            {
            case '=':
                t.type = TK_GE;
                t.lexeme = ">=";
                t.linenumber = linenumber;
                return t;
            default:
                t.type = TK_GT;
                t.lexeme = ">";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            break;
        case 11:;
            char *lexeme = malloc(sizeof(char) * 2);
            lexeme[0] = '#';

            if (!(isalpha(c) && islower(c)))
            {
                t.type = TK_ERROR;
                t.lexeme = "Unknown Symbol <#>";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            strncat(lexeme, &c, 1);

            while (c = getch(buffer), isalpha(c) && islower(c))
            {
                strncat(lexeme, &c, 1);
            }

            t.type = TK_RUID;
            t.lexeme = lexeme;
            t.linenumber = linenumber;
            ungetch(buffer);
            return t;
        case 12:;
            lexeme = malloc(sizeof(char) * 2);
            lexeme[0] = '_';

            if (!isalpha(c))
            {
                t.type = TK_ERROR;
                t.lexeme = "Unknown Symbol <_>";
                t.linenumber = linenumber;
                ungetch(buffer);
                return t;
            }
            strncat(lexeme, &c, 1);

            while (c = getch(buffer), isalpha(c))
            {
                strncat(lexeme, &c, 1);
            }

            if (isdigit(c))
            {
                strncat(lexeme, &c, 1);
                while (c = getch(buffer), isdigit(c))
                {
                    strncat(lexeme, &c, 1);
                }
            }

            if (strlen(lexeme) > 30)
            {
                t.type = TK_ERROR;
                t.lexeme = "Function Identifier is longer than the prescribed length of 30 characters.";
                t.linenumber = linenumber;   
                ungetch(buffer);
                return t;
            }

            if (strcmp(lexeme, "_main") == 0)
                t.type = TK_MAIN;
            else
                t.type = TK_FUNID;

            t.lexeme = lexeme;
            t.linenumber = linenumber;
            ungetch(buffer);
            return t;
        default:
            printf("The DFA entered an invalid state - Implementation error");
            exit(0);
            break;
        }
    }
}

void print_token(FILE *out, token t)
{
    if (t.type == TK_ERROR)
        fprintf(out, "Line no. %d: Error : %s\n", t.linenumber, t.lexeme);
    else
        fprintf(out, "Line no. %d\t Lexeme %-31s\t Token %-20s\n", t.linenumber, t.lexeme, tokenNames[t.type]);
}

twinBuffer *init_lexer(FILE *fp)
{
    linenumber = 1;
    twinBuffer *buffer = init_buffer(fp);

    printf("Buffer initialized, forward pointer at %d\n", buffer->forward);

    // Initialize the keyword map
    table = (keyMap *)malloc(sizeof(keyMap));
    loadKeyMap(table, "keywords.txt");
    printf("Keyword map load status: %d\n", search(table, "global") == TK_GLOBAL);

    return buffer;
}

tokenTag tokstrToToken(char * str) {
    tokenTag i = 0;
    for(; i < NUMBER_OF_TOKENS; i++) {
        if(strcpy(str,tokenNames[i]) == 0) {
            return i;
        }
        i++;
    }
    return -1;
}