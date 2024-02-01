#define _POSIX_C_SOURCE 200809L

#include "domain_expansion.h"

#include <err.h>
#include <stdio.h>
#include <string.h>

#include "string_builder.h"

struct expand_functions
{
    char c;
    void (*expand_funs)(FILE *, struct string_builder *sb);
};

static void expand_quote(FILE *file, struct string_builder *sb)
{
    char c = getc(file);
    while (c != '\'')
    {
        append_char_sb(c, sb);
        c = getc(file);
    }
}

static void expand_backslash(FILE *file, struct string_builder *sb)
{
    char c = getc(file);
    if (c != '\0')
    {
        append_char_sb(c, sb);
    }
}

static int check_d_backslash(char c)
{
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

static void expand_d_quote(FILE *file, struct string_builder *sb)
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
                append_char_sb(bs, sb);
            }
            append_char_sb(c, sb);
        } // add '$'
        else
        {
            append_char_sb(c, sb);
        }
        c = getc(file);
    }
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

void expand(char **str)
{
    FILE *file = to_file(*str);
    struct string_builder *sb = new_sb();
    char c = getc(file);
    int is_simple = 1;
    while (c != EOF)
    {
        is_simple = 1;
        for (size_t i = 0;
             i < sizeof(expand_funs) / sizeof(struct expand_functions); i++)
        {
            if (c == expand_funs[i].c)
            {
                expand_funs[i].expand_funs(file, sb);
                is_simple = 0;
            }
        }
        if (is_simple)
        {
            append_char_sb(c, sb);
        }
        c = getc(file);
    }
    get_sb_string_free(str, sb);
}
