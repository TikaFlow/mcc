//
// Created by tika on 24-6-18.
//

#include "protos.h"

// current file and location
static FILE *in_file;
static Pos cur_pos;

static Pos *new_pos() {
    Pos *pos = calloc(1, sizeof(Pos));
    if (!pos) {
        error(format_str("%s in %s", MALLOC_FAIL, __func__));
        return NULL;
    }

    memcpy(pos, &cur_pos, sizeof(Pos));

    return pos;
}

static Token *new_token() {
    Token *token = calloc(1, sizeof(Token));
    if (!token) {
        fatal(format_str("%s in %s", MALLOC_FAIL, __func__));
    }
    token->type_name = "head";

    return token;
}

static int next() {
    int c = fgetc(in_file);

    if (c == '\n') {
        cur_pos.line++;
        cur_pos.col = 0;
    } else {
        cur_pos.col++;
    }

    return c;
}

static int peek() {
    int c = fgetc(in_file);
    ungetc(c, in_file);
    return c;
}

static void decimal_to_str(char *const_str, long val) {
    char *str = format_str("%ld", val);
    strcpy(const_str, str);
    free(str);
}

static void change_to_decimal(char *const_str, int base) {
    // to long
    long val = strtol(const_str, NULL, base);
    // to str
    decimal_to_str(const_str, val);
}

static void get_integer(char *const_str, int c, int base) {
    int i = 0, index;

    switch (base) {
        case 8:
            const_str[i++] = '0';
            break;
        case 16:
            const_str[i++] = '0';
            const_str[i++] = 'x';
            break;
        default:
            break;
    }

    index = char_index(HEX_DIGIT, tolower(c));
    if (index == NOT_FOUND || index >= base) {
        fatal(format_str("%s:'%c' in %s%s", UNKNOWN_CHAR, (char) c, __func__, pos_to_str(new_pos())));
    }
    const_str[i++] = (char) c;

    for (; i < MAX_TEXT - 2;) {
        c = peek();
        index = char_index(HEX_DIGIT, tolower(c));
        if (index == NOT_FOUND || index >= base) {
            break;
        }

        const_str[i++] = (char) c;
        next();
    }

    const_str[i++] = '\0';

    change_to_decimal(const_str, base);
}

static int get_float(char *const_str, int c) {
    int type = L_INTEGER;
    bool dot, exp, exp_sign;
    dot = exp = exp_sign = FALSE;
    int i = 0, index;

    const_str[i++] = (char) c;

    for (; i < MAX_TEXT - 2;) {
        c = peek();
        index = char_index(FLOAT_DIGIT, tolower(c));
        if (index == NOT_FOUND) {
            break;
        }

        if (c == '.') {
            if (dot) {
                fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
            }
            dot = TRUE;
        } else if (tolower(c) == 'e') {
            if (exp) {
                fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
            }
            exp = i;
        } else if (c == '+' || c == '-') {
            if (!exp || tolower(const_str[i - 1]) != 'e') {
                // 1.2e3+ or 1.2+, '+' is plus operator
                break;
            }
            if (exp_sign) {
                fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
            }
            exp_sign = i;
        }

        const_str[i++] = (char) c;
        next();
    }

    if (exp && (exp == i - 1 || exp_sign == i - 1)) {
        fatal(format_str("%s:'%c' in %s%s", EXPONENT_NO_DIGITS, (char) c, __func__, pos_to_str(new_pos())));
    }

    if (dot || exp) {
        type = L_FLOAT;
    }

    const_str[i++] = '\0';

    return type;
}

