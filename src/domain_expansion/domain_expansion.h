#ifndef DOMAIN_EXPANSION_H
#define DOMAIN_EXPANSION_H

#include <err.h>

#include "../ast/ast.h"

void expand(struct ast *ast);
void expand_command(struct ast *ast);

#endif /* DOMAIN_EXPANSION_H */
