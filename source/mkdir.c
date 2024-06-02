#include <sys/stat.h>
#include <string.h>
#include <swshell.h>
#include <stdlib.h>

/**
 * Creates a directory with the specified path.
 *
 * @param path The path of the directory to be created.
 * @return A success message if the directory is created, or an error message if it fails.
 */
char *shell_mkdir(char *path) {
    if (path == NULL) {
        return error("No directory name specified.\r\n");
    }

    if (mkdir(path, 0755) != 0) {
        return error("Failed to create directory.\r\n");
    } else {
        // Allocate memory for the success message
        char *msg = malloc(25);
        if (msg != NULL) {
            strcpy(msg, "Directory created.\r\n");
        }
        return msg;
    }
}
