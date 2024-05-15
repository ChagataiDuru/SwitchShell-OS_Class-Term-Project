#include <stdlib.h>

#include <switch.h>
#include <m_threads.h>

#ifndef SwitchShell_H
#define SwitchShell_H

/* Consts */
#define SwitchShell_H_WELCOME "Welcome to SwitchShell! Type 'help' for a list of available commands.\r\n"
#define SwitchShell_H_SEPARATOR "-------------------------------------------------------------\r\n"
#define SwitchShell_H_HELP "List of available commands:\r\n" \
                    "\tls - get a directory listing\r\n" \
                    "\tcd - change directory\r\n" \
                    "\tmkdir - make directory\r\n" \
                    "\trm - remove file or directory\r\n" \
                    "\tcp - copy file or directory\r\n" \
                    "\tcat - print file on standard output\r\n" \
                    "\treboot - reboots the console\r\n" \
                    "\tshutdown - turns off the console\r\n" \
                    "\techo - echo the arguments\r\n" \
                    "\thelp - print this message\r\n\r\n" \


#define SwitchShell_H_DIR "/SwitchShell_H"

#define SwitchShell_H_VERSION "0.1.0"

/* Functions */
void xor_encrypt_decrypt(char *data, int data_len, const char *key, int key_len);
int authenticate(int client_fd);
void execute_command(const char *command, int client_fd);
void *handle_client(void *arg);
int setup_server();
void accept_connections(int server_fd, ThreadList *thread_list);
#endif