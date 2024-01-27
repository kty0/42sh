#ifndef AST_H
#define AST_H

#include "../lexer/token.h"

enum ast_type
{
    AST_COMMAND,
    AST_LIST,
    AST_IF,
    AST_NOT,
    AST_PIPE,
    AST_WHILE,
    AST_UNTIL,
    AST_OPERATOR,
};

enum ope_type
{
    AND,
    OR
};

/* definition of the ast nodes */

struct ast_cmd
{
    enum exp_type **exps; // NULL terminated exp_type array
    char **args; // NULL terminated arguments array
};

struct ast_if
{
    struct ast *condition; // the condition
    struct ast *then_body; // the body of the then clause
    struct ast *else_body; // the body of the else, may be NULL
};

struct ast_list
{
    struct ast **children;
};

struct ast_not
{
    struct ast *child;
};

struct ast_pipe
{
    struct ast *command;
    struct ast *child;
};

struct ast_while
{
    struct ast *condition;
    struct ast *body;
};

struct ast_until
{
    struct ast *condition;
    struct ast *body;
};

struct ast_ope
{
    enum ope_type type;
    struct ast *left;
    struct ast *right;
};

struct ast_redir
{
    enum redir_type type;
    int fd;
    char *file;
    struct ast *left;
    struct ast *right;
};

struct ast_word
{
    enum exp_type exp;
    char *arg;
};

/* a few very nice base structs */

union ast_union
{
    struct ast_cmd ast_cmd;
    struct ast_list ast_list;
    struct ast_if ast_if;
    struct ast_not ast_not;
    struct ast_pipe ast_pipe;
    struct ast_while ast_while;
    struct ast_until ast_until;
    struct ast_ope ast_ope;
};

struct ast
{
    enum ast_type type;
    union ast_union data;
};

/* our pretty functions */

int ast_cmd_push(struct ast *ast, char *arg, enum exp_type exp);

int ast_list_push(struct ast *ast, struct ast *child);

void ast_print(struct ast *ast);

void ast_free(struct ast *ast);

#endif /* !AST_H */
