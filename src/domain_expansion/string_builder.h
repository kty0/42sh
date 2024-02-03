#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

struct string_builder
{
    char *str;
    int len;
};

struct string_builder *sb_new(void);

void sb_append(char c, struct string_builder *sb);

char *sb_get(struct string_builder *sb);

#endif // !STRING_BUILDER_H
