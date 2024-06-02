
int SwitchShell_LOGGING_ENABLED;
#define SwitchShell_PASSWORD_PROMPT "Enter password: "
#define SwitchShell_PASSWORD_ERROR "\r\nIncorrect password entered\r\n"

#include <swshell.h>
#include <switch.h>

#ifdef __SYS__
#include <sysmodule.h>
#endif


#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int setupServerSocket(int *lissock) {
    struct sockaddr_in serv_addr;
    
    // Create socket
    *lissock = socket(AF_INET, SOCK_STREAM, 0);
    if (*lissock < 0) {
        printf("Failed to initialize socket\n");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(2323);

    // Reuse address
    int yes = 1;
    setsockopt(*lissock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    // Bind to the socket
    while (bind(*lissock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        #ifdef __SYS__
        svcSleepThread(1e+9L);
        #else
        printf("Failed to bind: error %d\n", errno);
        break;
        #endif
    }
    
    return listen(*lissock, 5); // Listen on the apropriate address
}

void shell_session(int connfd) {
    char buffer[1024];
    ssize_t len;
    char *command, *arg1, *arg2;
    
    while (1) {
        // Send prompt to client
        send(connfd, "SwitchShell> ", 13, 0);
        
        // Receive command from client
        len = recv(connfd, buffer, sizeof(buffer) - 1, 0);
        
        if (len <= 0) {
            break;  // Connection closed or error
        }
        
        buffer[len] = '\0';  // Null-terminate the received string
        
        // Remove newline characters
        char *newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';
        newline = strchr(buffer, '\r');
        if (newline) *newline = '\0';
        
        // Parse the command and arguments
        command = strtok(buffer, " ");
        arg1 = strtok(NULL, " ");
        arg2 = strtok(NULL, " ");
        
        //Echo the command with the Text "Executing: "
        char *output = malloc(strlen(command) + 11);
        strcpy(output, "Executing: ");
        strcat(output, command);
        strcat(output, "\r\n");
        send(connfd, output, strlen(output), 0);
        free(output);

        // Escape sequence for testing
        char escape[2];
        escape[0] = 0x04;
        escape[1] = '\0';

        // Execute the command
        if (command == NULL) {
            continue;
        } else if (strcmp(command, "ls") == 0) {
            char *output = shell_ls();
            send(connfd, output, strlen(output)+1, 0);
            free(output);
        } else if (strcmp(command, "cd") == 0) {
            char *output = shell_cd(arg1);
            send(connfd, output, strlen(output)+1, 0);
            free(output);
        } else if (strcmp(command, "mkdir") == 0) {
            char *output = shell_mkdir(arg1);
            send(connfd, output, strlen(output)+1, 0);
            free(output);
        } /*else if (strcmp(command, "rm") == 0) {
            char *output = shell_rm(arg1);
            send(connfd, output, strlen(output)+1, 0);
            free(output);
        }*/ else if (strcmp(command, "cp") == 0) {
            char *output = shell_cp(arg1, arg2);
            send(connfd, output, strlen(output)+1, 0);
            free(output);
        } else if (strcmp(command, "cat") == 0) {
            char *output = shell_cat(1, &arg1);
            send(connfd, output, strlen(output)+1, 0);
            free(output);
        } else if (strcmp(command, "reboot") == 0) {
            char *output = shell_reboot(0, NULL, connfd);
            send(connfd, output, strlen(output)+1, 0);
            free(output);
        } else if (strcmp(command, "shutdown") == 0) {
            char *output = shell_shutdown(connfd);
            send(connfd, output, strlen(output)+1, 0);
            free(output);
        } else if (strcmp(command, "echo") == 0) {
            char *output = shell_echo(1, &arg1);
            send(connfd, output, strlen(output)+1, 0);
            free(output);
        } else if (strcmp(command, "help") == 0) {
            send(connfd, SwitchShell_H_HELP, strlen(SwitchShell_H_HELP)+1, 0);
        } else if (strcmp(command, "version") == 0) {
            output = malloc(sizeof(char) * strlen(SwitchShell_H_VERSION)+16);
            strcpy(output, "SwitchShell ");
            strcat(output, SwitchShell_H_VERSION);
            strcat(output, "\r\n");
            send(connfd, output, strlen(output)+1, 0);
        } 
        // Print working directory
        else if (strcmp(command, "pwd") == 0) {
            char *path = shell_cwd();
            size_t len = strlen(path);

            output = malloc(sizeof(char) * (len+3));
            strcpy(output, path);
            output[len++] = '\r';
            output[len++] = '\n';
            output[len] = '\0';
            // Send the path
            send(connfd, output, len, 0);
            free(output);
        }
        // End the session
        else if (strcmp(command, "exit") == 0) {
            close(connfd);
            break;
        }
        // End the session (Ctrl+D)
        else if (strcmp(command, escape) == 0) {
            close(connfd);
            break;
        }
        else {
            send(connfd, "Unknown command. Type 'help' for a list of commands.\r\n", 52, 0);
        }
    }
    
    close(connfd);  // Close the connection
}

int main(int argc, char **argv) {
    nifmInitialize(NifmServiceType_Admin);
    #ifndef __SYS__
    socketInitializeDefault();
    consoleInit(NULL);
    #endif

    printf("  _________       .__  __         .__      _________.__           .__  .__ \r\n");
    printf(" /   _____/_  _  _|__|/  |_  ____ |  |__  /   _____/|  |__   ____ |  | |  |  \r\n");
    printf(" \\_____  \\ \\/ \\/ /  \\   __\\/ ___\\|  |  \\ \\_____  \\ |  |  \\_/ __ \\|  | |  |  \\\r\n");
    printf(" /        \\     /|  ||  | \\  \\___|   Y  \\/        \\|   Y  \\  ___/|  |_|  |__\r\n");
    printf("/_______  / \\/\\_/ |__||__|  \\___  >___|  /_______  /|___|  /\\___  >____/____/\r\n");
    printf("        \\/                      \\/     \\/        \\/      \\/     \\/           \r\n");
    printf("===========================\r\n");
    printf("Welcome to SwitchShell %s\r\n", SwitchShell_H_VERSION);  
    printf("===========================\r\n\r\n");
    consoleUpdate(NULL);

    int listenfd;
    int rc = setupServerSocket(&listenfd);
    
    if (rc != 0) {
        printf("Failed to listen\n");
    }
    else {
        char hostname[128];
        gethostname(hostname, sizeof(hostname));
        printf("Listening on %s:%d...\n", hostname, 23);
        consoleUpdate(NULL);

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Main loop
        for (;;) {
            int connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
            
            #ifdef __SYS__
            if (connfd <= 0) { // Accepting fails after sleep
                svcSleepThread(1e+9L);
                close(listenfd);
                setupServerSocket(&listenfd);
                continue;
            }
            #endif
            
            printf("New connection established from %s\n", inet_ntoa(client_addr.sin_addr));
            consoleUpdate(NULL);

            // Start a new shell session
            shell_session(connfd);
        }
    }

    consoleUpdate(NULL);
    socketExit();
    nifmExit();
    #ifndef __SYS__
    consoleExit(NULL);
    #endif

    return 0;
}