static void get_char(char *const_str, int c) {
    int t, i = 0;

    if (c == '\\') {
        c = next();
        switch (c) {
            case '\\':
                const_str[0] = '\\';
                break;
            case '?':
                const_str[0] = '\?';
                break;
            case '\'':
                const_str[0] = '\'';
                break;
            case '\"':
                const_str[0] = '\"';
                break;
            case 'a':
                const_str[0] = '\a';
                break;
            case 'b':
                const_str[0] = '\b';
                break;
            case 'f':
                const_str[0] = '\f';
                break;
            case 'n':
                const_str[0] = '\n';
                break;
            case 'r':
                const_str[0] = '\r';
                break;
            case 't':
                const_str[0] = '\t';
                break;
            case 'v':
                const_str[0] = '\v';
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case 'x':
                t = c;
                const_str[i++] = '0';
                if (t == 'x') {
                    const_str[i++] = 'x';
                } else {
                    const_str[i++] = (char) c;
                }
                for (; i < 4; i++) {
                    c = peek();
                    if (t != 'x' && (!isdigit(c) || c > '7')) {
                        break;
                    }
                    if (t == 'x' && !isxdigit(c)) {
                        break;
                    }
                    const_str[i] = (char) c;
                    next();
                }
                if (i < 3) { // at least 1 char
                    fatal(format_str("%s:'%c' in %s%s", UNKNOWN_CHAR, (char) c, __func__,
                                     pos_to_str(new_pos())));
                }
                change_to_decimal(const_str, t == 'x' ? 16 : 8);
                return;
            default:
                break;
        }
    } else {
        const_str[0] = (char) c;
    }

    const_str[1] = '\0';
    decimal_to_str(const_str, const_str[0]);
}

static int get_string(char **const_str, int c) {
    int i = 0;
    char *cur_char;

    // need more space to save string
    *const_str = (char *) realloc(*const_str, MAX_STR_LEN);
    if (!*const_str) {
        fatal(format_str("%s in %s", MALLOC_FAIL, __func__));
    }

    for (; i < MAX_STR_LEN - 2;) {
        if (c == '\"') {
            break;
        }
        if (c == '\\') {
            cur_char = *const_str + i;
            get_char(cur_char, c);
            (*const_str)[i++] = (char) strtol(cur_char, NULL, 10);
            c = next();
            continue;
        }
        (*const_str)[i++] = (char) c;
        c = next();
    }
    if (c != '\"') {
        fatal(format_str("%s:'%c' in %s%s", UNCLOSED_STRING, (char) c, __func__, pos_to_str(new_pos())));
    }
    (*const_str)[i++] = '\0';

    return L_STRING;
}

/*
 * get constant string
 * and return the type of constant
 * which is one of below
 *
 *** ANSI C grammar:
 * 0[xX]{H}+{IS}?           hexadecimal
 * 0{D}+{IS}?               octal
 * {D}+{IS}?                decimal
 * 0[bB]{D}+{IS}?           binary
 * '(\\.|[^\\'])+'          char
 * {D}+{E}{FS}?             decimal
 * {D}*"."{D}+({E})?{FS}?   float
 * {D}+"."{D}*({E})?{FS}?   float
 * \"(\\.|[^\\"])*\"        string
 */
