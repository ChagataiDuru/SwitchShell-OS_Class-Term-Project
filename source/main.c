#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

int SwitchShell_LOGGING_ENABLED;
#define SwitchShell_PASSWORD_PROMPT "Enter password: "
#define SwitchShell_PASSWORD_ERROR "\r\nIncorrect password entered\r\n"

#include <swshell.h>
#include <switch.h>

#ifdef __SYS__
#include <sysmodule.h>
#endif

int setupServerSocket(int *lissock) {
    struct sockaddr_in serv_addr;
    
    // Create socket
    *lissock = socket(AF_INET, SOCK_STREAM, 0);
    if (*lissock < 0) {
        printf("Failed to initialize socket\n");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(23);

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

int main(int argc, char **argv) {
    nifmInitialize(NifmServiceType_Admin);
    #ifndef __SYS__
    socketInitializeDefault(); // The sysmodule does this in __appInit(), otherwise it gets glitchy
    consoleInit(NULL); // If the sysmodule tries to do this it crashes
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

            // Send the welcome message
            send(connfd, SEPARATOR, strlen(SEPARATOR)+1, 0);
            send(connfd, SwitchShell_H_WELCOME, strlen(SwitchShell_H_WELCOME)+1, 0);
            send(connfd, SEPARATOR, strlen(SEPARATOR)+1, 0);

            // Get password file path
            char *pw_file = malloc(sizeof(char) * (strlen(SWSHELL_DIR) + 10));
            sprintf(pw_file, "%s/nxsh.pw", SWSHELL_DIR);

            // See if we need to prompt for the password
            if (exists(pw_file)) {
                free(pw_file);
                char *pw_buf = malloc(sizeof(char) * 256);
                size_t len;
                int authenticated = 0;

                // Loop until a correct password is entered
                while (!authenticated) {
                    send(connfd, SwitchShell_PASSWORD_PROMPT, strlen(SwitchShell_PASSWORD_PROMPT)+1, 0);
                    len = recv(connfd, pw_buf, 256, 0);

                    // Error occurred on socket receive
                    if (len <= 0) {
                        close(connfd);
                        free(pw_buf);
                        return 0;
                    }
                    else {

                        // Strip the newline character, if it exists
                        if (pw_buf[len-1] == '\n')
                            pw_buf[len-1] = '\0';
                        else
                            pw_buf[len] = '\0';

                        trim(pw_buf);

                        // Try authentication
                        if (nxsh_authenticate(pw_buf))
                            authenticated = 1;
                        else
                            send(connfd, SwitchShell_PASSWORD_ERROR, strlen(SwitchShell_PASSWORD_ERROR)+1, 0);
                    }
                }
                free(pw_buf);
            }

            // Here is where we'd ideally implement some multithreading logic
            nxsh_session(connfd);
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
