#ifndef SwitchShell_H
#define SwitchShell_H

/* Consts */
#define SwitchShell_H_WELCOME "Welcome to SwitchShell! Type 'help' for a list of available commands.\r\n"
#define SEPARATOR "-------------------------------------------------------------\r\n"
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

#define SWSHELL_DIR "/swshell"

/* Functions */
char *shell_reboot(int argc, char **argv, int connfd);
char *shell_shutdown(int connfd);
char *shell_ls(int argc, char **argv, int connfd);
char *shell_cd(int argc, char **argv, int connfd);
char *shell_mkdir(int argc, char **argv, int connfd);
char *shell_rm(int argc, char **argv, int connfd);
char *shell_cp(int argc, char **argv, int connfd);
char *shell_cat(int argc, char **argv, int connfd);
char *shell_echo(int argc, char **argv, int connfd);
char *shell_help(int connfd);
char *error(char *msg);
#endif
