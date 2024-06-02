#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#include <switch.h>
#include "swshell.h"

/*
    Allocates buffer for given message, and copies it in
    @param msg - pointer to error message

    @returns - pointer to allocated memory
*/
char *error(const char *msg) {
    char *error_msg = malloc(strlen(msg) + 1);
    if (error_msg != NULL) {
        strcpy(error_msg, msg);
    }
    return error_msg;
}

/*
    Check if directory is empty
    @param dir - directory in question

    @returns - 1 if empty, 0 if not
*/
int is_dir_empty(const char *dir) {
    int n = 0;
    struct dirent *entry;
    DIR *handle = opendir(dir);

    if (handle == NULL) {
        return 1; // Consider directory as empty if it cannot be opened
    }

    while ((entry = readdir(handle)) != NULL) {
        if (++n > 2) {
            break; // Directory is not empty if more than two entries found
        }
    }

    closedir(handle);
    return n <= 2 ? 1 : 0; // Return 1 if directory is empty, 0 otherwise
}

/*
    Check if specified path is a file
    @param name - path in question

    @returns - 1 if file, 0 if not
*/
int is_file(const char *path) {
    struct stat buf;
    if (stat(path, &buf) != 0) {
        return 0; // Return 0 if stat fails
    }
    return S_ISREG(buf.st_mode);
}

/*
    Check if file exists
    @param name - path in question

    @returns - 1 if it does, 0 if not
*/
int exists(const char *path) {
    return access(path, F_OK) != -1;
}
