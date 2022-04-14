CC=gcc
CFLAGS=-I.

all: driver.c parser.c parser.h parserDef.h FirstAndFollow.h FirstAndFollow.c lexer.h lexerDef.h lexer.c buffer.c buffer.h keywordMap.h keywordMap.c astDef.h astGenerator.h astGenerator.c symbolTable.h symbolTable.c symbolTableDef.h typing.h typing.c TypeChecker.c TypeChecker.h SemanticAnalyzer.c SemanticAnalyzer.h
	$(CC) parser.c parser.h parserDef.h FirstAndFollow.h FirstAndFollow.c lexer.h lexerDef.h lexer.c buffer.c buffer.h keywordMap.h keywordMap.c stack.h stack.c driver.c astDef.h astGenerator.h astGenerator.c symbolTable.h symbolTable.c symbolTableDef.h typing.h typing.c TypeChecker.c TypeChecker.h SemanticAnalyzer.c SemanticAnalyzer.h
