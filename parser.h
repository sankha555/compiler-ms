#ifndef PARSER_H
#define PARSER_H

#include "FirstAndFollow.h"
#include "globalDef.h"

void populateRules();

void createParseTable();

void writeParseTableToFile();

#endif

