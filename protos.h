//
// Created by tika on 24-6-18.
//

/*
 * All prototypes are defined here.
 * This file is included by all files in the project.
 */

#ifndef MCC_PROTOS_H
#define MCC_PROTOS_H

#include "defs.h"

// lexer.c
Token *lex(char *file);

// parser.c
ASTNode *parse(Token *tokens);

// gen.c
void gen(ASTNode *node);

// tool.c
char *format_str(char *fmt, ...);

void debug(char *str);

void info(char *str);

void warn(char *str);

void error(char *str);

void fatal(char *str);

#endif //MCC_PROTOS_H
