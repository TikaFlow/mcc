# for more detail, see https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

translation_unit
	: external_declaration
	| external_declaration translation_unit
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier
	| type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers
	;

declarator
	: pointer direct_declarator
	| direct_declarator
	;

declaration_list
	: declaration
	| declaration declaration_list
	;

compound_statement
	: '{' '}'
	| '{' statement_list '}'
	| '{' declaration_list '}'
	| '{' declaration_list statement_list '}'
	;

init_declarator_list
	: init_declarator
	| init_declarator ',' init_declarator_list
	;

storage_class_specifier
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID
	| CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
	| struct_or_union_specifier
	| enum_specifier
	| TYPE_NAME
	;

type_qualifier
	: CONST
	| VOLATILE
	;

pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;

direct_declarator
	: IDENTIFIER direct_declarator_aux
	| '(' declarator ')' direct_declarator_aux
	;
direct_declarator_aux
	: <empty>
	| '[' ']' direct_declarator_aux
	| '[' constant_expression ']' direct_declarator_aux
	| '(' ')' direct_declarator_aux
	| '(' identifier_list ')' direct_declarator_aux
	| '(' parameter_type_list ')' direct_declarator_aux
	;

statement_list
	: statement
	| statement statement_list
	;

init_declarator
	: declarator
	| declarator '=' initializer
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier type_qualifier_list
	;

constant_expression
	: conditional_expression
	;

parameter_type_list
	: parameter_list
	| parameter_list ',' ELLIPSIS
	;

identifier_list
	: IDENTIFIER
	| IDENTIFIER ',' identifier_list
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration struct_declaration_list
	;

enumerator_list
	: enumerator
	| enumerator ',' enumerator_list
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

parameter_list
	: parameter_declaration
	| parameter_declaration ',' parameter_list
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

initializer_list
	: initializer
	| initializer ',' initializer_list
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_and_expression OR_OP logical_or_expression
	;

expression
	: assignment_expression
	| assignment_expression ',' expression
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator ',' struct_declarator_list
	;

logical_and_expression
	: inclusive_or_expression
	| inclusive_or_expression AND_OP logical_and_expression
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

postfix_expression
	: primary_expression postfix_expression_aux
	;
postfix_expression_aux
	:  <empty>
	|  '(' ')' postfix_expression_aux
	|  '(' argument_expression_list ')' postfix_expression_aux
	| '[' expression ']' postfix_expression_aux
	|  '.' IDENTIFIER postfix_expression_aux
	|  PTR_OP IDENTIFIER postfix_expression_aux
	|  INC_OP postfix_expression_aux
	|  DEC_OP postfix_expression_aux
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

struct_declarator
	: declarator
	| declarator ':' constant_expression
	| ':' constant_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| exclusive_or_expression '|' inclusive_or_expression
	;

direct_abstract_declarator
	: direct_abstract_declarator_aux
	| '(' abstract_declarator ')' direct_abstract_declarator_aux
	;
direct_abstract_declarator_aux
	: '(' ')'
	| '(' parameter_type_list ')'
	| '[' ']'
	| '[' constant_expression ']'
	| '(' ')' direct_abstract_declarator_aux
	| '(' parameter_type_list ')' direct_abstract_declarator_aux
	| '[' ']' direct_abstract_declarator_aux
	| '[' constant_expression ']' direct_abstract_declarator_aux
	;

primary_expression
	: IDENTIFIER
	| CONSTANT
	| STRING_LITERAL
	| '(' expression ')'
	;

argument_expression_list
	: assignment_expression
	| assignment_expression ',' argument_expression_list
	;

exclusive_or_expression
	: and_expression
	| and_expression '^' exclusive_or_expression
	;

and_expression
	: equality_expression
	| equality_expression '&' and_expression
	;

equality_expression
	: relational_expression
	| relational_expression EQ_OP equality_expression
	| relational_expression NE_OP equality_expression
	;

relational_expression
	: shift_expression
	| shift_expression '<' relational_expression
	| shift_expression '>' relational_expression
	| shift_expression LE_OP relational_expression
	| shift_expression GE_OP relational_expression
	;

shift_expression
	: additive_expression
	| additive_expression LEFT_OP shift_expression
	| additive_expression RIGHT_OP shift_expression
	;

additive_expression
	: multiplicative_expression
	| multiplicative_expression '+' additive_expression
	| multiplicative_expression '-' additive_expression
	;

multiplicative_expression
	: cast_expression
	| cast_expression '*' multiplicative_expression
	| cast_expression '/' multiplicative_expression
	| cast_expression '%' multiplicative_expression
	;
