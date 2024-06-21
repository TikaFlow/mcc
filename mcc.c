//
// Created by tika on 24-6-18.
//

#include "protos.h"

/*
 * Print usage message.
 */
static void usage(char *exec) {
    printf("Usage: %s [-o outfile] file\n", exec);
}

/*
 * Parse command line options.
 * Return the input file name, if there are none, print usage and exit.
 * The output file name is stored in the param "out".
 */
static char *parse_option(int argc, char **argv, char **out) {
    char *in = NULL;

    int i = 1;

    while (i < argc) {
        if (!strcmp(argv[i], "-o")) {
            if (++i >= argc) {
                usage(argv[0]);
                fatal("Expect filename after -o");
            }
            *out = argv[i];
        } else {
            if (in) {
                warn(MULTIPLE_INPUT_FILES);
            }
            in = argv[i];
        }
        i++;
    }

    if (!in) {
        usage(argv[0]);
        fatal(NO_INPUT_FILE);
    }

    return in;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage(argv[0]);
        fatal(NO_INPUT_FILE);
    }

    char *out = NULL;
    char *in = parse_option(argc, argv, &out);
    if (!out) {
        out = "out.s";
    }

    Token *tokens = lex(in);

    // DEBUG
    Token *tk = tokens;
    while (tk) {
        switch (tk->type) {
            case T_INTEGER:
                printf("INT: %s: %ld\n", tk->type_name, tk->v_int);
                break;
            case T_INTEGER_U:
                printf("U: %s: %ld\n", tk->type_name, tk->v_int);
                break;
            case T_INTEGER_L:
                printf("L: %s: %ld\n", tk->type_name, tk->v_int);
                break;
            case T_INTEGER_UL:
                printf("UL: %s: %ld\n", tk->type_name, tk->v_int);
                break;
            case T_INTEGER_LL:
                printf("LL: %s: %ld\n", tk->type_name, tk->v_int);
                break;
            case T_INTEGER_ULL:
                printf("ULL: %s: %ld\n", tk->type_name, tk->v_int);
                break;
            case T_FLOATING:
                printf("D: %s: %lf\n", tk->type_name, tk->v_float);
                break;
            case T_FLOATING_F:
                printf("F: %s: %lf\n", tk->type_name, tk->v_float);
                break;
            case T_FLOATING_L:
                printf("LD: %s: %lf\n", tk->type_name, tk->v_float);
                break;
            case T_STRING:
            case T_IDENTIFIER:
                printf("%s: \"%s\"\n", tk->type_name, tk->v_string);
                break;
            default:
                printf("%s\n", tk->type_name);
        }
        tk = tk->next;
    }
    // DEBUG END

    // ASTNode *ast = parse(tokens);

    // ASTNode *tree = analyze(ast);

    // gen(tree);

    return 0;
}
