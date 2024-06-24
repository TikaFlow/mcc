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
#include <ctype.h>

// defines
#define TRUE 1
#define FALSE 0
#define NOT_FOUND (-1)
#define MAX_TEXT 256
#define MAX_STR_LEN (256 * 1024)
#define HEX_DIGIT "0123456789abcdef"
#define FLOAT_DIGIT "0123456789.e+-"
#define SUFFIX_DIGIT "ulf"
#define MALLOC_FAIL "Unable to allocate memory"
#define NO_INPUT_FILE "No input file specified"
#define MULTIPLE_INPUT_FILES "Multiple input files, only the last one takes effect"
#define UNKNOWN_CHAR "Unexpected character"
#define INVALID_SUFFIX "Invalid suffix"
#define EXPONENT_NO_DIGITS "Exponent has no digits"
#define UNCLOSED_CHAR "Unclosed character literal"
#define UNCLOSED_STRING "Unclosed string literal"

// typedefs
typedef int bool;
typedef unsigned char byte;

// types
enum LogLevel {
    M_DEBUG,
    M_INFO,
    M_WARN,
    M_ERROR,
    M_FATAL,
};

enum TokenType {
    T_EOF = 256,
    // operators // precedence
    T_COMMA, // 1
    T_ASSIGN, T_ADD_ASSIGN, T_SUB_ASSIGN, T_MUL_ASSIGN, T_DIV_ASSIGN, T_MOD_ASSIGN, // 2
    T_LEFT_ASSIGN, T_RIGHT_ASSIGN, T_AND_ASSIGN, T_XOR_ASSIGN, T_OR_ASSIGN,
    T_QUESTION, // 3
    T_LOG_OR, // 4
    T_LOG_AND, // 5
    T_BIT_OR, // 6
    T_BIT_XOR, // 7
    T_BIT_AND, // 8
    T_EQUAL, T_NOT_EQUAL, // 9
    T_LESS, T_LESS_EQUAL, T_GREATER, T_GREATER_EQUAL, // 10
    T_LEFT, T_RIGHT, // 11
    T_PLUS, T_MINUS, // 12
    T_STAR, T_SLASH, T_MOD, // 13
    T_INC, T_DEC, T_LOG_NOT, T_BIT_NOT, // 14
    T_LPAREN, T_RPAREN, T_LBRACKET, T_RBRACKET, T_DOT, T_ARROW, // 15
    // other op
    T_COLON, T_SEMI, T_LBRACE, T_RBRACE,
    // type
    T_VOID, T_CHAR, T_SHORT, T_INT, T_LONG, T_FLOAT, T_DOUBLE,
    // keywords
    T_AUTO, T_STATIC, T_EXTERN, T_REGISTER, T_CONST, T_VOLATILE, T_SIGNED, T_UNSIGNED, T_SIZEOF,
    T_TYPEDEF, T_STRUCT, T_UNION, T_ENUM,
    T_IF, T_ELSE, T_SWITCH, T_CASE, T_DEFAULT, T_WHILE, T_DO, T_FOR, T_GOTO, T_BREAK, T_CONTINUE, T_RETURN,
    // other
    T_IDENTIFIER, T_STRING,
    T_INTEGER, T_INTEGER_U, T_INTEGER_L, T_INTEGER_UL, T_INTEGER_LL, T_INTEGER_ULL,
    T_FLOATING, T_FLOATING_F, T_FLOATING_L,
};

enum LiteralType {
    L_NONE,
    L_CHAR, L_BINARY, L_OCTAL, L_HEXADECIMAL,
    L_STRING = T_STRING,
    L_INTEGER, L_INTEGER_U, L_INTEGER_L, L_INTEGER_UL, L_INTEGER_LL, L_INTEGER_ULL,
    L_FLOAT, L_FLOAT_F, L_FLOAT_L,
};

typedef struct pos {
    char *file;
    int line;
    int col;
} Pos;

typedef struct token {
    int type;
    char *type_name; // type desc
    union {
        long v_int; // integer value
        double v_float; // float value
        char *v_string; // identifier name or string literal
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
