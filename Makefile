CC=gcc
CFLAGS=-I.

lexer: keywordMap.c keywordMap.h lexer.c lexer.h buffer.c buffer.h lexerDef.h
	$(CC) driver.c keywordMap.h keywordMap.c lexer.c lexer.h buffer.c buffer.h lexerDef.h -o lexify
