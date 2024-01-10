#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/parser.h"
#include "quarantedeuxsh.h"

static char *get_input(enum source source, char *file);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        char *input = get_input(STDIN, NULL);
        if (input == NULL)
        {
            return 1;
        }

        int res = parser(input);

        free(input);

        return res;
    }

    if (argv[1][0] == '-')
    {
        if (strcmp(argv[1], "-c") == 0)
        {
            return parser(argv[2]);
        }
        else
        {
            errx(1, "42sh: Usage: ./42sh [OPTIONS] [SCRIPT] [ARGUMENTS...]");
        }
    }
    else
    {
        char *input = get_input(SCRIPT, argv[1]);
        if (input == NULL)
        {
            return 1;
        }

        int res = parser(input);

        free(input);

        return res;
    }

    return 1;
}

static char *get_input(enum source source, char *file)
{
    FILE *fd = source == SCRIPT ? fopen(file, "r") : stdin;
    if (fd == NULL)
    {
        return NULL;
    }

    char *input = malloc(BUFFER_SIZE);
    if (input == NULL)
    {
        return NULL;
    }

    int iter = 0;

    while (fread(input + iter * BUFFER_SIZE, sizeof(char), BUFFER_SIZE, fd)
           == BUFFER_SIZE)
    {
        char *temp = realloc(input, BUFFER_SIZE * (++iter + 1));
        if (temp == NULL)
        {
            free(input);
            return NULL;
        }
        input = temp;
    }

    fclose(fd);

    return input;
}
