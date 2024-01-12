#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int set_flags(char *option, int *newline, int *enable)
{
    if (option[0] == '-')
    {
        for (int i = 1; option[i] != '\0'; i++)
        {
            if (option[i] != 'e' && option[i] != 'E' && option[i] != 'n')
            {
                *newline = 1;
                *enable = 1;

                return 1;
            }

            if (option[i] == 'E' && *enable == 2)
            {
                *enable = 0;
            }

            if (option[i] == 'e')
            {
                *enable = 1;
            }

            if (option[i] == 'n')
            {
                *newline = 0;
            }
        }

        return 2;
    }

    return 1;
}

static void print(int enable, char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (enable)
        {
            if (str[i] == '\\' && str[i + 1] == 'n')
            {
                printf("\n");
                i++;
            }
            else if (str[i] == '\\' && str[i + 1] == 't')
            {
                printf("\t");
                i++;
            }
            else if (str[i] == '\\' && str[i + 1] == '\\')
            {
                printf("\\");
                i++;
            }
            else
            {
                printf("%c", str[i]);
            }
        }
        else
        {
            printf("%c", str[i]);
        }
    }
}

int echo(char *args[])
{
    int newline = 1;
    int enable = 2;

    if (args[1] == NULL)
    {
        return 0;
    }

    int i = set_flags(args[1], &newline, &enable);

    if (args[i] != NULL)
    {
        print(enable, args[i]);
        i++;
    }

    for (; args[i] != NULL; i++)
    {
        printf(" ");
        print(enable, args[i]);
    }

    if (newline)
    {
        puts("");
    }

    return 0;
}
