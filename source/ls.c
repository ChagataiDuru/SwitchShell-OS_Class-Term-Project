#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <swshell.h>

/**
 * Lists the contents of the current directory.
 *
 * @return A dynamically allocated string containing the names of the files and directories in the current directory,
 *         separated by "\r\n". The string must be freed by the caller.
 * @throws An error message if the directory contents cannot be listed.
 */
char *shell_ls() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        char *output = malloc(sizeof(char) * 4096); // Allocate a large buffer
        output[0] = '\0'; // Ensure the string is null-terminated

        while ((dir = readdir(d)) != NULL) {
            strcat(output, dir->d_name);
            strcat(output, "\r\n");
        }
        closedir(d);
        return output;
    } else {
        return error("Failed to list directory contents\r\n");
    }
}
