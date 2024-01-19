#ifndef AST_H
#define AST_H

enum ast_type
{
    AST_COMMAND,
    AST_LIST,
    AST_IF,
    AST_NOT,
    AST_PIPE,
};

/* definition of the ast nodes */

struct ast_cmd
{
    char **args; // NULL terminated arguments list
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

/* a few very nice base structs */

union ast_union
{
    struct ast_cmd ast_cmd;
    struct ast_list ast_list;
    struct ast_if ast_if;
    struct ast_not ast_not;
};

struct ast
{
    enum ast_type type;
    union ast_union data;
};

/* our pretty functions */

int ast_cmd_push(struct ast *ast, char *arg);

int ast_list_push(struct ast *ast, struct ast *child);

void ast_print(struct ast *ast);

void ast_free(struct ast *ast);

#endif /* !AST_H */
