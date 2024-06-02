#include <unistd.h>
#include <string.h>

#include <switch.h>
#include <swshell.h>

/**
 * Reboots the system.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @param connfd The connection file descriptor.
 * @return An error message if the command is used incorrectly, otherwise an empty string.
 */
char *shell_reboot(int argc, char **argv, int connfd) {
    if (argc == 0) {
        close(connfd);
        fsdevUnmountAll();
        bpcInitialize();
        bpcRebootSystem();
    } else if (argc == 1 && strcmp(argv[0], "--rcm") == 0) {
        close(connfd);
        svcSleepThread(1e+8L); // For some reason if don't sleep for a decisecond, the socket won't close properly.
        splInitialize();
        fsdevUnmountAll();
        splSetConfig((SplConfigItem) 65001, 1);
    } else {
        return error("Usage: reboot [options]\r\n" \
                     "Options:\r\n\t--rcm\tReboot to RCM\r\n");
    }
    return "";
}

/**
 * Shuts down the system.
 *
 * @param connfd The connection file descriptor.
 * @return An empty string.
 */
char *shell_shutdown(int connfd) {
    close(connfd);
    svcSleepThread(1e+8L);
    bpcInitialize();
    fsdevUnmountAll();
    bpcShutdownSystem();
    return "";
}