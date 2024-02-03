#ifndef DOMAIN_EXPANSION_H
#define DOMAIN_EXPANSION_H

#include <err.h>

#include "../ast/ast.h"
#include "../hash_map/hash_map.h"

char **expand(char **args, struct hash_map *hm);

char **expand_string(char *str, char **args);

#endif /* DOMAIN_EXPANSION_H */
