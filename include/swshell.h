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
                    "\tversion - display NXSH version\r\n" \
                    "\thelp - print this message\r\n" \
                    "\tpwd - print working directory\r\n\r\n" \

#define SwitchShell_H_DIR "/SwitchShell_H"

#define SwitchShell_H_VERSION "0.1.0"

#define SWSHELL_DIR "/swshell"

/* Functions */
char *shell_reboot(int argc, char **argv, int connfd);
char *shell_shutdown(int connfd);
char *shell_ls();
char *shell_cd(char *path);
char *shell_mkdir(char *path);
char *shell_rm(int argc, char **argv);
char *shell_cp(char *src, char *dst);
char *shell_cat(int argc, char **argv);
char *shell_echo(int argc, char **argv);
char *shell_cwd();
char *shell_help(int connfd);

int recursive_delete(const char *path);

int is_file(const char *path);
int exists(const char *path);
int is_dir_empty(const char *dir);
char *error(const char *msg);
#endif
