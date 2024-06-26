//
// Created by tika on 24-6-18.
//

#include "protos.h"

char *pos_to_str(Pos *pos) {
    char *str;
    if (pos) {
        str = format_str(" at %s:%d, %d", pos->file, pos->line, pos->col);
    } else {
        str = "";
    }

    return str;
}

int char_index(char *s, int c) {
    char *p;

    p = strchr(s, c);
    return p ? (int) (p - s) : NOT_FOUND;
}

char *format_str(char *fmt, ...) {
    char *str = calloc(MAX_TEXT, sizeof(char));
    if (!str) {
        error("Out of memory.");
    }

    va_list args;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);

    return str;
}

static void message(int level, char *str) {
    switch (level) {
        case M_DEBUG:
            fprintf(stdout, "[DEBUG] %s.\n", str);
            break;
        case M_INFO:
            fprintf(stdout, "[INFO] %s.\n", str);
        case M_WARN:
            fprintf(stderr, "[WARN] %s.\n", str);
            break;
        case M_ERROR:
            fprintf(stderr, "[ERROR] %s.\n", str);
            break;
        case M_FATAL:
            fprintf(stderr, "[FATAL] %s.\n", str);
            exit(1);
        default:
            break;
    }
}

void debug(char *str) {
    message(M_DEBUG, str);
}

void info(char *str) {
    message(M_INFO, str);
}

void warn(char *str) {
    message(M_WARN, str);
}

void error(char *str) {
    message(M_ERROR, str);
}

void fatal(char *str) {
    message(M_FATAL, str);
}
