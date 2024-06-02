#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <swshell.h>

/**
 * Changes the current working directory to the specified path.
 *
 * @param path The path of the directory to change to.
 * @return A success message if the directory was changed successfully, or an error message if not.
 */
char *shell_cd(char *path) {
    if (path == NULL) {
        return error("No directory specified.\r\n");
    }

    if (chdir(path) != 0) {
        return error("Failed to change directory.\r\n");
    } else {
        // Allocate memory for the success message
        char *msg = malloc(21); // "Directory changed.\r\n" + 1 for null terminator
        if (msg != NULL) {
            strcpy(msg, "Directory changed.\r\n");
        }
        return msg;
    }
}
