#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "built_in.h"

int my_exit(char *args[])
{
    if (args == NULL)
    {
        return -1;
    }
    if (args[1] == NULL)
    {
        return -1;
    }
    if (args[2] == NULL)
    {
        if (strcmp(args[1], "0") == 0)
        {
            return 0;
        }
        int ret = atoi(args[1]);
        if (ret == 0)
        {
            return -1;
        }
        return ret;
    }
    else
    {
        return -1;
    }
}
