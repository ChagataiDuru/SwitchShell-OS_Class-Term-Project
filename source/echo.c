#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <swshell.h>

/**
 * Concatenates the command line arguments into a single string and adds "\r\n" at the end.
 * 
 * @param argc The number of command line arguments.
 * @param argv An array of strings representing the command line arguments.
 * @return A dynamically allocated string containing the concatenated arguments with "\r\n" at the end.
 *         The caller is responsible for freeing the memory.
 */
char *shell_echo(int argc, char **argv) {
    int out_length = 0;
    for (int i = 0; i < argc; i++) {
        out_length += strlen(argv[i]) + 1;
    }

    char *out = malloc(sizeof(char) * (out_length + 3)); // Include space for "\r\n\0"
    if (out != NULL) {
        memset(out, '\0', out_length + 3);
        for (int i = 0; i < argc; i++) {
            strcat(out, argv[i]);
            strcat(out, " ");
        }
        strcat(out, "\r\n");
    }

    return out;
}
