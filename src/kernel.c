#include "cli.h"

#define MAX_CMD_SIZE 100
#define CMD_TRACKER_SIZE 20
char CMD_TRACKER[CMD_TRACKER_SIZE][MAX_CMD_SIZE];
int LAST_STATE_TRACKER_INDEX = 0;
int CMD_TRACKER_INDEX = 0;
int accessHistory = 0;

void navigateCommandHistory(char *cli_buffer, int *index, int direction);
void cli();

void main() {
    // set up serial console
    uart_init();

    printf("\033[1;31m", "\x1b[40m");
    // show a Welcome Message when the OS successfully boot up
    printf(
    " _______   _______   _______  _________   _______  ___   ___  ________  ________          \n"
    "|\\  ___ \\ |\\  ___ \\ |\\  ___ \\|\\___   ___\\/  ___  \\|\\  \\ |\\  \\|\\  ___  \\|\\   __  \\         \n"
    "\\ \\   __/|\\ \\   __/|\\ \\   __/\\|___ \\  \\_/__/|_/  /\\ \\  \\\\ \\  \\ \\____   \\ \\  \\|\\  \\        \n"
    " \\ \\  \\_|/_\\ \\  \\_|/_\\ \\  \\_|/__  \\ \\  \\|__|//  / /\\ \\______  \\|____|\\  \\ \\  \\\\\\  \\       \n"
    "  \\ \\  \\_|\\ \\ \\  \\_|\\ \\ \\  \\_|\\ \\  \\ \\  \\   /  /_/__\\|_____|\\  \\  __\\_\\  \\ \\  \\\\\\  \\      \n"
    "   \\ \\_______\\ \\_______\\ \\_______\\  \\ \\__\\ |\\________\\     \\ \\__\\|\\_______\\ \\_______\\     \n"
    "    \\|_______|\\|_______|\\|_______|   \\|__|  \\|_______|      \\|__|\\|_______|\\|_______|     \n"
    "                                                                                          \n"
    "                                                                                          \n"
    "                                                                                          \n"
    "  ________  ________  ________  ________  ________  ________            _____    _____     \n"
    " |\\   ___ \\|\\   __  \\|\\   __  \\|\\   __  \\|\\   __  \\|\\   ____\\          / __  \\  / __  \\    \n"
    " \\ \\  \\_|\\ \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\___|_        |\\/_|\\  \\|\\/_|\\  \\   \n"
    "  \\ \\  \\ \\\\ \\ \\  \\\\\\  \\ \\  \\\\\\  \\ \\   _  _\\ \\  \\\\\\  \\ \\_____  \\       \\|/ \\ \\  \\|/ \\ \\  \\  \n"
    "   \\ \\  \\_\\\\ \\ \\  \\\\\\  \\ \\  \\\\\\  \\ \\  \\\\  \\\\ \\  \\\\\\  \\|____|\\  \\           \\ \\  \\   \\ \\  \\ \n"
    "    \\ \\_______\\ \\_______\\ \\_______\\ \\__\\\\ _\\\\ \\_______\\____\\_\\  \\           \\ \\__\\   \\ \\__\\\n"
    "     \\|_______|\\|_______|\\|_______|\\|__|\\|__|\\|_______|\\_________\\           \\|__|    \\|__|\n"
    "                                                      \\|_________|                         \n"
    "                                                                                          \n"
    " Developed by <Nguyen Ngoc Luong> - <S3927460>\n"
    " DoorOS 11 - 2024 LuongCorp.LLC All rights reserved.\n\n"); 


    printf("\033[1;37m", "\x1b[40m");
    printf("DoorOS> ");

    while (1) {
        cli();
    }

}