static int get_literal(char **const_str, int c) {
    int type = L_NONE;
    int t = 0, pre = 0;

    switch (c) {
        case '.':
            type = L_FLOAT;
            break;
        case '\'':
            type = L_CHAR;
            c = next();
            break;
        case '"':
            type = L_STRING;
            c = next();
            break;
        case '0':
            if (tolower(t = peek()) == 'x') {
                type = L_HEXADECIMAL;
                next();
                c = next();
                break;
            } else if (tolower(t) == 'b') {
                type = L_BINARY;
                next();
                c = next();
                break;
            } else if (t == '.') {
                type = L_FLOAT;
                break;
            }
            type = L_OCTAL;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            type = L_INTEGER; // tentative
            break;
        default:
            fatal(format_str("%s:'%c' in %s%s", UNKNOWN_CHAR, (char) c, __func__, pos_to_str(new_pos())));
    }

    if (type == L_NONE) {
        fatal(format_str("something is wrong, please check code in %s", __func__));
    }

    // now, c is the first char of literal
    switch (type) {
        case L_HEXADECIMAL:
            get_integer(*const_str, c, 16);
            type = L_INTEGER;
            break;
        case L_OCTAL:
            get_integer(*const_str, c, 8);
            type = L_INTEGER;
            break;
        case L_BINARY:
            get_integer(*const_str, c, 2);
            type = L_INTEGER;
            break;
        case L_INTEGER:
        case L_FLOAT:
            type = get_float(*const_str, c);
            break;
        case L_CHAR:
            get_char(*const_str, c);
            c = next();
            if (c != '\'') {
                fatal(format_str("%s:'%c' in %s%s", UNCLOSED_CHAR, (char) c, __func__, pos_to_str(new_pos())));
            }
            type = L_INTEGER;
            break;
        case L_STRING:
            return get_string(const_str, c);
        default:
            break;
    }

    // suffix u|U|l|L|f|F
    while (TRUE) {
        c = tolower(peek());
        t = char_index(SUFFIX_DIGIT, c);
        if (t == NOT_FOUND) {
            break;
        }

        next();
        if (c == 'u') {
            if (type < L_INTEGER || type > L_INTEGER_ULL) {
                fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
            }
            switch (type) {
                case L_INTEGER:
                    type = L_INTEGER_U;
                    break;
                case L_INTEGER_L:
                    type = L_INTEGER_UL;
                    break;
                case L_INTEGER_LL:
                    type = L_INTEGER_ULL;
                    break;
                case L_INTEGER_U:
                case L_INTEGER_UL:
                case L_INTEGER_ULL:
                    fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
                    break;
                default:
                    break;
            }
        } else if (c == 'l') {
            switch (type) {
                case L_INTEGER:
                    type = L_INTEGER_L;
                    break;
                case L_INTEGER_U:
                    type = L_INTEGER_UL;
                    break;
                case L_INTEGER_L:
                case L_INTEGER_UL:
                    if (!pre || pre != 'l') {
                        fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
                    }
                    if (type == L_INTEGER_L) {
                        type = L_INTEGER_LL;
                        break;
                    }
                    type = L_INTEGER_ULL;
                    break;
                case L_INTEGER_LL:
                case L_INTEGER_ULL:
                    fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
                    break;
                case L_FLOAT:
                    type = L_FLOAT_L;
                    break;
                case L_FLOAT_F:
                case L_FLOAT_L:
                    fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
                    break;
                default:
                    break;
            }
        } else { // then 'f'
            if (type < L_FLOAT || type > L_FLOAT_L) {
                fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
            }
            switch (type) {
                case L_FLOAT:
                    type = L_FLOAT_F;
                    break;
                case L_FLOAT_F:
                case L_FLOAT_L:
                    fatal(format_str("%s:'%c' in %s%s", INVALID_SUFFIX, (char) c, __func__, pos_to_str(new_pos())));
                    break;
                default:
                    break;

            }
        }
        pre = c;
    }

    return type;
}

/*
 * parse a integer or float constant
 * when integer, octal or hexadecimal is supported
 * the value is stored in the token->v_int or token->v_float
 */
static void parse_const(Token *token, int c) {
    char *const_str = calloc(MAX_TEXT, sizeof(char));
    if (!const_str) {
        fatal(format_str("%s in %s", MALLOC_FAIL, __func__));
    }

    int type = get_literal(&const_str, c);

    token->type = type;
    switch (type) {
        case L_INTEGER:
        case L_INTEGER_U:
        case L_INTEGER_L:
        case L_INTEGER_UL:
        case L_INTEGER_LL:
        case L_INTEGER_ULL:
            token->type_name = "integer_literal";
            token->v_int = strtol(const_str, NULL, 10);
            break;
        case L_FLOAT:
        case L_FLOAT_F:
        case L_FLOAT_L:
            token->type_name = "double_literal";
            token->v_float = strtod(const_str, NULL);
            break;
        case L_STRING:
            token->type_name = "string_literal";
            token->v_string = const_str;
            break;
        default:
            break;
    }
}

