//
// Created by tika on 24-6-18.
//

#include "protos.h"

// current file and location
FILE *in_file;
char *cur_file;
int cur_line = 1;
int cur_col = 1;

static Pos *new_pos() {
    Pos *pos = malloc(sizeof(Pos));
    if (!pos) {
        error(format_str("%s in %s", MALLOC_FAIL, __func__));
        return NULL;
    }

    pos->file = cur_file;
    pos->line = cur_line;
    pos->col = cur_col;

    return pos;
}

static char *str_pos(Pos *pos) {
    char *str;
    if (pos) {
        str = format_str(" at %s:%d, %d", pos->file, pos->line, pos->col);
        free(pos);
    } else {
        str = "";
    }

    return str;
}

static Token *new_token() {
    Token *token = malloc(sizeof(Token));
    if (!token) {
        fatal(format_str("%s in %s%s", MALLOC_FAIL, __func__, str_pos(new_pos())));
    }
    token->next = NULL;

    return token;
}

static Token *get_token() {
    Token *token = new_token();

    // scan file and gen token

    return NULL;
}

Token *lex(char *file) {
    Token *token_head = NULL;
    Token *token_tail = NULL;
    Token *token;

    if (!(in_file = fopen(file, "r"))) {
        fatal(format_str("Unable to open %s: %s", file, strerror(errno)));
    }

    while (TRUE) {
        token = get_token();
        if (!token || token->type == T_EOF) {
            break;
        }

        // get a new token
        if (token_head) {
            token_tail->next = token;
            token_tail = token;
        } else {
            token_head = token_tail = token;
        }
    }

    return token_head;
}
