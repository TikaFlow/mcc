//
// Created by tika on 24-6-18.
//

/*
 * All global includes/defines/types are defined here
 */

#ifndef MCC_DEFS_H
#define MCC_DEFS_H

// includes
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// defines
#define TRUE 1
#define FALSE 0
#define MALLOC_FAIL "Unable to allocate memory"
#define MAX_TEXT 1024

// types
enum {
    M_DEBUG,
    M_INFO,
    M_WARN,
    M_ERROR,
    M_FATAL,
};

enum {
    T_EOF,
};

typedef struct {
    char *file;
    int line;
    int col;
} Pos;

typedef struct token {
    int type;
    char *type_name;
    union {
        long v_int;
        double v_float;
        char *v_string;
    };
    Pos *pos; // where the token from
    struct token *next;
} Token;

typedef struct astnode {
    int type;
    struct astnode *left;
    struct astnode *right;
    Pos *pos; // where the node from
} ASTNode;

#endif //MCC_DEFS_H