// Function to handle the command history navigation
void navigateCommandHistory(char *cli_buffer, int *index, int direction) {
    if (direction == 1 && CMD_TRACKER_INDEX < CMD_TRACKER_SIZE) {
        if (CMD_TRACKER_INDEX <= LAST_STATE_TRACKER_INDEX) {
            CMD_TRACKER_INDEX++;
        }
        if (CMD_TRACKER_INDEX == 20) {
            cli_buffer[0] = '\0'; // Clear the buffer if at the newest command
            uart_puts("\b \b");
        } else {
            strncpy(cli_buffer, CMD_TRACKER[CMD_TRACKER_INDEX], MAX_CMD_SIZE - 1);
            cli_buffer[MAX_CMD_SIZE - 1] = '\0';
        }
    } else if (direction == -1 && CMD_TRACKER_INDEX > 0) {
        if(accessHistory == 1){
            CMD_TRACKER_INDEX--;
        }
        else {
            accessHistory = 1;
            LAST_STATE_TRACKER_INDEX = CMD_TRACKER_INDEX;
        }
        
        strncpy(cli_buffer, CMD_TRACKER[CMD_TRACKER_INDEX], MAX_CMD_SIZE - 1);
        cli_buffer[MAX_CMD_SIZE - 1] = '\0';
    }
    *index = strlen(cli_buffer);
}

// Command Line Interpreter
void cli() {
    static char cli_buffer[MAX_CMD_SIZE];
    static int index = 0;
    static int reset = 1;

    char c = uart_getc();


    if ((c != '\0' && (c != '\b' && c != 0x7F && c != 0x08)) && (c != '+' && c != '_')) {
        uart_sendc(c);
        
    }
    if (c != '=' && c != '-' && accessHistory == 1) {
        accessHistory = 0;
        CMD_TRACKER_INDEX = LAST_STATE_TRACKER_INDEX;
    }

    if (c == '\n' || index >= MAX_CMD_SIZE - 1) {
        if (c == '\n') {
            cli_buffer[index] = '\0';
        } else {
            cli_buffer[MAX_CMD_SIZE - 1] = '\0'; // Ensure null-terminated
        }

        if (c == '\n' || index >= MAX_CMD_SIZE - 1) {
            processCommand(cli_buffer);

            // Add the command to command history
            if (CMD_TRACKER_INDEX >= CMD_TRACKER_SIZE) {
                // Shift the history to make space for the new command
                for (int i = 0; i < CMD_TRACKER_SIZE - 1; i++) {
                    strncpy(CMD_TRACKER[i], CMD_TRACKER[i + 1], MAX_CMD_SIZE - 1);
                    CMD_TRACKER[i][MAX_CMD_SIZE - 1] = '\0';
                }
                CMD_TRACKER_INDEX = CMD_TRACKER_SIZE - 1;
            }
            strncpy(CMD_TRACKER[CMD_TRACKER_INDEX], cli_buffer, MAX_CMD_SIZE - 1);
            CMD_TRACKER[CMD_TRACKER_INDEX][MAX_CMD_SIZE - 1] = '\0';
            CMD_TRACKER_INDEX++;

            reset = 1;
            index = 0;
        }

        if (reset) {
            printf("\nDoorOS> ");
            reset = 0;
        }
        accessHistory = 0;
        LAST_STATE_TRACKER_INDEX = CMD_TRACKER_INDEX;
    } else if (c == '\t') {
        autoComplete(cli_buffer, &index);
    } else if (c == 0x7F || c == 0x08 || c == '\b') {
        if (index > 0) {
            uart_sendc(0x08);
            uart_sendc(' ');
            uart_sendc(0x08);
            index--;
            cli_buffer[index] = '\0';
        }
    } else if (c == '_') { // Move down in history using shift and -
        for(int i = 0; i < strlen(cli_buffer)+1; i++){
            uart_sendc(0x08);
            uart_sendc(' ');
            uart_sendc(0x08);
        }
        navigateCommandHistory(cli_buffer, &index, -1);
        
        printf("\rDoorOS> %s", cli_buffer);
    } else if (c == '+') { // Move up in history
        for(int i = 0; i < strlen(cli_buffer)+1; i++){
            uart_sendc(0x08);
            uart_sendc(' ');
            uart_sendc(0x08);
        }
        navigateCommandHistory(cli_buffer, &index, 1);
        printf("\rDoorOS> %s", cli_buffer);
    } else {
        cli_buffer[index] = c;
        index++;
        reset = 1; // A new character indicates a new command
    }
}
