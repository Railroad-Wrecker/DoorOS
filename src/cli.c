#include "cli.h"

#define MAX_CMD_SIZE 100

// Commands syntax & description
const char *commands[] = {"help", "clear", "setcolor", "showinfo", "home"};

const char *commandDescriptions[] = {
    "The help command provides assistance in navigating the MyBareOS CLI environment. Use 'help' for a list of available commands.",
    "Refreshes the terminal by clearing clutter. The screen will scroll down, leaving a fresh display.",
    "Adjusts text and background colors in the terminal. Use '-t' for text color and '-b' for background. Example: setcolor -b yellow -t white. Available colors: black, red, green, yellow, blue, purple, cyan, white.",
    "Displays board revision and MAC address.",
    "Return to home."
};

// Simple isspace implementation
static inline int isspace(int c) {
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}


// Process a command from user input
void processCommand(const char *cmd) {
    int commandIndex = -1;
    char *saveptr; // For strtok_r

    // Declare variables here
    const char *options;
    char *token = NULL;
    const char *textColor = NULL; // Changed to const char
    const char *backgroundColor = NULL; // Changed to const char

    // Check if the command is empty or contains only whitespaces
    const char *p = cmd;
    while (*p != '\0') {
        if (!isspace((unsigned char)*p)) {
            break;
        }
        p++;
    }
    
    // If the command is empty or whitespace only, just return to prompt
    if (*p == '\0') {
        return;
    }

    // Check if the command starts with "help "
    if (strncmp(cmd, "help ", 5) == 0) {
        // Extract the command name (e.g., "help showinfo" -> "showinfo")
        const char *commandName = cmd + 5;
        printCommandHelp(commandName);
        return;
    }

    // Determine if the command matches a predefined command
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if (strncmp(cmd, commands[i], strlen(commands[i])) == 0) {
            commandIndex = i;
            break;
        }
    }

    switch (commandIndex) {
        case 0:
            help();
            break;
        case 1:
            clear();
            break;
        case 2:
            // Check if the command is "setcolor"
            if (strncmp(cmd, "setcolor", 7) == 0) {
                // Extract the options part of the command (e.g., "-b yellow -t white")
                options = cmd + 8; // Skip "setcolor "

                // Tokenize the options based on spaces using strtok_r
                token = strtok_r((char *)options, " ", &saveptr);

                while (token != NULL) {
                    if (strncmp(token, "-b", 2) == 0) {
                        // Next token is the background color
                        token = strtok_r((char *)options + 4, " ", &saveptr);
                        if (token != NULL) {
                            // Map input color name to ANSI escape code
                            backgroundColor = mapColorToCodeBackground(token);
                        }
                    } else if (strncmp(token, "-t", 2) == 0) {
                        // Next token is the text color
                        token = strtok_r((char *)options + 4, " ", &saveptr);
                        if (token != NULL) {
                            // Map input color name to ANSI escape code
                            textColor = mapColorToCodeText(token);
                        }
                    }
                    
                    options += 4 + strlen(token); // Skip "-b/-t <color> "
                    token = strtok_r((char *)options + 1, " ", &saveptr);
                }

                setColor(textColor, backgroundColor);
            }
            break;
        case 3:
            showInfo();
            break;
        case 4:
            home();
            break;
        default:
            printf(
                   "+------------------------------------------+\n"
                   "| Command List                     - [ ] x |\n"
                   "+==========================================+\n"
                   "| Invalid command. Please try again!       |\n"
                   "+------------------------------------------+\n");
            break;
    }
}

// Function to display command list
void help(){
    printf(
    "+-------------------------------------------------------------+\n"
    "| Command List                                        - [ ] x |\n"
    "+=============================================================+\n"
    "| Use 'help <command>' to display detailed instructions for a |\n"
    "| specific command.                                           |\n"
    "|                                                             |\n"
    "| Commands:                                                   |\n"
    "|   help      - Display a list of available commands.         |\n"
    "|   home      - Return to the home screen.                    |\n"
    "|   clear     - Clear the terminal screen.                    |\n"
    "|   setcolor  - Adjust text and background colors in the      |\n"
    "|               terminal.                                     |\n"
    "|   showinfo  - Display board revision and MAC address.       |\n"
    "+-------------------------------------------------------------+\n");
}

void home() {

    printf(
    "+----------------------------------------------------------------------------------------------+\n"
    "|                                                                                              |\n"
    "|                                                                                              |\n"
    "|  Welcome to                                                                                  |\n"
    "|  ________  ________  ________  ________  ________  ________            _____    _____        |\n"                      
    "| |\\   ___ \\|\\   __  \\|\\   __  \\|\\   __  \\|\\   __  \\|\\   ____\\          / __  \\  / __  \\       |\n"
    "| \\ \\  \\_|\\ \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\___|_        |\\/_|\\  \\|\\/_|\\  \\      |\n"
    "|  \\ \\  \\ \\\\ \\ \\  \\\\\\  \\ \\  \\\\\\  \\ \\   _  _\\ \\  \\\\\\  \\ \\_____  \\       \\|/ \\ \\  \\|/ \\ \\  \\     |\n"
    "|   \\ \\  \\_\\\\ \\ \\  \\\\\\  \\ \\  \\\\\\  \\ \\  \\\\  \\\\ \\  \\\\\\  \\|____|\\  \\           \\ \\  \\   \\ \\  \\    |\n"
    "|    \\ \\_______\\ \\_______\\ \\_______\\ \\__\\\\ _\\\\ \\_______\\____\\_\\  \\           \\ \\__\\   \\ \\__\\   |\n"
    "|     \\|_______|\\|_______|\\|_______|\\|__|\\|__|\\|_______|\\_________\\           \\|__|    \\|__|   |\n"
    "|                                                      \\|_________|                            |\n"
    "|                                                                                              |\n"
    "|                                                                                              |\n"
    "|                                                                                              |\n"
    "|                                                                                              |\n"
    "|                                                                                              |\n"
    "|                                                                                              |\n"
    "|                                                             Activate Doors                   |\n"
    "|                                                             Go to setting to activate Doors  |\n"
    "|                                                                                              |\n"
    "+----------------------------------------------------------------------------------------------+\n"
    "Type 'help' for a list of available commands.                                  \n");

}

