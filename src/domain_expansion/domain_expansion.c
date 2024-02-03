#define _POSIX_C_SOURCE 200809L

#include "domain_expansion.h"

#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hash_map/hash_map.h"
#include "string_builder.h"

struct expand_functions
{
    char c;
    int (*expand_funs)(FILE *, struct string_builder *sb);
};

static struct hash_map *hash_map = NULL;

/**
 *  \brief  Util funcition appending to an array
 *  \fn     static char **array_append(char **array, char *str);
 *  \param  array The destination array
 *  \param  str The string to be inserted
 *  \return The new array with the value inserted
 */
static char **array_append(char **array, char *str)
{
    int len = 0;
    while (array[len] != NULL)
    {
        len++;
    }
    len++;

    char **res = realloc(array, sizeof(char *) * (len + 1));
    if (res == NULL)
    {
        return NULL;
    }

    res[len - 1] = str;
    res[len] = NULL;

    return res;
}

/**
 *  \brief  Function to expand single quotes
 *  \fn     static int expand_quote(FILE *file, struct string_builder *sb);
 *  \param  file The source to be expanded
 *  \param  sb The destination string builder
 *  \return 0 if it succeeded
 */
static int expand_quote(FILE *file, struct string_builder *sb)
{
    char c = getc(file);
    while (c != '\'')
    {
        sb_append(c, sb);
        c = getc(file);
    }

    return 0;
}

/**
 *  \brief  Function to expand backslashes
 *  \fn     static int expand_backslash(FILE *file, struct string_builder *sb)
 *  \param  file The source to be expanded
 *  \param  sb The destination string builder
 *  \return 0 if it succeeded
 */
static int expand_backslash(FILE *file, struct string_builder *sb)
{
    char c = getc(file);
    if (c != '\0')
    {
        sb_append(c, sb);
    }

    return 0;
}

static int is_name(char c)
{
    return c == '_' || isdigit(c) || isalpha(c);
}

/**
 *  \brief  Function to expand variables
 *  \fn     static char expand_var(FILE *file, struct string_builder *sb)
 *  \param  file The source to be expanded
 *  \param  sb The destination string builder
 *  \return 0 if it succeeded
 */
static char expand_var(FILE *file, struct string_builder *sb)
{
    char c = getc(file);

    struct string_builder *key_sb = sb_new();

    if (c == '{')
    {
        c = getc(file);
        while (c != '}')
        {
            sb_append(c, key_sb);
            c = getc(file);

            if (c == EOF)
            {
                char *key = sb_get(key_sb);
                free(key);
                free(key_sb);
                return -2;
            }
        }
    }
    else
    {
        if (isdigit(c))
        {
            while (isdigit(c))
            {
                sb_append(c, key_sb);
                c = getc(file);
            }
        }
        else
        {
            while (is_name(c))
            {
                sb_append(c, key_sb);
                c = getc(file);
            }
        }
    }

    char *key = sb_get(key_sb);
    char *value = hash_map_get(hash_map, key);

    if (value != NULL)
    {
        for (int i = 0; value[i] != '\0'; i++)
        {
            sb_append(value[i], sb);
        }
    }

    free(key);
    free(key_sb);

    return c;
}

static int check_d_backslash(char c)
{
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

/**
 *  \brief  Function to expand double quotes
 *  \fn     static int expand_d_quote(FILE *file, struct string_builder *sb);
 *  \param  file The source to be expanded
 *  \param  sb The destination string builder
 *  \return 0 if it succeeded
 */
static int expand_d_quote(FILE *file, struct string_builder *sb)
{
    char c = getc(file); // to skip the first "
    while (c != '"')
    {
        if (c == '\\')
        {
            char bs = c;
            c = getc(file);
            if (!check_d_backslash(c))
            {
                sb_append(bs, sb);
            }
            sb_append(c, sb);
        }
        else if (c == '$')
        {
            c = expand_var(file, sb);
            if (c == -2)
            {
                errx(1, "failed expanding variable");
            }
        }
        else
        {
            sb_append(c, sb);
        }
        c = getc(file);
    }

    return 0;
}

static struct expand_functions expand_funs[] = { { '\'', expand_quote },
                                                 { '\\', expand_backslash },
                                                 { '"', expand_d_quote } };

static FILE *to_file(char *str)
{
    FILE *stream = fmemopen(str, strlen(str), "r");
    if (!stream)
    {
        errx(1, "Failed to convert the expandable string to a FILE *");
    }
    return stream;
}

/*
static char **split_word(char *str, char **args)
{
    struct string_builder *sb = sb_new();

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (isblank(str[i]))
        {
            if (sb->len > 0)
            {
                char *tmp = sb_get(sb);

                args = array_append(args, tmp);

                free(sb);

                sb = sb_new();
            }
        }
        else
        {
            sb_append(str[i], sb);
        }
    }

    if (sb->len > 0)
    {
        char *tmp = sb_get(sb);

        args = array_append(args, tmp);
    }

    free(sb);
    free(str);

    return args;
}
*/

/**
 *  \brief  Function to expand a single string
 *  \fn     char **expand_string(char *str, char **args);
 *  \param  str The source to be expanded
 *  \param  args The destination array to be freed
 *  \return The copied destination array with the expansions
 */
char **expand_string(char *str, char **args)
{
    FILE *file = to_file(str);
    struct string_builder *sb = sb_new();

    char c = getc(file);

    int is_normal = 1;

    while (c != EOF)
    {
        is_normal = 1;
        for (size_t i = 0;
             i < sizeof(expand_funs) / sizeof(struct expand_functions); i++)
        {
            if (c == expand_funs[i].c)
            {
                if (expand_funs[i].expand_funs(file, sb))
                {
                    char *word = sb_get(sb);
                    free(word);
                    free(sb);
                    free(args);
                    return NULL;
                }
                is_normal = 0;
            }
            else if (c == '$')
            {
                c = expand_var(file, sb);
                if (c == -2)
                {
                    char *word = sb_get(sb);
                    free(word);
                    free(sb);
                    free(args);
                    return NULL;
                }
                is_normal = 0;
            }
        }

        if (is_normal)
        {
            sb_append(c, sb);
        }

        c = getc(file);
    }

    char *word = sb_get(sb);

    free(sb);

    return array_append(args, word);
}

/**
 *  \brief  Function to expand an array
 *  \fn     char **expand(char **args, struct hash_map *hm);
 *  \param  args The source array to be expanded
 *  \param  hm The hash_map where variables can be found
 *  \return The copied destination array with the expansions not freed
 */
char **expand(char **args, struct hash_map *hm)
{
    hash_map = hm;

    char **res = calloc(1, sizeof(char *));
    if (res == NULL)
    {
        return args;
    }

    for (int i = 0; args[i] != NULL; i++)
    {
        res = expand_string(args[i], res);
    }

    return res;
}
