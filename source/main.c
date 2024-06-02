
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

void execute_command(int connfd, char *command) {
    char *argv[10];
    int argc = 0;
    //echo the command
    send(connfd, command, strlen(command), 0);
    // Tokenize the command into arguments
    char *token = strtok(command, " ");
    while (token != NULL && argc < 10) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    send(connfd, "\n", 1, 0);
    // Execute the appropriate command
    if (argc > 0) {
        if (strcmp(argv[0], "ls") == 0) {
            shell_ls(argc - 1, &argv[1], connfd);
        } else if (strcmp(argv[0], "cd") == 0) {
            shell_cd(argc - 1, &argv[1], connfd);
        } else if (strcmp(argv[0], "mkdir") == 0) {
            shell_mkdir(argc - 1, &argv[1], connfd);
        } else if (strcmp(argv[0], "rm") == 0) {
            shell_rm(argc - 1, &argv[1], connfd);
        } else if (strcmp(argv[0], "cp") == 0) {
            shell_cp(argc - 1, &argv[1], connfd);
        } else if (strcmp(argv[0], "cat") == 0) {
            shell_cat(argc - 1, &argv[1], connfd);
        } else if (strcmp(argv[0], "echo") == 0) {
            shell_echo(argc - 1, &argv[1], connfd);
        } else if (strcmp(argv[0], "help") == 0) {
            shell_help(connfd);
        } else if (strcmp(argv[0], "reboot") == 0) {
            shell_reboot(argc - 1, &argv[1], connfd);
        } else if (strcmp(argv[0], "shutdown") == 0) {
            shell_shutdown(connfd);
        } else {
            const char *msg = "Unknown command. Type 'help' for a list of commands.\n";
            send(connfd, msg, strlen(msg), 0);
            // echo the received command
            send(connfd, "Received: ", 10, 0);
            // echo the command
            send(connfd, command, strlen(command), 0);
        }
    }
}

void shell_session(int connfd) {
    char buffer[BUFFER_SIZE];
    ssize_t len;

    // Send a welcome message
    const char *welcome_msg = "Welcome to SwitchShell! Type your commands below:\n";
    send(connfd, welcome_msg, strlen(welcome_msg), 0);

    // Main session loop
    while (1) {
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Prompt for command
        const char *prompt = "SwitchShell> ";
        send(connfd, prompt, strlen(prompt), 0);

        // Read command from client
        len = recv(connfd, buffer, BUFFER_SIZE, 0);
        if (len <= 0) {
            // Connection closed or error
            break;
        }

        // Strip the newline character if it exists
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        const char *recv_msg = "Received: ";
        send(connfd, recv_msg, strlen(recv_msg), 0);
        // Echo the command back to the client
        send(connfd, buffer, strlen(buffer), 0);
        // Execute command
        execute_command(connfd, buffer);
    }

    // Close the connection
    close(connfd);
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