#include "string_builder.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

struct string_builder *sb_new(void)
{
    struct string_builder *sb = calloc(1, sizeof(struct string_builder));
    sb->str = NULL;
    sb->len = 0;
    return sb;
}

void sb_append(char c, struct string_builder *sb)
{
    sb->str = realloc(sb->str, sb->len + 1);
    if (!sb->str)
    {
        errx(1, "Failed to realloc during expansion\n c'est la D");
    }
    sb->str[sb->len++] = c;
}

char *sb_get(struct string_builder *sb)
{
    sb_append('\0', sb);
    return sb->str;
}