// Function to print help for a specific command
void printCommandHelp(const char *cmd) {
    // Store the original value of commands[2]
    const char *originalSetcolor = commands[2];

    // Temporarily remove the trailing space
    commands[2] = "setcolor";

    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if (strncmp(cmd, commands[i], strlen(commands[i])) == 0) {
            printf("%s\n", commandDescriptions[i]);

            // Restore the original value of commands[2]
            commands[2] = originalSetcolor;

            return;
        }
    }

    printf(
    "+-------------------------------+\n"
    "| Command List          - [ ] x |\n"
    "+===============================+\n"
    "| Error: Command does not exist |\n"
    "+-------------------------------+\n"
);

}

// Function to clear terminal
void clear(){
    printf("\033[2J\033[1;1H");
}

// Function to set text and background colors
void setColor(const char *textColor, const char *backgroundColor) {
    if (textColor || backgroundColor) {
        // Check if valid text color is provided
        if (textColor) {
            printf("%s", textColor);
        }

        // Check if valid background color is provided
        if (backgroundColor) {
            printf("%s", backgroundColor);
        }

        return;
    }
    
    printf(
    "+----------------------------------------------------------------+\n"
    "| Command List                                           - [ ] x |\n"
    "+================================================================+\n"
    "| Please recheck your command or if the color is supported.      |\n"
    "+----------------------------------------------------------------+\n"
);

}

// Displays board revision
void showInfo()
{
    printf(
    "+----------------------------------------------------------------+\n"
    "| Board revision and MAC address                         - [ ] x |\n"
    "+================================================================+\n"
    "\n");
    unsigned int *response = 0;

    // Board MAC address
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_MACADDR, &response);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    unsigned int address[6];
    address[0] = (response[1] >> 8) & 0xFF; // extract the most significant byte (1st byte)
    address[1] = (response[1]) & 0xFF; // 2nd byte
    address[2] = (response[0] >> 24) & 0xFF; // 3rd byte
    address[3] = (response[0] >> 16) & 0xFF; // 4th byte
    address[4] = (response[0] >> 8) & 0xFF; // 5th byte (1st byte of the 2nd half of the MAC address)
    address[5] = (response[0]) & 0xFF; // 6th byte (2nd byte of the 2nd half of the MAC address)
    printf("  Board MAC address: %x:%x:%x:%x:%x:%x\n\n", address[0], address[1], address[2], address[3], address[4], address[5]);

    // Board revision
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_GETBOARDREVISION, &response);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("  Board revision: %x\n\n", response[0]);

    // ARM clock rate
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_GETCLKRATE, &response, 3);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("  ARM clock rate: %dHz\n\n", response[0]);

    // VC memory
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_VCMEMORY, &response);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("  VC memory: %dB\n\n", response[0]);

    // Firmware revision
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_GETFIRMWAREREVISION, &response);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("  Firmware revision: %dB\n\n", response[0]);
}


// Function to handle command auto-completion and display suggestions
void autoComplete(char *buffer, int *index) {
    static int lastMatchIndex = -1;
    int commandCount = sizeof(commands) / sizeof(commands[0]);
    int multipleMatches = 0;

    // Find the last space or beginning of the buffer
    int i;
    for (i = *index - 1; i >= 0 && buffer[i] != ' '; i--);

    // Extract the word that needs completion
    char word[MAX_CMD_SIZE];
    strncpy(word, buffer + i + 1, *index - i - 1);
    word[*index - i - 1] = '\0';

    // Attempt to find the next matching command
    int start = (lastMatchIndex + 1) % commandCount;
    int found = 0, firstMatchIndex = -1;
    for (int j = start; ; j = (j + 1) % commandCount) {
        if (strstr(commands[j], word) == commands[j]) {
            if (!found) {
                firstMatchIndex = j;
            }
            found++;
            if (found > 1) {
                multipleMatches = 1;
                break;
            }
        }
        if (j == commandCount - 1) j = -1; // wrap around the command list
        if (j == start - 1) break; // completed a full loop
    }

    // If multiple matches, display all and complete the first found
    if (multipleMatches) {
        printf("\nPossible commands:\n");
        for (int j = 0; j < commandCount; j++) {
            if (strstr(commands[j], word) == commands[j]) {
                printf("- %s\n", commands[j]);
            }
        }
        strncpy(buffer + i + 1, commands[firstMatchIndex], MAX_CMD_SIZE - (i + 2));
        buffer[i + 1 + strlen(commands[firstMatchIndex])] = '\0';
        *index = i + 1 + strlen(commands[firstMatchIndex]);
        lastMatchIndex = firstMatchIndex;
    } else if (found == 1) {
        strncpy(buffer + i + 1, commands[firstMatchIndex], MAX_CMD_SIZE - (i + 2));
        buffer[i + 1 + strlen(commands[firstMatchIndex])] = '\0';
        *index = i + 1 + strlen(commands[firstMatchIndex]);
        lastMatchIndex = firstMatchIndex;
    } else {
        lastMatchIndex = -1;
    }

    printf("\rDoorOS> %s", buffer);
}