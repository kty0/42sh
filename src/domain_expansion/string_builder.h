#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

struct string_builder
{
    char *str;
    int len;
};

struct string_builder *new_sb();
void append_char_sb(char c, struct string_builder *sb);
void free_sb(struct string_builder *sb);
char *get_sb_string(struct string_builder *sb);
char *get_sb_string_free(char **s, struct string_builder *sb);

#endif // !STRING_BUILDER_H
