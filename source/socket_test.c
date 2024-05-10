// Include the main libnx system header, for Switch development
#include <switch.h>

#include <unistd.h>
#include <malloc.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/unistd.h>


#define PORT 6868

void Sleep(int milliseconds) {
    svcSleepThread(1000000 * milliseconds);
}

int main(int argc, char** argv) {
    // Initialize gfx and console.
    gfxInitDefault();
    // allocate buffer for SOC service
    int ret = 0;
    u32* SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

    if(SOC_buffer == NULL) {
        failExit("memalign: failed to allocate\n");
    }

    // Now intialise soc:u service
    if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
        failExit("socInit: 0x%08X\n", (unsigned int)ret);
    }
    consoleInit(GFX_TOP, NULL);

    int sock = 0;
    struct sockaddr_in server_addr;
    char buffer[1024] = { 0 };

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << CONSOLE_RED << "Error occurred on socket creation.\n" << CONSOLE_RESET;
        return -1;
    }

    // Main loop
    while (aptMainLoop()) {
        // Scan all inputs.
        hidScanInput();

        u32 kDown = hidKeysDown();

        if (kDown & KEY_START)
            break; // Exit the application.

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    // Exit
    gfxExit();
    return 0;
}