static void check_keyword(char *const_str, Token *token) {
    switch (const_str[0]) {
        case 'a':
            if (!strcmp(const_str, "auto")) {
                token->type = T_AUTO;
                token->type_name = "auto";
                break;
            }
            break;
        case 'b':
            if (!strcmp(const_str, "break")) {
                token->type = T_BREAK;
                token->type_name = "break";
                break;
            }
            break;
        case 'c':
            if (!strcmp(const_str, "case")) {
                token->type = T_CASE;
                token->type_name = "case";
                break;
            }
            if (!strcmp(const_str, "char")) {
                token->type = T_CHAR;
                token->type_name = "char";
                break;
            }
            if (!strcmp(const_str, "const")) {
                token->type = T_CONST;
                token->type_name = "const";
                break;
            }
            if (!strcmp(const_str, "continue")) {
                token->type = T_CONTINUE;
                token->type_name = "continue";
                break;
            }
            break;
        case 'd':
            if (!strcmp(const_str, "default")) {
                token->type = T_DEFAULT;
                token->type_name = "default";
                break;
            }
            if (!strcmp(const_str, "do")) {
                token->type = T_DO;
                token->type_name = "do";
                break;
            }
            if (!strcmp(const_str, "double")) {
                token->type = T_DOUBLE;
                token->type_name = "double";
                break;
            }
            break;
        case 'e':
            if (!strcmp(const_str, "else")) {
                token->type = T_ELSE;
                token->type_name = "else";
                break;
            }
            if (!strcmp(const_str, "enum")) {
                token->type = T_ENUM;
                token->type_name = "enum";
                break;
            }
            if (!strcmp(const_str, "extern")) {
                token->type = T_EXTERN;
                token->type_name = "extern";
                break;
            }
            break;
        case 'f':
            if (!strcmp(const_str, "float")) {
                token->type = T_FLOAT;
                token->type_name = "float";
                break;
            }
            if (!strcmp(const_str, "for")) {
                token->type = T_FOR;
                token->type_name = "for";
                break;
            }
            break;
        case 'g':
            if (!strcmp(const_str, "goto")) {
                token->type = T_GOTO;
                token->type_name = "goto";
                break;
            }
            break;
        case 'i':
            if (!strcmp(const_str, "if")) {
                token->type = T_IF;
                token->type_name = "if";
                break;
            }
            if (!strcmp(const_str, "int")) {
                token->type = T_INT;
                token->type_name = "int";
                break;
            }
            break;
        case 'l':
            if (!strcmp(const_str, "long")) {
                token->type = T_LONG;
                token->type_name = "long";
                break;
            }
            break;
        case 'r':
            if (!strcmp(const_str, "register")) {
                token->type = T_REGISTER;
                token->type_name = "register";
                break;
            }
            if (!strcmp(const_str, "return")) {
                token->type = T_RETURN;
                token->type_name = "return";
                break;
            }
            break;
        case 's':
            if (!strcmp(const_str, "short")) {
                token->type = T_SHORT;
                token->type_name = "short";
                break;
            }
            if (!strcmp(const_str, "signed")) {
                token->type = T_SIGNED;
                token->type_name = "signed";
                break;
            }
            if (!strcmp(const_str, "sizeof")) {
                token->type = T_SIZEOF;
                token->type_name = "sizeof";
                break;
            }
            if (!strcmp(const_str, "static")) {
                token->type = T_STATIC;
                token->type_name = "static";
                break;
            }
            if (!strcmp(const_str, "struct")) {
                token->type = T_STRUCT;
                token->type_name = "struct";
                break;
            }
            if (!strcmp(const_str, "switch")) {
                token->type = T_SWITCH;
                token->type_name = "switch";
                break;
            }
            break;
        case 't':
            if (!strcmp(const_str, "typedef")) {
                token->type = T_TYPEDEF;
                token->type_name = "typedef";
                break;
            }
            break;
        case 'u':
            if (!strcmp(const_str, "union")) {
                token->type = T_UNION;
                token->type_name = "union";
                break;
            }
            if (!strcmp(const_str, "unsigned")) {
                token->type = T_UNSIGNED;
                token->type_name = "unsigned";
                break;
            }
            break;
        case 'v':
            if (!strcmp(const_str, "void")) {
                token->type = T_VOID;
                token->type_name = "void";
                break;
            }
            if (!strcmp(const_str, "volatile")) {
                token->type = T_VOLATILE;
                token->type_name = "volatile";
                break;
            }
            break;
        case 'w':
            if (!strcmp(const_str, "while")) {
                token->type = T_WHILE;
                token->type_name = "while";
                break;
            }
            break;
        default:
            break;
    }
}

