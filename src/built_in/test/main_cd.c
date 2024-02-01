#define _POSIX_C_SOURCE 200112L

#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../built_in.h"

int main(void)
{
    char s[] = "..";
    char *args[3];
    args[0] = "cd";
    args[1] = s;
    args[2] = NULL;
    int v = cd(args);
    open("test.txt", O_CREAT);
    return v;
}
