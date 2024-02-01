#define _POSIX_C_SOURCE 200112L

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "built_in.h"

static char *concat_pwd_curpath(char *pwd, char *curpath)
{
    size_t strlen_pwd = strlen(pwd);
    size_t strlen_curpath = strlen(curpath);
    if (curpath[0] != '/' && pwd[strlen_pwd - 1] != '/')
    {
        char *new = calloc((strlen_pwd + strlen_curpath + 2), sizeof(char));
        if (!new)
        {
            return NULL;
        }
        strcpy(new, pwd);
        new[strlen_pwd] = '/';
        strcat(new, curpath);
        new[strlen_pwd + strlen_curpath + 1] = '\0';
        return new;
    }
    if (curpath[0] != '/' && pwd[strlen_pwd - 1] == '/')
    {
        char *new = calloc((strlen_pwd + strlen_curpath + 1), sizeof(char));
        if (!new)
        {
            return NULL;
        }
        strcat(new, pwd);
        strcat(new, curpath);
        new[strlen_curpath + strlen_pwd] = '\0';
        return new;
    }
    return NULL;
}

static char *remove_slash(char *curpath)
{
    size_t strlen_curpath = strlen(curpath);
    char *new = calloc(strlen_curpath + 1, sizeof(char));
    size_t i = 0;
    size_t len = 0;
    while (curpath[i] != '\0')
    {
        if (curpath[i] == '/')
        {
            new[len++] = '/';
            i++;
            size_t j = 0;
            while (curpath[i + j] == '/')
            {
                j++;
            }
            i = i + j - 1;
        }
        else
        {
            new[len++] = curpath[i];
        }
        i++;
    }
    new[len] = '\0';
    free(curpath);
    return new;
}

static char *remove_dot_slash(char *curpath)
{
    size_t strlen_curpath = strlen(curpath);
    char *new_curpath = calloc(strlen_curpath + 1, sizeof(char));
    size_t len = 0;
    for (size_t i = 0; i < strlen_curpath; i++)
    {
        if (curpath[i] == '.'
            && (curpath[i + 1] != '.' && (i > 0 && curpath[i - 1] != '.')))
        {
            while (curpath[i + 1] == '/')
            {
                i++;
            }
        }
        else
        {
            new_curpath[len] = curpath[i];
            len++;
        }
    }
    new_curpath[len] = '\0';
    return new_curpath;
}

static char **component_array(char *curpath)
{
    char **array = NULL;
    char *sub_string = NULL;
    size_t len_array = 0;
    size_t len_sub_string = 0;
    if (curpath[0] == '/')
    {
        len_array++;
        array = malloc(sizeof(char *));
        if (!array)
        {
            return NULL;
        }
        char *slash = malloc(sizeof(char) * 2);
        slash[0] = '/';
        slash[1] = '\0';
        array[0] = slash;
    }
    size_t i = len_array;
    for (; i < strlen(curpath); i++)
    {
        array = realloc(array, sizeof(char *) * (len_array + 3));
        if (!array)
        {
            return NULL;
        }
        for (size_t j = 0; curpath[i + j] != '/' && curpath[i + j] != '\0'; j++)
        {
            sub_string =
                realloc(sub_string, (len_sub_string + 2) * sizeof(char));
            if (!sub_string)
            {
                return NULL;
            }
            sub_string[j] = curpath[i + j];
            len_sub_string++;
        }
        sub_string[len_sub_string] = '\0';
        array[len_array] = sub_string;
        sub_string = NULL;
        i = i + len_sub_string;
        len_sub_string = 0;
        len_array++;
    }
    free(curpath);
    array[len_array] = NULL;
    return array;
}

static int is_directory(char *path)
{
    struct stat st;
    int res_stat = 0;
    res_stat = stat(path, &st);
    if (res_stat != 0)
    {
        return -1;
    }
    if (S_ISDIR(st.st_mode) == 1)
    {
        return 1;
    }
    if (S_ISDIR(st.st_mode) == 0)
    {
        return 0;
    }
    return 2;
}

static void free_list(char **list)
{
    for (int i = 0; list[i] != NULL; i++)
    {
        free(list[i]);
    }
    free(list);
}

