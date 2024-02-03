#include "hash_map.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* The insert function but with no guards */
int hash_map_insert_op(struct hash_map *hash_map, char *key, char *value)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return 0;
    }

    int hash_value = hash(key) % hash_map->size;
    struct pair_list *tmp = hash_map->data[hash_value];

    while (tmp != NULL)
    {
        if (strcmp(tmp->key, key) == 0)
        {
            free(tmp->value);
            free(key);
            tmp->value = value;

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

    return 1;
}

static void set_default(struct hash_map *hash_map)
{
    /* Sets up the UID variable */
    int uid = getuid();
    char *value = calloc(8, sizeof(char));
    sprintf(value, "%d", uid);

    char *k = "UID";
    char *key = calloc(4, sizeof(char));
    strcpy(key, k);

    hash_map_insert_op(hash_map, key, value);

    /* Sets up the $ variable */
    int pid = getpid();
    value = calloc(8, sizeof(char));
    sprintf(value, "%d", pid);

    key = calloc(2, sizeof(char));
    key[0] = '$';

    hash_map_insert_op(hash_map, key, value);

    /* Sets up the $ variable */
    value = calloc(2, sizeof(char));
    value[0] = '0';

    key = calloc(2, sizeof(char));
    key[0] = '?';

    hash_map_insert_op(hash_map, key, value);

    /* Sets up the IFS variable */
    char *v = " \t\n\0";
    value = calloc(5, sizeof(char));
    strcpy(value, v);

    k = "IFS";
    key = calloc(4, sizeof(char));
    strcpy(key, k);

    hash_map_insert_op(hash_map, key, value);
}

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

    set_default(hash_map);

    return hash_map;
}

static char *copy(char *str)
{
    char *new = calloc(strlen(str) + 1, sizeof(char));

    strcpy(new, str);

    return new;
}

static int is_reserved(char *key)
{
    if (!strcmp(key, "UID"))
    {
        errx(1, "cannot set this variable");
    }

    return !strcmp(key, "#");
}

int hash_map_insert(struct hash_map *hash_map, char *key, char *value)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return 0;
    }

    if (is_reserved(key))
    {
        return 1;
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

char *hash_map_get(struct hash_map *hash_map, char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return NULL;
    }

    if (!strcmp(key, "RANDOM"))
    {
        srand(time(NULL));
        int n = rand() % 32768;

        char *value = calloc(8, sizeof(char));
        sprintf(value, "%d", n);

        hash_map_insert(hash_map, key, value);

        free(value);
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
