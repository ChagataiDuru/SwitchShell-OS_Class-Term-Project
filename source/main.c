#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <swshell.h>

#include <switch.h>

#define PORT 22
#define BACKLOG 10

void xor_encrypt_decrypt(char *data, int data_len, const char *key, int key_len) {
    for (int i = 0; i < data_len; i++) {
        data[i] ^= key[i % key_len];
    }
}

int authenticate(int client_fd) {
    char username[256], password[256];

    // Simple prompt for username/password (no encryption yet)
    write(client_fd, "Username: ", 10);
    read(client_fd, username, 256);
    write(client_fd, "Password: ", 10);
    read(client_fd, password, 256);

    // Simple authentication (hardcoded for demonstration)
    if (strcmp(username, "user\n") == 0 && strcmp(password, "pass\n") == 0) {
        write(client_fd, "Authentication successful\n", 26);
        return 1;
    } else {
        write(client_fd, "Authentication failed\n", 22);
        return 0;
    }
}

void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    free(arg);

    if (!authenticate(client_fd)) {
        close(client_fd);
        return NULL;
    }

    char buffer[1024] = {0};
    const char *key = "my_secret_key";
    int key_len = strlen(key);

    while (1) {
        int valread = read(client_fd, buffer, 1024);
        if (valread <= 0) break;
        xor_encrypt_decrypt(buffer, valread, key, key_len);  // Decrypt
        buffer[valread] = '\0';  // Null-terminate the command string
        //execute_command(buffer, client_fd);
    }
    close(client_fd);
    return NULL;
}

int setup_server() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

void accept_connections(int server_fd, ThreadList *thread_list) {
    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_addrlen = sizeof(client_address);
        int *client_fd = malloc(sizeof(int));

        *client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen);
        if (*client_fd < 0) {
            perror("accept");
            free(client_fd);
            continue;
        }

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, client_fd);
        add_thread(thread_list, thread_id);
    }
}

// Get and print the IP address of the Nintendo Switch
void print_switch_ip() {

    struct in_addr addr;
    inet_aton("192.168.137.1", &addr);  // Replace with your router's IP address as needed

    u32 ip;
    nifmGetCurrentIpAddress(&ip);

    printf("Switch IP: %s\n", inet_ntoa(*(struct in_addr *)&ip));

    socketExit();
}   

// Main program entrypoint
int main(int argc, char* argv[]) {
    // Initialize the console
    consoleInit(NULL);

    // Configure input layout
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad
    PadState pad;
    padInitializeDefault(&pad);

    // Initialize networking
    socketInitializeDefault();
    romfsInit();
	nifmInitialize(NifmServiceType_User);
    int server_fd = setup_server();

    // Initialize thread list
    ThreadList thread_list;
    init_thread_list(&thread_list, 10);

    // Print hello world message
    printf("Hello World!\n");

    // Print the IP address of the Switch
    //print_switch_ip();

    // Create a thread to handle SSH connections
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, (void *(*)(void *))accept_connections, &thread_list);

    // Main loop
    while (appletMainLoop()) {
        // Scan the gamepad
        padUpdate(&pad);

        // Get buttons that are newly pressed
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
            break; // Exit to hbmenu

        // Update the console
        consoleUpdate(NULL);
    }

    // Cleanup
    close(server_fd);
    join_and_cleanup_threads(&thread_list);
    pthread_cancel(server_thread); // Ensure server thread is stopped
    socketExit();
    consoleExit(NULL);
    return 0;
}