static char **remove_dot_dot(char **component)
{
    size_t i = 0;
    size_t len = 0;
    for (; component[len] != NULL; len++)
    {
        continue;
    }
    for (; component[i] != NULL; i++)
    {
        if (strcmp(component[i], "..") == 0)
        {
            if (i
                && is_directory(component[i - 1])
                    == 0) // si c'est pas un directory
            {
                free(component[i]);
                for (size_t k = i; component[k]; k++)
                {
                    component[k] = component[k + 1];
                }
                component[len - 1] = NULL;
                len = len - 1;
                i = i - 1;
            }
            else if (i && is_directory(component[i - 1]) == 0)
            {
                free_list(component);
                errx(1, "No existant directory");
            }
            else
            {
                free(component[i]);
                free(component[i - 1]);
                for (size_t j = i; component[j] != NULL; j++)
                {
                    component[j - 1] = component[j + 1];
                }
                component[len - 1] = NULL;
                component[len - 2] = NULL;
                len = len - 2;
                i = i - 2;
            }
        }
    }
    return component;
}

static char *simplify_curpath(char *curpath_path, int to_free)
{
    char *new_curpath = remove_dot_slash(curpath_path);
    if (to_free == 1)
    {
        free(curpath_path);
    }
    new_curpath = remove_slash(new_curpath);
    size_t strlen_new_curpath = strlen(new_curpath);
    char **component = component_array(new_curpath);
    char **new_component = remove_dot_dot(component);
    char *component_string = calloc(sizeof(char), (strlen_new_curpath + 2));
    size_t len_string = 0;
    size_t i = 0;
    if (new_component[0][0] == '/')
    {
        component_string[0] = '/';
        i = i + 1;
        len_string = 1;
        free(new_component[0]);
    }
    for (; new_component[i] != NULL; i++)
    {
        for (size_t j = 0; new_component[i][j] != '\0'; j++)
        {
            component_string[len_string] = new_component[i][j];
            len_string = len_string + 1;
        }
        component_string[len_string] = '/';
        len_string = len_string + 1;
        free(new_component[i]);
    }
    free(new_component);
    if (strlen(component_string) > 1 && component_string[len_string - 1] == '/')
    {
        component_string[len_string - 1] = '\0';
    }
    return component_string;
}

char *cd_moins(void)
{
    char *oldpwd = getenv("OLDPWD");
    if (oldpwd == NULL)
    {
        return NULL;
    }
    if (strlen(oldpwd) == 0)
    {
        return NULL;
    }
    char *s = calloc(sizeof(char), strlen(oldpwd) + 3);
    for (size_t i = 0; oldpwd[i] != '\0'; i++)
    {
        s[i] = oldpwd[i];
    }
    return s;
}

int cd(char *args[])
{
    char *curpath_path = NULL;
    char pwd_path[1024];
    if (getcwd(pwd_path, 1024) == NULL)
    {
        return 1;
    }
    char *home_path = getenv("HOME");
    if (args[1] == NULL && getenv("HOME") == NULL)
    {
        return 1;
    }
    if (args[1] == NULL && getenv("HOME") != NULL)
    {
        curpath_path = home_path;
        setenv("PWD", curpath_path, 1);
    }
    else
    {
        if (strcmp(args[1], "-") == 0)
        {
            curpath_path = cd_moins();
            if (chdir(curpath_path) == -1)
            {
                free(curpath_path);
                return 1;
            }
            free(curpath_path);
            return 0;
        }
        curpath_path = args[1];
    }
    int to_free = 0;
    if (curpath_path[0] != '/')
    {
        char *new = concat_pwd_curpath(pwd_path, curpath_path);
        curpath_path = new;
        to_free = 1;
    }
    char *new_curpath = simplify_curpath(curpath_path, to_free);
    if (new_curpath == NULL)
    {
        return 1;
    }
    if (chdir(new_curpath) == -1)
    {
        free(new_curpath);
        return 1;
    }
    setenv("PWD", new_curpath, 1);
    free(new_curpath);
    return 0;
}
