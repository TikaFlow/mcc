//
// Created by tika on 24-6-22.
//

#include "protos.h"

Token *cur_token = NULL;

/*
 * prototypes:
 *
 * declarator();
 * statement();
 * initializer();
 * assignment_expression();
 * declaration_specifiers();
 */

static bool has_next() {
    return cur_token && cur_token->next;
}

static Token *next() {
    if (cur_token) {
        cur_token = cur_token->next;
    }
    return cur_token;
}

static Token *peek() {
    if (cur_token) {
        return cur_token->next;
    }
    return cur_token;
}

static ASTNode *new_node() {
    ASTNode *node = calloc(1, sizeof(Token));
    if (!node) {
        fatal(format_str("%s in %s", MALLOC_FAIL, __func__));
    }

    return node;
}

static bool is_unary_operator() {
    if (!has_next()) {
        return FALSE;
    }

    int type = peek()->type;
    if (type == T_BIT_AND
        || type == T_STAR
        || type == T_PLUS
        || type == T_MINUS
        || type == T_BIT_NOT
        || type == T_LOG_NOT) {
        return TRUE;
    }

    return FALSE;
}

static bool is_assignment_operator() {
    if (!has_next()) {
        return FALSE;
    }

    int type = peek()->type;
    if (type == T_ASSIGN
        || type == T_MUL_ASSIGN
        || type == T_DIV_ASSIGN
        || type == T_MOD_ASSIGN
        || type == T_ADD_ASSIGN
        || type == T_SUB_ASSIGN
        || type == T_LEFT_ASSIGN
        || type == T_RIGHT_ASSIGN
        || type == T_AND_ASSIGN
        || type == T_XOR_ASSIGN
        || type == T_OR_ASSIGN) {
        return TRUE;
    }

    return FALSE;
}

static bool is_jump_keyword() {
    if (!has_next()) {
        return FALSE;
    }

    int type = peek()->type;
    if (type == T_GOTO
        || type == T_CONTINUE
        || type == T_BREAK
        || type == T_RETURN) {
        return TRUE;
    }

    return FALSE;
}

static bool is_struct_or_union() {
    if (!has_next()) {
        return FALSE;
    }

    int type = peek()->type;
    return type == T_STRUCT || type == T_UNION;
}

static bool is_type_qualifier() {
    if (!has_next()) {
        return FALSE;
    }

    int type = peek()->type;
    return type == T_CONST || type == T_VOLATILE;
}

static bool is_type_specifier() {
    if (!has_next()) {
        return FALSE;
    }

    int type = peek()->type;
    if (type == T_VOID
        || type == T_CHAR
        || type == T_SHORT
        || type == T_INT
        || type == T_LONG
        || type == T_FLOAT
        || type == T_DOUBLE
        || type == T_SIGNED
        // union/struct/enum
        || type == T_UNSIGNED) {
        return TRUE;
    }

    return FALSE;
}

static bool is_storage_class_specifier() {
    if (!has_next()) {
        return FALSE;
    }

    int type = peek()->type;
    if (type == T_TYPEDEF
        || type == T_EXTERN
        || type == T_STATIC
        || type == T_AUTO
        || type == T_REGISTER) {
        return TRUE;
    }

    return FALSE;
}

static ASTNode *multiplicative_expression() {
    /*
     * cast_expression();
     * ['*' multiplicative_expression();] // recursive call
     * or ['/' multiplicative_expression();]
     * or ['%' multiplicative_expression();]
     */
}

static ASTNode *additive_expression() {
    /*
     * multiplicative_expression();
     * ['+' additive_expression();] // recursive call
     * or ['-' additive_expression();]
     */
}

static ASTNode *shift_expression() {
    /*
     * additive_expression();
     * ['<<' shift_expression();] // recursive call
     * or ['>>' shift_expression();]
     */
}

static ASTNode *relation_expression() {
    /*
     * shift_expression();
     * ['<' relation_expression();] // recursive call
     * or ['<=' relation_expression();]
     * or ['>' relation_expression();]
     * or ['>=' relation_expression();]
     */
}

static ASTNode *equality_expression() {
    /*
     * relational_expression();
     * ['==' equality_expression();] // recursive call
     * or ['!=' equality_expression();]
     */
}

static ASTNode *and_expression() {
    /*
     * equality_expression();
     * ['&' and_expression();] // recursive call
     */
}

