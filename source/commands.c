#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/socket.h>

#include <switch.h>
#include "swshell.h"

char *shell_ls(int argc, char **argv, int connfd) {
    DIR *dir;
    struct dirent *entry;
    char buffer[1024] = "";

    if (argc == 0) {
        dir = opendir(".");
    } else {
        dir = opendir(argv[0]);
    }

    if (dir == NULL) {
        snprintf(buffer, sizeof(buffer), "Error opening directory: %s\n", strerror(errno));
        send(connfd, buffer, strlen(buffer), 0);
        return "";
    }

    while ((entry = readdir(dir)) != NULL) {
        snprintf(buffer, sizeof(buffer), "%s\n", entry->d_name);
        send(connfd, buffer, strlen(buffer), 0);
    }

    closedir(dir);
    return "";
}

char *shell_cd(int argc, char **argv, int connfd) {
    if (argc == 0) {
        return "Usage: cd <directory>\n";
    }

    if (chdir(argv[0]) != 0) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "Error changing directory: %s\n", strerror(errno));
        send(connfd, buffer, strlen(buffer), 0);
    }

    return "";
}

char *shell_mkdir(int argc, char **argv, int connfd) {
    if (argc == 0) {
        return "Usage: mkdir <directory>\n";
    }

    if (mkdir(argv[0], 0755) != 0) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "Error creating directory: %s\n", strerror(errno));
        send(connfd, buffer, strlen(buffer), 0);
    }

    return "";
}

char *shell_rm(int argc, char **argv, int connfd) {
    if (argc == 0) {
        return "Usage: rm <file>\n";
    }

    if (remove(argv[0]) != 0) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "Error removing file: %s\n", strerror(errno));
        send(connfd, buffer, strlen(buffer), 0);
    }

    return "";
}

char *shell_cp(int argc, char **argv, int connfd) {
    if (argc < 2) {
        return "Usage: cp <source> <destination>\n";
    }

    int src_fd = open(argv[0], O_RDONLY);
    if (src_fd < 0) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "Error opening source file: %s\n", strerror(errno));
        send(connfd, buffer, strlen(buffer), 0);
        return "";
    }

    int dest_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        close(src_fd);
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "Error opening destination file: %s\n", strerror(errno));
        send(connfd, buffer, strlen(buffer), 0);
        return "";
    }

    char buf[4096];
    ssize_t bytes;
    while ((bytes = read(src_fd, buf, sizeof(buf))) > 0) {
        write(dest_fd, buf, bytes);
    }

    close(src_fd);
    close(dest_fd);

    return "";
}

char *shell_cat(int argc, char **argv, int connfd) {
    if (argc == 0) {
        return "Usage: cat <file>\n";
    }

    int fd = open(argv[0], O_RDONLY);
    if (fd < 0) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "Error opening file: %s\n", strerror(errno));
        send(connfd, buffer, strlen(buffer), 0);
        return "";
    }

    char buf[4096];
    ssize_t bytes;
    while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
        send(connfd, buf, bytes, 0);
    }

    close(fd);
    return "";
}

char *shell_echo(int argc, char **argv, int connfd) {
    for (int i = 0; i < argc; i++) {
        send(connfd, argv[i], strlen(argv[i]), 0);
        if (i < argc - 1) {
            send(connfd, " ", 1, 0);
        }
    }
    send(connfd, "\n", 1, 0);
    return "";
}

char *shell_help(int connfd) {
    send(connfd, SwitchShell_H_HELP, strlen(SwitchShell_H_HELP), 0);
    return "";
}

/*
    Allocates buffer for given message, and copies it in
    @param msg - pointer to error message

    @returns - pointer to allocated memory
*/
char *error(char *msg) {
    char *ptr = malloc(strlen(msg)+1);
    strcpy(ptr, msg);
    return ptr;
}