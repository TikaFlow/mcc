//
// Created by tika on 24-6-18.
//

#include "protos.h"

int main(int argc, char **argv) {
    Token *tokens = lex(argv[1]);

    // DEBUG
    Token *tk = tokens;
    while (tk) {
        printf("%s\n", tk->type_name);
        tk = tk->next;
    }
    // DEBUG END

    // ASTNode *ast = parse(tokens);

    // code_gen(ast);

    return 0;
}
