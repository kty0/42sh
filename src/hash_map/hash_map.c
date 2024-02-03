#include "hash_map.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *hash_map = malloc(sizeof(struct hash_map));
    if (hash_map == NULL)
    {
        return NULL;
    }

    hash_map->size = size;

    hash_map->data = calloc(size, sizeof(struct pair_list));
    if (hash_map->data == NULL)
    {
        free(hash_map);
        return NULL;
    }

    return hash_map;
}

static char *copy(char *str)
{
    char *new = calloc(strlen(str) + 1, sizeof(char));

    strcpy(new, str);

    return new;
}

int hash_map_insert(struct hash_map *hash_map, char *key, char *value,
                    int *updated)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return 0;
    }

    key = copy(key);
    value = copy(value);

    int hash_value = hash(key) % hash_map->size;
    struct pair_list *tmp = hash_map->data[hash_value];

    while (tmp != NULL)
    {
        if (strcmp(tmp->key, key) == 0)
        {
            free(tmp->value);
            free(key);
            tmp->value = value;
            *updated = 1;

            return 1;
        }

        tmp = tmp->next;
    }

    struct pair_list *new = malloc(sizeof(struct pair_list));
    if (new == NULL)
    {
        return 0;
    }

    new->next = hash_map->data[hash_value];
    hash_map->data[hash_value] = new;
    new->key = key;
    new->value = value;

    *updated = 0;

    return 1;
}

static void hash_map_free_rec(struct pair_list *data)
{
    if (data == NULL)
    {
        return;
    }

    hash_map_free_rec(data->next);
    free(data->value);
    free(data->key);
    free(data);
}

void hash_map_free(struct hash_map *hash_map)
{
    if (hash_map == NULL)
    {
        return;
    }

    for (size_t i = 0; i < hash_map->size; i++)
    {
        hash_map_free_rec(hash_map->data[i]);
    }

    free(hash_map->data);
    free(hash_map);
}

void hash_map_dump(struct hash_map *hash_map)
{
    for (size_t i = 0; i < hash_map->size; i++)
    {
        struct pair_list *tmp = hash_map->data[i];

        while (tmp != NULL)
        {
            printf("%s: %s", tmp->key, tmp->value);
            tmp = tmp->next;

            if (tmp == NULL)
            {
                printf("\n");
            }
            else
            {
                printf(", ");
            }
        }
    }
}

char *hash_map_get(struct hash_map *hash_map, char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return NULL;
    }

    size_t i = hash(key) % hash_map->size;
    struct pair_list *cur = hash_map->data[i];

    while (cur != NULL)
    {
        if (strcmp(cur->key, key) == 0)
        {
            return cur->value;
        }

        cur = cur->next;
    }

    return NULL;
}

int hash_map_remove(struct hash_map *hash_map, char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return 0;
    }

    size_t i = hash(key) % hash_map->size;

    struct pair_list *prev = NULL;
    struct pair_list *cur = hash_map->data[i];

    while (cur != NULL)
    {
        if (strcmp(cur->key, key) == 0)
        {
            if (prev == NULL)
            {
                hash_map->data[i] = cur->next;
            }
            else
            {
                prev->next = cur->next;
            }

            free(cur);

            return 1;
        }

        prev = cur;
        cur = cur->next;
    }

    return 0;
}
