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
    // Initialize UART
    uart_init();

    // Print welcome message
    home();
    printf("DoorOS> ");

    // Command Line Interpreter
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

    // Ignore non-printable characters
    if ((c != '\0' && (c != '\b' && c != 0x7F && c != 0x08)) && (c != '+' && c != '_')) {
        uart_sendc(c);
        
    }
    
    if (c != '=' && c != '-' && accessHistory == 1) { // Reset the history access tracker
        accessHistory = 0;
        CMD_TRACKER_INDEX = LAST_STATE_TRACKER_INDEX;
    }

    // Process the command when Enter is pressed
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
    } else if (c == '\t') { // Tab completion
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
