#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <swshell.h>

// Recursively delete directory
int recursive_delete(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        return -1;
    }

    struct dirent *entry;
    int result = 0;
    size_t path_len = strlen(path);

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        size_t buf_len = path_len + strlen(entry->d_name) + 2;
        char *buf = malloc(buf_len);
        if (!buf) {
            result = -1;
            break;
        }

        snprintf(buf, buf_len, "%s/%s", path, entry->d_name);
        struct stat statbuf;
        if (stat(buf, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                result = recursive_delete(buf);
            } else {
                result = remove(buf);
            }
        } else {
            result = -1;
        }

        free(buf);
        if (result != 0) {
            break;
        }
    }

    closedir(dir);

    if (result == 0) {
        result = rmdir(path);
    }

    return result;
}

// Removes a file or directory
char *nxsh_rm(int argc, char **argv) {
    if (argc < 2) {
        return error("Usage: rm [options] [file/directory]\r\nOptions:\r\n\t-r\trecurse into directory\r\n");
    }

    int recursive_flag = 0;
    char *targets[128];
    int target_count = 0;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; ++j) {
                if (argv[i][j] == 'r') {
                    recursive_flag = 1;
                } else {
                    return error("Usage: rm [options] [file/directory]\r\nOptions:\r\n\t-r\trecurse into directory\r\n");
                }
            }
        } else {
            targets[target_count++] = argv[i];
        }
    }

    for (int i = 0; i < target_count; ++i) {
        if (!exists(targets[i])) {
            return error("Error: no such file or directory\r\n");
        }

        if (!is_file(targets[i])) {
            if (!recursive_flag) {
                size_t error_msg_len = strlen(targets[i]) + 64;
                char *error_msg = malloc(error_msg_len);
                if (error_msg) {
                    snprintf(error_msg, error_msg_len, "Error: cannot recurse into directory '%s'\r\n", targets[i]);
                    char *err = error(error_msg);
                    free(error_msg);
                    return err;
                } else {
                    return error("Error: memory allocation failed.\r\n");
                }
            }

            if (recursive_delete(targets[i]) != 0) {
                return error("Error: could not delete specified directory.\r\n");
            }
        } else {
            if (remove(targets[i]) != 0) {
                return error("Error: could not delete specified file.\r\n");
            }
        }
    }

    return NULL;
}
