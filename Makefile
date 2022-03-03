CC=gcc
CFLAGS=-I.

lexer: keywordMap.c keywordMap.h lexer.c lexer.h buffer.c buffer.h lexerDef.h
	$(CC) driver.c keywordMap.h keywordMap.c lexer.c lexer.h buffer.c buffer.h lexerDef.h

parser: driver.c parser.c parser.h parserDef.h FirstAndFollow.h FirstAndFollow.c lexer.h lexerDef.h lexer.c buffer.c buffer.h keywordMap.h keywordMap.c
	$(CC) parser.c parser.h parserDef.h FirstAndFollow.h FirstAndFollow.c lexer.h lexerDef.h lexer.c buffer.c buffer.h keywordMap.h keywordMap.c driver.c