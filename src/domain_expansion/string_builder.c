#include "string_builder.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

struct string_builder *new_sb()
{
    struct string_builder *sb = calloc(1, sizeof(struct string_builder));
    sb->str = NULL;
    sb->len = 0;
    return sb;
}

void append_char_sb(char c, struct string_builder *sb)
{
    sb->str = realloc(sb->str, sb->len + 1);
    if (!sb->str)
    {
        errx(1, "Failed to realloc during expansion\n c'est la D");
    }
    sb->str[sb->len++] = c;
}

void free_sb(struct string_builder *sb)
{
    free(sb->str);
    free(sb);
}

char *get_sb_string(struct string_builder *sb)
{
    append_char_sb('\0', sb);
    return sb->str;
}

char *get_sb_string_free(char **s, struct string_builder *sb)
{
    *s = realloc(*s, sb->len + 1);
    if (!s)
    {
        errx(1, "Failed to realloc during expansion");
    }
    char *str = get_sb_string(sb);
    strcpy(*s, str);
    free_sb(sb);
    return str;
}
