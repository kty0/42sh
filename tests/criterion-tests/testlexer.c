#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../src/lexer/lexer.h"
#include "../src/lexer/token.h"

Test(lexer_new, null, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("");
    cr_assert_not_null(l, "lexer shouldn't be null");
    lexer_pop(l);
    cr_assert_eq(l->current_tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(l->current_tok.value, "str should be null");
    free_token(l->current_tok);
    lexer_free(l);
}

Test(lexer_new, if, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("if");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_IF, "incorrect type");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, then, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("then");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_THEN, "incorrect type");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, elif, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("elif");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_ELIF, "incorrect type");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, else, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("else");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_ELSE, "incorrect type");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, fi, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("fi");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_FI, "incorrect type");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, semicolon, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new(";");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_SEMICOLON, "incorrect type");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, word, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("hello");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_WORD, "incorrect type");
    cr_assert_str_eq(tok.value, "hello", "string is not equal to hello");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, singlequote_simple, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("'echo'");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    printf("%s /n", tok.value);
    cr_assert_eq(tok.type, TOKEN_WORD, "incorrect type");
    cr_assert_str_eq(tok.value, "echo", "string is not equal to echo");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, singlequote_repetition, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("'e''c''h''o'");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_WORD, "incorrect type");
    cr_assert_str_eq(tok.value, "echo", "string is not equal to echo");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, whitespace, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("ec ho");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_WORD, "incorrect type");
    cr_assert_str_eq(tok.value, "ec", "string is not equal to echo");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_WORD, "incorrect type");
    cr_assert_str_eq(tok.value, "ho", "string is not equal to hello");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, tricky, .init = cr_redirect_stdout)
{
    struct lexer *l =
        lexer_new("if then else fi; word1 '\"'quoted'\"' ; word2");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tokens[] = {
        { TOKEN_IF, NORMAL, NULL },           { TOKEN_THEN, NORMAL, NULL },
        { TOKEN_ELSE, NORMAL, NULL },         { TOKEN_FI, NORMAL, NULL },
        { TOKEN_SEMICOLON, NORMAL, NULL },    { TOKEN_WORD, NORMAL, "word1" },
        { TOKEN_WORD, NORMAL, "\"quoted\"" }, { TOKEN_SEMICOLON, NORMAL, NULL },
        { TOKEN_WORD, NORMAL, "word2" },      { TOKEN_EOF, NORMAL, NULL },
    };

    for (long unsigned int i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++)
    {
        struct token tok = lexer_pop(l);
        cr_assert_eq(tok.type, tokens[i].type, "Incorrect token at position %d",
                     i);
        if (tokens[i].value != NULL)
        {
            cr_assert_str_eq(tok.value, tokens[i].value,
                             "Incorrect content in the string at pos %d", i);
        }
        free_token(tok);
    }
    lexer_free(l);
}

Test(lexer_new, trickier, .init = cr_redirect_stdout)
{
    struct lexer *l =
        lexer_new("if t'h'en e'l''s''e' fi; word1 '\"quoted\"'; word2");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tokens[] = {
        { TOKEN_IF, NORMAL, NULL },           { TOKEN_THEN, NORMAL, NULL },
        { TOKEN_ELSE, NORMAL, NULL },         { TOKEN_FI, NORMAL, NULL },
        { TOKEN_SEMICOLON, NORMAL, NULL },    { TOKEN_WORD, NORMAL, "word1" },
        { TOKEN_WORD, NORMAL, "\"quoted\"" }, { TOKEN_SEMICOLON, NORMAL, NULL },
        { TOKEN_WORD, NORMAL, "word2" },      { TOKEN_EOF, NORMAL, NULL },
    };

    for (long unsigned int i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++)
    {
        struct token tok = lexer_pop(l);
        cr_assert_eq(tok.type, tokens[i].type, "Incorrect token at position %d",
                     i);
        if (tokens[i].value != NULL)
        {
            cr_assert_str_eq(tok.value, tokens[i].value,
                             "Incorrect content in the string at pos %d", i);
        }
        free_token(tok);
    }
    lexer_free(l);
}

Test(lexer_new, comment, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("#command");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "str should be null");
    free_token(tok);
    lexer_free(l);
}

Test(lexer_new, comment_with_word, .init = cr_redirect_stdout)
{
    struct lexer *l = lexer_new("hello #command");
    cr_assert_not_null(l, "lexer shouldn't be null");

    struct token tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_WORD, "incorrect type");
    cr_assert_str_eq(tok.value, "hello");
    free_token(tok);
    tok = lexer_pop(l);
    cr_assert_eq(tok.type, TOKEN_EOF, "it isn't the end");
    cr_assert_null(tok.value, "value should be null");
    free_token(tok);
    lexer_free(l);
}
