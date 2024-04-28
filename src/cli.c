#include "cli.h"
#include "uart.h"

#define MAX_CMD_SIZE 100
#define UART_CLOCK 48000000 // Default UART clock frequency



// Updated command array
const char *commands[] = {"help", "clear", "setcolor", "showinfo", "home", "setbaud", 
                        "setdatabits", "setstopbits", "setparity", "setflowcontrol",
                        "currentuartsettings"};

// Updated command descriptions array
const char *commandDescriptions[] = {
    "Provides assistance in navigating the DoorOS CLI environment.",
    "Refreshes the terminal by clearing clutter.",
    "Adjusts text and background colors. Example Usage: setcolor -b yellow -t white.",
    "Displays board revision and MAC address.",
    "Return to home.",
    "Sets the UART baud rate. Example: setbaud 115200",
    "Sets the UART data bits (5, 6, 7 or 8). Example: setdatabits 8",
    "Sets the UART stop bits (1 or 2). Example: setstopbits 1",
    "Sets the UART parity (N for None, E for Even, O for Odd). Example: setparity N",
    "Sets the UART hardware handshake (N for None, E for Enable). Example: setflowcontrol N",
};

// Simple isspace implementation
static inline int isspace(int c) {
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

// Function to convert ASCII string to integer (since we cannot use the standard library's atoi)
static int simple_atoi(const char *str) {
    int res = 0;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res;
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
            help(); break;
        case 1:
            clear(); break;
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
            showInfo(); break;
        case 4:
            home();break;
        case 5:
            // Set Baud Rate
            if (strncmp(cmd, "setbaud ", 8) == 0) {
                // Turn off uart before changing baud rate
                UART0_CR = 0x0;
                int baud_rate = simple_atoi(cmd + 8);
                unsigned int ibrd, fbrd, divider;
                divider = UART_CLOCK / (16 * baud_rate);
                ibrd = divider;                       // Integer part of divider
                fbrd = (divider - ibrd) * 64 + 0.5;   // Fractional part of divider

                UART0_IBRD = ibrd;
                UART0_FBRD = fbrd;
                // Restart UART
                UART0_CR = 0x301;
                printf("Baud rate set to %d\n", baud_rate);
            } 
            break;
        case 6:
            // Set Data Bits
            if (strncmp(cmd, "setdatabits ", 12) == 0) {
                // Turn off UART0 before changing data bits
                UART0_CR = 0x0;

                // Parse the number of data bits from the command
                int data_bits = simple_atoi(cmd + 12);
                if (data_bits < 5 || data_bits > 8) {
                    printf("Invalid data bits. Must be between 5 and 8.\n");
                    return; // Add error handling for invalid data bits
                }

                // Read current LCRH, clear the data bits field, and set new data bits
                unsigned int lcrh = UART0_LCRH & ~UART0_LCRH_WLEN_MASK;
                lcrh |= set_data_bits(data_bits); // Set the new data bits

                // Write the updated value back to the LCRH register
                UART0_LCRH = lcrh;

                // Re-enable UART0 after configuration
                UART0_CR = 0x301;

                UART0_LCRH = lcrh;

                // Print confirmation message
                printf("Data bits set to %d\n", data_bits);
            }
            break;
        case 7:
            // Set Stop Bits
            if (strncmp(cmd, "setstopbits ", 12) == 0) {
                // Turn off UART0 before changing stop bits
                UART0_CR = 0x0;

                // Parse the number of stop bits from the command
                int stop_bits = simple_atoi(cmd + 12);
                if (stop_bits != 1 && stop_bits != 2) {
                    printf("Invalid stop bits. Must be 1 or 2.\n");
                    return; // Add error handling for invalid stop bits
                }

                // Read current LCRH, clear the stop bits field, and set new stop bits
                unsigned int lcrh = UART0_LCRH & ~UART0_LCRH_STP2; // Clear the stop bits
                lcrh |= set_stop_bits(stop_bits); // Set the new stop bits

                
                // Write the updated value back to the LCRH register
                UART0_LCRH = lcrh;

                // Re-enable UART0 after configuration
                UART0_CR = 0x301;

                UART0_LCRH = lcrh;

                // Print confirmation message
                printf("Stop bits set to %d\n", stop_bits);
            }
            break;
        case 8:
            // Set Parity
            if (strncmp(cmd, "setparity ", 10) == 0) {
                // Turn off UART0 before changing parity
                UART0_CR = 0x0;

                // Parse the parity setting from the command
                char parity = cmd[10]; // Assuming this gets you the correct character for parity

                // Read current LCRH, clear the parity bits, then set new parity
                unsigned int lcrh = UART0_LCRH & ~(UART0_LCRH_EPS | UART0_LCRH_PEN); // Clear parity bits
                lcrh |= set_parity(parity); // Apply new parity settings

                // Write the updated value back to the LCRH register
                UART0_LCRH = lcrh;

                // Re-enable UART0 after configuration
                UART0_CR = 0x301;

                UART0_LCRH = lcrh;

                // Print confirmation message based on the parity setting
                printf("Parity set to %c\n", parity);
            }
            break;
        case 9:
            // Set Hardware Handshake (CTS/RTS)
            if (strncmp(cmd, "setflowcontrol ", 15) == 0) {
                // Turn off UART0 before changing flow control
                UART0_CR = 0x0;

                // Parse the flow control setting from the command
                char flow_control = cmd[15]; // Assuming this gets you the correct character for flow control

                // Clear the RTS/CTS bits, then set new flow control
                UART0_CR &= ~(UART0_CR_RTSEN | UART0_CR_CTSEN); // Clear flow control bits
                UART0_CR |= set_rts_cts(flow_control); // Apply new flow control settings

                // Re-enable UART0 after configuration
                UART0_CR |= 0x301; // Reapply the enable bits

                // Print confirmation message based on the flow control setting
                printf("Flow control set to %c\n", flow_control);
            }
            break;
        case 10:
            // Display current UART settings          
            if (strncmp(cmd, "currentuartsettings", 19) == 0) {
                
                printf("Current UART Settings:\n");

                // Display baud rate
                unsigned int ibrd = UART0_IBRD;
                unsigned int fbrd = UART0_FBRD;
                unsigned int baud_rate = UART_CLOCK / (16 * (ibrd + (fbrd / 64.0)));
                printf("Baud rate: %d\n", baud_rate);

                // Display FIFO status
                printf("FIFO: %s\n", (UART0_LCRH & UART0_LCRH_FEN) ? "Enabled" : "Disabled");

                // Display data bits
                printf("Data bits: ");
                switch (UART0_LCRH & 0x60) {
                    case UART0_LCRH_WLEN_5BIT: printf("5\n"); break;
                    case UART0_LCRH_WLEN_6BIT: printf("6\n"); break;
                    case UART0_LCRH_WLEN_7BIT: printf("7\n"); break;
                    case UART0_LCRH_WLEN_8BIT: printf("8\n"); break;
                    default: printf("Unknown\n"); break;
                }

                // Display parity
                if (UART0_LCRH & UART0_LCRH_PEN) {
                    if (UART0_LCRH & UART0_LCRH_EPS) {
                        printf("Parity: Even\n");
                    } else {
                        printf("Parity: Odd\n");
                    }
                } else {
                    printf("Parity: None\n");
                }

                // Display stop bits
                printf("Stop bits: %s\n", (UART0_LCRH & UART0_LCRH_STP2) ? "2" : "1");

                // Display RTS/CTS flow control
                printf("RTS/CTS flow control: %s\n", (UART0_CR & UART0_CR_RTSEN) ? "Enabled" : "Disabled");

            }
            break;
        default:
            printf(
                   "\n"
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
    "\n"
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
    "\n"
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
    "\n"
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
    "\n"
    "+----------------------------------------------------------------+\n"
    "| Command List                                           - [ ] x |\n"
    "+================================================================+\n"
    "| Please recheck your command or if the color is supported.      |\n"
    "+----------------------------------------------------------------+\n");
}

// Displays board revision
void showInfo()
{
    printf(
    "\n"
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