#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <swshell.h>

/**
 * Copies a file from the source path to the destination path.
 *
 * @param src The path of the source file.
 * @param dst The path of the destination file.
 * @return A success message if the file is copied successfully, or an error message if any error occurs.
 */
char *shell_cp(char *src, char *dst) {
    if (src == NULL || dst == NULL) {
        return error("Source or destination not specified.\r\n");
    }

    FILE *source = fopen(src, "rb");
    if (source == NULL) {
        return error("Failed to open source file.\r\n");
    }

    FILE *destination = fopen(dst, "wb");
    if (destination == NULL) {
        fclose(source);
        return error("Failed to open destination file.\r\n");
    }

    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, destination);
    }

    fclose(source);
    fclose(destination);

    // Allocate memory for the success message
    char *msg = malloc(15);
    if (msg != NULL) {
        strcpy(msg, "File copied.\r\n");
    }
    return msg;
}
