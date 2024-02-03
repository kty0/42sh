#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stddef.h>

struct pair_list
{
    char *key;
    char *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    size_t size;
};

size_t hash(char *str);

struct hash_map *hash_map_init(size_t size);

int hash_map_insert(struct hash_map *hash_map, char *key, char *value);

int hash_map_insert_op(struct hash_map *hash_map, char *key, char *value);

void hash_map_free(struct hash_map *hash_map);

char *hash_map_get(struct hash_map *hash_map, char *key);

#endif /* ! HASH_MAP_H */
