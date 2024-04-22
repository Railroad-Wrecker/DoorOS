#include "uart0.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    /* initialize UART0 */
    UART0_CR = 0;            // Turn off UART0

    /* Set up the GPIO pin 14 && 15 */
    // Use GPFSEL1 to select alternate function 0 for pins 14, 15 which correspond to TXD0 and RXD0
    unsigned int r = GPFSEL1;
    r &= ~((7 << 12) | (7 << 15));  // Clear existing settings for pins
    r |= (4 << 12) | (4 << 15);     // Set pins to Alt0
    GPFSEL1 = r;

    // Disable pull-up/down for all GPIO pins & delay for 150 cycles
    GPPUD = 0;
    r = 150; while(r--) { asm volatile("nop"); }

    // Disable pull-up/down for pin 14,15 & delay for 150 cycles
    GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150; while(r--) { asm volatile("nop"); }
    GPPUDCLK0 = 0;  // flush GPIO setup

    /* Set the UART0 baud rate */
    // UART_CLOCK = 48MHz, Baud rate = 115200
    UART0_IBRD = 26;  // 48MHz / (16 * 115200) = 26.0416666
    UART0_FBRD = 3;   // Fractional part = .0416666 * 64 + 0.5 = 3.5 (approximately)

    /* Set the line control */
    UART0_LCRH = (3 << 5);  // 8 bits, no parity, one stop bit

    /* Mask all interrupts */
    UART0_IMSC = (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10);

    /* Enable UART0, receive & transmit */
    UART0_CR = (1 << 0) | (1 << 8) | (1 << 9);
}

/**
 * Send a character
 */
void uart_sendc(char c) {
    // Wait until the UART transmitter is empty
    while (UART0_FR & (1 << 5)) { asm volatile("nop"); }

    // Write the character to the UART data register
    UART0_DR = c;
}

/**
 * Receive a character
 */
char uart_getc() {
    char c;

    // Wait until a character is ready to be read
    while (UART0_FR & (1 << 4)) { asm volatile("nop"); }

    // Read the character from UART data register
    c = (unsigned char)(UART0_DR);

    // Convert carriage return to newline character (optional, depending on your application)
    return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while (*s) {
        if (*s == '\n') uart_sendc('\r'); // Optional: convert newline to carriage return + newline
        uart_sendc(*s++);
    }
}

/**
 * Display a value in hexadecimal format
 * Note: No changes required for this function
 */
void uart_hex(unsigned int num) {
    uart_puts("0x");
    for (int pos = 28; pos >= 0; pos -= 4) {
        char digit = (num >> pos) & 0xF;
        digit += (digit > 9) ? (-10 + 'A') : '0';
        uart_sendc(digit);
    }
}

/**
 * Display a value in decimal format
 * Note: No changes required for this function
 */
void uart_dec(int num) {
    char str[33] = "";
    int len = 1, temp = num;
    while (temp >= 10) {
        len++;
        temp /= 10;
    }
    for (int i = 0; i < len; i++) {
        int digit = num % 10;
        num /= 10;
        str[len - (i + 1)] = digit + '0';
    }
    str[len] = '\0';
    uart_puts(str);
}
