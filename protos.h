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

// analyzer.c
ASTNode *analyze(ASTNode *node);

// gen.c
void gen(ASTNode *node);

// tool.c
char *pos_to_str(Pos *pos);

int char_index(char *s, int c);

char *format_str(char *fmt, ...);

void debug(char *str);

void info(char *str);

void warn(char *str);

void error(char *str);

void fatal(char *str);

#endif //MCC_PROTOS_H