static ASTNode *exclusive_or_expression() {
    /*
     * and_expression();
     * ['^' exclusive_or_expression();] // recursive call
     */
}

static ASTNode *argument_expression_list() {
    /*
     * assignment_expression();
     * [',' argument_expression_list();] // recursive call
     */
}

static ASTNode *primary_expression() {
    /*
     * '(' expression(); ')'
     * or IDENTIFIER
     * or CONSTANT
     * or STRING_LITERAL
     */
}

static ASTNode *direct_abstract_declarator_aux() {
    /*
     * (
     * '(' [parameter_type_list();] ')'
     * or '[' [constant_expression();] ']'
     * )
     * [direct_abstract_declarator_aux();] // recursive call
     */
}

static ASTNode *direct_abstract_declarator() {
    /*
     * ['(' abstract_declarator(); ')']
     * direct_abstract_declarator_aux();
     */
}

static ASTNode *inclusive_or_expression() {
    /*
     * exclusive_or_expression();
     * ['|' inclusive_or_expression();] // recursive call
     */
}

static ASTNode *struct_declarator() {
    /*
     * ':' constant_expression();
     * or declarator(); [':' constant_expression();]
     */
}

static ASTNode *type_name() {
    /*
     * specifier_qualifier_list();
     * [abstract_declarator();]
     */
}

static ASTNode *cast_expression() {
    /*
     * unary_expression();
     * or '(' type_name(); ')' cast_expression();
     */
}

static ASTNode *postfix_expression_aux() {
    /*
     * if(<empty>) return;
     * ['[' expression(); ']' postfix_expression_aux(); // recursive call
     * or '(' [argument_expression();] ')' postfix_expression_aux();
     * or '.' IDENTIFIER postfix_expression_aux();
     * or '->' IDENTIFIER postfix_expression_aux();
     * or '++' postfix_expression_aux();
     * or '--' postfix_expression_aux();
     */
}

static ASTNode *postfix_expression() {
    /*
     * primary_expression();
     * postfix_expression_aux();
     */
}

static ASTNode *abstract_declarator() {
    /*
     * direct_abstract_declarator();
     * or pointer(); [direct_abstract_declarator();]
     */
}

static ASTNode *logical_and_expression() {
    /*
     * logical_or_expression();
     * ['&&' logical_and_expression();] // recursive call
     */
}

static ASTNode *struct_declarator_list() {
    /*
     * struct_declarator();
     * [',' struct_declarator_list();] // recursive call
     */
}

static ASTNode *specifier_qualifier_list() {
    /*
     * (type_specifier() or type_qualifier())
     * [specifier_qualifier_list();]
     */
}

static ASTNode *unary_expression() {
    /*
     * postfix_expression();
     * or unary_operator(); cast_expression();
     * or ('sizeof' or '++' or '--') unary_expression();
     * or 'sizeof' '(' type_name(); ')'
     */
}

static ASTNode *parameter_declaration() {
    /*
     * declaration_specifiers();
     * [declarator(); or abstract_declarator();]
     */
}

static ASTNode *expression() {
    /*
     * assignment_expression();
     * [',' expression();] // recursive call
     */
}

static ASTNode *logical_or_expression() {
    /*
     * logical_or_expression();
     * ['||' logical_and_expression();] // recursive call
     */
}

static ASTNode *enumerator() {
    /*
     * IDENTIFIER
     * ['=' constant_expression();]
     */
}

static ASTNode *struct_declaration() {
    /*
     * specifier_qualifier_list();
     * struct_declarator_list();
     * ';'
     */
}

static ASTNode *initializer_list() {
    /*
     * initializer();
     * [',' initializer_list();] // recursive call
     */
}

static ASTNode *assignment_expression() {
    /*
     * conditional_expression();
     * or unary_expression(); assignment_operator(); assignment_expression(); // recursive call
     */
}

static ASTNode *labeled_statement() {
    /*
     * IDENTIFIER ':' statement();
     * or case constant_expression ':' statement();
     * or default ':' statement();
     */
}

static ASTNode *expression_statement() {
    /*
     * expression();
     * or ';'
     */
}

static ASTNode *selection_statement() {
    /*
     * or 'if' '(' expression(); ')' statement(); ['else' statement();]
     * or 'switch' '(' expression(); ')' statement();
     */
}

static ASTNode *iteration_statement() {
    /*
     * 'while' '(' expression(); ')' statement();
     * or 'do' statement(); 'while' '(' expression(); ')' ';'
     * or 'for' '(' expression_statement(); expression_statement(); [expression();] ')' statement();
     */
}

