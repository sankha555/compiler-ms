CC=gcc
CFLAGS=-I.

lexer: keywordMap.c keywordMap.h lexer.c lexer.h buffer.c buffer.h token.h
	$(CC) frontend.c keywordMap.h keywordMap.c lexer.c lexer.h buffer.c buffer.h token.h -o lexify
	./lexify tests/input.spl