static void parse_indent(Token *token, int c) {
    int i = 0;
    char *const_str = calloc(MAX_TEXT, sizeof(char));
    if (!const_str) {
        fatal(format_str("%s in %s", MALLOC_FAIL, __func__));
    }

    const_str[i++] = (char) c;
    while (TRUE) {
        c = peek();
        if (!isalpha(c) && !isdigit(c) && c != '_') {
            break;
        }
        next();
        const_str[i++] = (char) c;
    }
    const_str[i++] = '\0';
    token->type = T_IDENTIFIER;
    token->type_name = "identifier";
    token->v_string = const_str;

    check_keyword(const_str, token);
}

static int skip() {
    int c;
    while ((c = next()) == ' ' || c == '\t' || c == '\v' || c == '\n' || c == '\f');

    return c;
}

static Token *get_token() {
    Token *token = new_token();

    // scan file and gen token
    int c = skip();
    token->pos = new_pos();
    switch (c) {
        case EOF:
            token->type = T_EOF;
            token->type_name = "EOF";
            break;
        case ',':
            token->type = T_COMMA;
            token->type_name = "comma";
            break;
        case '+':
            if ((c = peek()) == '=') {
                next();
                token->type = T_ADD_ASSIGN;
                token->type_name = "add_assign";
                break;
            } else if (c == '+') {
                next();
                token->type = T_INC;
                token->type_name = "increment";
                break;
            }
            token->type = T_PLUS;
            token->type_name = "plus";
            break;
        case '-':
            if ((c = peek()) == '=') {
                next();
                token->type = T_SUB_ASSIGN;
                token->type_name = "sub_assign";
                break;
            } else if (c == '-') {
                next();
                token->type = T_DEC;
                token->type_name = "decrement";
                break;
            } else if (c == '>') {
                next();
                token->type = T_ARROW;
                token->type_name = "arrow";
                break;
            }
            token->type = T_MINUS;
            token->type_name = "minus";
            break;
        case '*':
            if (peek() == '=') {
                next();
                token->type = T_MUL_ASSIGN;
                token->type_name = "mul_assign";
                break;
            }
            token->type = T_STAR;
            token->type_name = "star";
            break;
        case '/':
            if (peek() == '=') {
                next();
                token->type = T_DIV_ASSIGN;
                token->type_name = "div_assign";
                break;
            }
            token->type = T_SLASH;
            token->type_name = "slash";
            break;
        case '%':
            if (peek() == '=') {
                next();
                token->type = T_MOD_ASSIGN;
                token->type_name = "mod_assign";
                break;
            }
            token->type = T_MOD;
            token->type_name = "modulo";
            break;
        case '<':
            if ((c = peek()) == '<') {
                next();
                if (peek() == '=') {
                    next();
                    token->type = T_LEFT_ASSIGN;
                    token->type_name = "left_shift_assign";
                    break;
                }
                token->type = T_LEFT;
                token->type_name = "left_shift";
                break;
            } else if (c == '=') {
                next();
                token->type = T_LESS_EQUAL;
                token->type_name = "less_equal";
                break;
            }
            token->type = T_LESS;
            token->type_name = "less";
            break;
        case '>':
            if ((c = peek()) == '>') {
                next();
                if (peek() == '=') {
                    next();
                    token->type = T_RIGHT_ASSIGN;
                    token->type_name = "right_shift_assign";
                    break;
                }
                token->type = T_RIGHT;
                token->type_name = "right_shift";
                break;
            } else if (c == '=') {
                next();
                token->type = T_GREATER_EQUAL;
                token->type_name = "greater_equal";
                break;
            }
            token->type = T_GREATER;
            token->type_name = "greater";
            break;
        case '&':
            if ((c = peek()) == '=') {
                next();
                token->type = T_AND_ASSIGN;
                token->type_name = "and_assign";
                break;
            } else if (c == '&') {
                next();
                token->type = T_LOG_AND;
                token->type_name = "logic_and";
                break;
            }
            token->type = T_BIT_AND;
            token->type_name = "bit_and";
            break;
        case '^':
            if (peek() == '=') {
                next();
                token->type = T_XOR_ASSIGN;
                token->type_name = "xor_assign";
                break;
            }
            token->type = T_BIT_XOR;
            token->type_name = "bit_xor";
            break;
        case '|':
            if ((c = peek()) == '=') {
                next();
                token->type = T_OR_ASSIGN;
                token->type_name = "or_assign";
                break;
            } else if (c == '|') {
                next();
                token->type = T_LOG_OR;
                token->type_name = "logic_or";
                break;
            }
            token->type = T_BIT_OR;
            token->type_name = "bit_or";
            break;
        case '?':
            token->type = T_QUESTION;
            token->type_name = "question";
            break;
        case '=':
            if (peek() == '=') {
                next();
                token->type = T_EQUAL;
                token->type_name = "equal";
                break;
            }
            token->type = T_ASSIGN;
            token->type_name = "assign";
            break;
        case '!':
            if ((peek()) == '=') {
                next();
                token->type = T_NOT_EQUAL;
                token->type_name = "not_equal";
                break;
            }
            token->type = T_LOG_NOT;
            token->type_name = "logic_not";
            break;
        case '~':
            token->type = T_BIT_NOT;
            token->type_name = "bit_not";
            break;
        case '(':
            token->type = T_LPAREN;
            token->type_name = "left_parenthesis";
            break;
        case ')':
            token->type = T_RPAREN;
            token->type_name = "right_parenthesis";
            break;
        case '[':
            token->type = T_LBRACKET;
            token->type_name = "left_bracket";
            break;
        case ']':
            token->type = T_RBRACKET;
            token->type_name = "right_bracket";
            break;
        case '.':
            if (isdigit(peek())) {
                parse_const(token, c);
                break;
            }
            token->type = T_DOT;
            token->type_name = "dot";
            break;
        case ':':
            token->type = T_COLON;
            token->type_name = "colon";
            break;
        case ';':
            token->type = T_SEMI;
            token->type_name = "semicolon";
            break;
        case '{':
            token->type = T_LBRACE;
            token->type_name = "left_brace";
            break;
        case '}':
            token->type = T_RBRACE;
            token->type_name = "right_brace";
            break;
        case '\'':
        case '\"':
            parse_const(token, c);
            break;

        default:
            if (isdigit(c)) {
                parse_const(token, c);
                break;
            }
            if (isalpha(c) || c == '_') {
                parse_indent(token, c);
                break;
            }
            fatal(format_str("%s:'%c' in %s%s", UNKNOWN_CHAR, (char) c, __func__, pos_to_str(new_pos())));
    }

    return token; // keep compiler happy
}

Token *lex(char *file) {
    Token *token_head = new_token(); // head is a empty token
    Token *token_tail = token_head;
    Token *token;

    if (!(in_file = fopen(file, "r"))) {
        fatal(format_str("Unable to open %s: %s", file, strerror(errno)));
    }
    cur_pos.file = file;
    cur_pos.line = 1;
    cur_pos.col = 0;

    while (TRUE) {
        token = get_token();
        if (token->type == T_EOF) {
            break;
        }

        // get a new token
        token_tail->next = token;
        token_tail = token;
    }

    fclose(in_file);

    return token_head;
}
