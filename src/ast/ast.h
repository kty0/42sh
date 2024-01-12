#ifndef AST_H
#define AST_H

enum ast_type
{
    AST_COMMAND,
    AST_WORD,
    AST_LIST,
    AST_IF,
    AST_SHELL
};

struct ast
{
    enum ast_type type;
    char **args;
    struct ast **children;
};

struct ast *ast_new(enum ast_type type);

void ast_free(struct ast *ast);

int ast_push_arg(struct ast *ast, char *arg);

int ast_push_child(struct ast *ast, struct ast *child);

void print_if(struct ast *ast);
void print_command(struct ast *ast);
void print_list(struct ast *ast);
void print(struct ast *ast);

#endif /* !AST_H */
