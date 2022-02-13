#include "lexer.h"

int linenumber;
keyMap* table;

char *make_string(char *str)
{
    char *s = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(s, str);
    return s;
}

token get_next_token()
{
    token t;
    char c;

    int state = 0;

    while (1)
    {
        c = getch();
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
                    c = getch();
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
                t.lexeme = make_string("");
                t.linenumber = linenumber;
                return t;
            case '~':
                t.type = TK_NOT;
                t.lexeme = make_string("~");
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
                t.lexeme = make_string("/");
                t.linenumber = linenumber;
                return t;
            case '*':
                t.type = TK_MUL;
                t.lexeme = make_string("*");
                t.linenumber = linenumber;
                return t;
            case '+':
                t.type = TK_PLUS;
                t.lexeme = make_string("+");
                t.linenumber = linenumber;
                return t;
            case '-':
                t.type = TK_MINUS;
                t.lexeme = make_string("-");
                t.linenumber = linenumber;
                return t;
            case '(':
                t.type = TK_OP;
                t.lexeme = make_string("(");
                t.linenumber = linenumber;
                return t;
            case ')':
                t.type = TK_CL;
                t.lexeme = make_string(")");
                t.linenumber = linenumber;
                return t;
            case '.':
                t.type = TK_DOT;
                t.lexeme = make_string(".");
                t.linenumber = linenumber;
                return t;
            case ':':
                t.type = TK_COLON;
                t.lexeme = make_string(":");
                t.linenumber = linenumber;
                return t;
            case ';':
                t.type = TK_SEM;
                t.lexeme = make_string(";");
                t.linenumber = linenumber;
                return t;
            case ',':
                t.type = TK_COMMA;
                t.lexeme = make_string(".");
                t.linenumber = linenumber;
                return t;
            case '[':
                t.type = TK_SQL;
                t.lexeme = make_string("[");
                t.linenumber = linenumber;
                return t;
            case ']':
                t.type = TK_SQR;
                t.lexeme = make_string("]");
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

                    while (c = getch(), isdigit(c))
                    {
                        strncat(lexeme, &c, 1);
                    }

                    if (c != '.')
                    {
                        t.type = TK_NUM;
                        t.lexeme = lexeme;
                        t.linenumber = linenumber;
                        ungetch(c);
                        return t;
                    }

                    char *fractional = malloc(sizeof(char) * 2);
                    fractional[0] = c;

                    c = getch();

                    // the case of (0-9)(0-9)*.(other) requires two retractions
                    if (!isdigit(c))
                    {
                        c = ungetch(c);
                        ungetch(c);

                        t.type = TK_NUM;
                        t.lexeme = lexeme;
                        t.linenumber = linenumber;
                        return t;
                    }

                    strncat(fractional, &c, 1);
                    c = getch();

                    // the case of (0-9)(0-9)*.(0-9)(other) requires two retractions
                    if (!isdigit(c))
                    {
                        c = ungetch(c);
                        c = ungetch(c);
                        ungetch(c);

                        t.type = TK_NUM;
                        t.lexeme = lexeme;
                        t.linenumber = linenumber;
                        return t;
                    }

                    strncat(fractional, &c, 1);
                    strncat(lexeme, fractional, strlen(fractional));

                    c = getch();

                    if (c != 'E')
                    {
                        t.type = TK_RNUM;
                        t.lexeme = lexeme;
                        t.linenumber = linenumber;
                        ungetch(c);
                        return t;
                    }

                    char *exponent = malloc(sizeof(char) * 2);
                    exponent[0] = c;

                    c = getch();

                    if (c == '+' || c == '-')
                    {
                        strncat(exponent, &c, 1);
                        c = getch();
                    }

                    if (!isdigit(c))
                    {
                        c = ungetch(c);
                        if (c != 'E')
                            c = ungetch(c);

                        t.type = TK_RNUM;
                        t.lexeme = lexeme;
                        t.linenumber = linenumber;
                        return t;
                    }

                    strncat(exponent, &c, 1);

                    c = getch();

                    if (!isdigit(c))
                    {
                        c = ungetch(c);
                        c = ungetch(c);
                        if (c != 'E')
                            c = ungetch(c);

                        t.type = TK_RNUM;
                        t.lexeme = lexeme;
                        t.linenumber = linenumber;
                        return t;
                    }

                    strncat(exponent, &c, 1);
                    strncat(lexeme, exponent, strlen(exponent));

                    t.type = TK_RNUM;
                    t.lexeme = lexeme;
                    t.linenumber = linenumber;
                    return t;
                }
                else if (isalpha(c) && islower(c)) // keywords, identifiers and field names
                {
                    char* lexeme = malloc(sizeof(char) * 2);
                    lexeme[0] = c;

                    if (c == 'b' || c == 'c' || c == 'd') {
                        c = getch();

                        if (isdigit(c) && c >= '2' && c <= '7') {
                            strncat(lexeme, &c, 1);

                            while (c = getch(), isalpha(c) && c >= 'b' && c <= 'd') {
                                strncat(lexeme, &c, 1);
                            }

                            if (isdigit(c) && c >= '2' && c <= '7') {
                                strncat(lexeme, &c, 1);

                                while (c = getch(), isdigit(c) && c >= '2' && c <= '7') {
                                    strncat(lexeme, &c, 1);
                                }
                            }

                            if (strlen(lexeme) > 20) {
                                printf("Lexical error at line %d because identifier length exceeds 20", linenumber);
                                exit(1);
                            }

                            t.type = TK_ID;
                            t.lexeme = lexeme;
                            t.linenumber = linenumber;
                            ungetch(c);
                            return t;
                        }
                    }

                    while (c = getch(), isalpha(c) && islower(c))
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
                    ungetch(c);
                    return t;
                }
                else
                {
                    printf("Lexical error at line %d due to invalid token %c", linenumber, c);
                    exit(1);
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
                printf("Lexical error at line %d", linenumber);
                exit(1);
                break;
            }
            break;
        case 2:
            switch (c)
            {
            case '@':
                t.type = TK_OR;
                t.lexeme = make_string("@@@");
                t.linenumber = linenumber;
                return t;
            default:
                printf("Lexical error at line %d", linenumber);
                exit(1);
                break;
            }
            break;
        case 3:
            switch (c)
            {
            case '&':
                state = 4;
                break;
            default:
                printf("Lexical error at line %d", linenumber);
                exit(1);
                break;
            }
            break;
        case 4:
            switch (c)
            {
            case '&':
                t.type = TK_AND;
                t.lexeme = make_string("&&&");
                t.linenumber = linenumber;
                return t;
            default:
                printf("Lexical error at line %d", linenumber);
                exit(1);
                break;
            }
            break;
        case 5:
            switch (c)
            {
            case '=':
                t.type = TK_EQ;
                t.lexeme = make_string("==");
                t.linenumber = linenumber;
                return t;
            default:
                printf("Lexical error at line %d", linenumber);
                exit(1);
                break;
            }
            break;
        case 6:
            switch (c)
            {
            case '=':
                t.type = TK_NE;
                t.lexeme = make_string("!=");
                t.linenumber = linenumber;
                return t;
            default:
                printf("Lexical error at line %d", linenumber);
                exit(1);
                break;
            }
            break;
        case 7:
            switch (c)
            {
            case '=':
                t.type = TK_LE;
                t.lexeme = make_string("<=");
                t.linenumber = linenumber;
                return t;
            case '-':
                state = 8;
                break;
            default:
                t.type = TK_LE;
                t.lexeme = make_string("<");
                t.linenumber = linenumber;
                ungetch(c);
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
                printf("Lexical error at line %d", linenumber);
                exit(1);
            }
            break;
        case 9:
            switch (c)
            {
            case '-':
                t.type = TK_ASSIGNOP;
                t.lexeme = make_string("<---");
                t.linenumber = linenumber;
                return t;
            default:
                break;
            }
            break;
        case 10:
            switch (c)
            {
            case '=':
                t.type = TK_GE;
                t.lexeme = make_string(">=");
                t.linenumber = linenumber;
                return t;
            default:
                t.type = TK_GT;
                t.lexeme = make_string(">");
                t.linenumber = linenumber;
                ungetch(c);
                return t;
            }
            break;
        case 11:
            char* lexeme = malloc(sizeof(char) * 2);
            lexeme[0] = '#';

            if (!(isalpha(c) && islower(c)))
            {
                printf("Lexical error at line %d", linenumber);
                exit(1);
            }
            strncat(lexeme, &c, 1);

            while (c = getch(), isalpha(c) && islower(c))
            {
                strncat(lexeme, &c, 1);
            }

            t.type = TK_RUID;
            t.lexeme = lexeme;
            t.linenumber = linenumber;
            ungetch(c);
            return t;
        case 12:
            char* lexeme = malloc(sizeof(char) * 2);
            lexeme[0] = '_';

            if (!isalpha(c))
            {
                printf("Lexical error at line %d", linenumber);
                exit(1);
            }
            strncat(lexeme, &c, 1);

            while (c = getch(), isalpha(c))
            {
                strncat(lexeme, &c, 1);
            }

            if (isdigit(c)) {
                strncat(lexeme, &c, 1);
                while (c = getch(), isdigit(c)) {
                    strncat(lexeme, &c, 1);
                }                
            }

            if (strlen(lexeme) > 30) {
                printf("Lexical error at line %d as function identifier length exceeds 30", linenumber);
                exit(1);
            }

            if (strcmp(lexeme, "_main") == 0)
                t.type = TK_MAIN;
            else
                t.type = TK_FUNID;

            t.lexeme = lexeme;
            t.linenumber = linenumber;
            ungetch(c);
            return t;
        default:
            printf("The DFA entered an invalid state - Implementation error");
            exit(0);
            break;
        }
    }
}

void init_lexer(FILE *fp)
{
    linenumber = 1;
    initializeBuffer(fp);
}