static ASTNode *jump_statement() {
    /*
     * 'goto' IDENTIFIER ';'
     * or 'continue' ';'
     * or 'break' ';'
     * or 'return' [expression();] ';'
     */
}

static ASTNode *statement() {
    /*
     * labeled_statement();
     * or compound_statement();
     * or expression_statement();
     * or selection_statement();
     * or iteration_statement();
     * or jump_statement();
     */
}

static ASTNode *parameter_list() {
    /*
     * parameter_declaration();
     * [',' parameter_list();] // recursive call
     */
}

static ASTNode *conditional_expression() {
    /*
     * logical_or_expression();
     * [('?' expression(); ':' conditional_expression();)] // recursive call
     */
}

static ASTNode *enumerator_list() {
    /*
     * enumerator();
     * [',' enumerator_list();] // recursive call
     */
}

static ASTNode *struct_declaration_list() {
    /*
     * struct_declaration();
     * [struct_declaration_list();] // recursive call
     */
}

static ASTNode *initializer() {
    /*
     * assignment_expression();
     * or '{' initializer_list(); [','] '}'
     */
}

static ASTNode *identifier_list() {
    /*
     * IDENTIFIER
     * [',' identifier_list();] // recursive call
     */
}

static ASTNode *parameter_type_list() {
    /*
     * parameter_list();
     * [',' '...']
     */
}

static ASTNode *constant_expression() {
    return conditional_expression();
}

static ASTNode *type_qualifier_list() {
    /*
     * type_qualifier();
     * [type_qualifier_list();] // recursive call
     */
}

static ASTNode *enum_specifier() {
    /*
     * ENUM
     * IDENTIFIER ['{' struct_declaration_list(); '}']
     * or '{' struct_declaration_list(); '}'
     */
}

static ASTNode *struct_or_union_specifier() {
    /*
     * if(!is_struct_or_union()) fatal;
     * struct_or_union();
     * IDENTIFIER ['{' struct_declaration_list(); '}']
     * or '{' struct_declaration_list(); '}'
     */
}

static ASTNode *init_declarator() {
    /*
     * declarator();
     * ['=' initializer();]
     */
}

static ASTNode *statement_list() {
    /*
     * statement();
     * [statement_list();] // recursive call
     */
}

static ASTNode *direct_declarator_aux() {
    /*
     * if(<empty>) return;
     * '[' [constant_expression();] ']'
     * or '(' [parameter_type_list(); or identifier_list();] ')'
     * [direct_declarator_aux();] // recursive call
     */
}

static ASTNode *direct_declarator() {
    /*
     * IDENTIFIER or '(' declarator ')'
     * [direct_declarator_aux();]
     */
}

static ASTNode *pointer() {
    /*
     * '*'
     * [type_qualifier_list();]
     * [pointer();] // recursive call
     */
}

static ASTNode *init_declarator_list() {
    /*
     * init_declarator();
     * [',' init_declarator_list();] // recursive call
     */
}

static ASTNode *compound_statement() {
    /*
     * '{'
     * statement_list();
     * or declaration_list(); [statement_list();]
     * '}'
     */
}

static ASTNode *declaration_list() {
    /*
     * declaration();
     * [declaration_list();] // recursive call
     */
}

static ASTNode *declarator() {
    /*
     * [pointer]
     * direct_declarator();
     */
}

static ASTNode *declaration_specifiers() {
    /*
     * storage_class_specifier(); or type_specifier(); or type_qualifier();
     * [declaration_specifiers();] // recursive call
     */
}

static ASTNode *function_definition() {
    /*
     * {spe} // return type is necessary, like C99
     * declarator();
     * [declaration_list();]
     * compound_statement();
     */
}

static ASTNode *declaration() {
    /*
     * {spe}
     * if(';') return;
     * init_declarator_list();
     * ';'
     */
}

static ASTNode *external_declaration() {
    /*
     * declaration_specifiers()
     * if (';' or init_declarator_list) {
     *     declaration(spe);
     * } else {
     *     func_def(spe);
     * }
     */
}

static ASTNode *translation_unit() {
    /*
     * external_declaration();
     * [translation_unit();] // recursive call
     */
}

ASTNode *parse(Token *tokens) {
    if (!tokens) {
        return NULL;
    }
    cur_token = tokens;

    if (has_next()) {
        return translation_unit();
    }

    return NULL;
}
