#include "uart.h"

// Define constants for UART configuration
#define UART_CLOCK 48000000 // Default UART clock frequency

/**
 * Initialize UART with default settings and map to GPIO
 */
void uart_init()
{
    unsigned int r;

    /* Turn off UART0 */
    UART0_CR = 0x0;

    /* Setup GPIO pins 14 and 15 */
    /* Set GPIO14 and GPIO15 to be PL011 TX/RX which is ALT0 */
    r = GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // Clear bits 17-12 (FSEL15, FSEL14)
    r |= (0b100 << 12) | (0b100 << 15); // Set value 0b100 (select ALT0: TXD0/RXD0)
    GPFSEL1 = r;

    /* Enable GPIO 14, 15 */
#ifdef RPI3
    GPPUD = 0; // No pull up/down control
    // Toggle clock to flush GPIO setup
    r = 150; while (r--) { asm volatile("nop"); } // Waiting 150 cycles
    GPPUDCLK0 = (1 << 14) | (1 << 15); // Enable clock for GPIO 14, 15
    r = 150; while (r--) { asm volatile("nop"); } // Waiting 150 cycles
    GPPUDCLK0 = 0; // Flush GPIO setup
#else // RPI4
    r = GPIO_PUP_PDN_CNTRL_REG0;
    r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
    GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

    /* Mask all interrupts. */
    UART0_IMSC = 0;

    /* Clear pending interrupts. */
    UART0_ICR = 0x7FF;

    /* Set default baud rate (115200) */
    uart_set_baud_rate(115200);

    /* Set default line control settings (8 data bits, no parity, 1 stop bit, no flow control) */
    uart_set_line_control(8, 'N', 1, 'E');

    /* Enable UART0, receive, and transmit */
    UART0_CR = 0x301; // Enable Tx, Rx, FIFO
}

/**
 * Set UART baud rate
 * @param baud_rate Baud rate
 */
void uart_set_baud_rate(unsigned int baud_rate)
{
    unsigned int ibrd, fbrd, divider;
    divider = UART_CLOCK / (16 * baud_rate);
    ibrd = divider;                       // Integer part of divider
    fbrd = (divider - ibrd) * 64 + 0.5;   // Fractional part of divider

    UART0_IBRD = ibrd;
    UART0_FBRD = fbrd;
}

/**
 * Set UART line control settings
 * @param data_bits Number of data bits (5 to 8)
 * @param parity Type of parity ('N' for none, 'E' for even, 'O' for odd)
 * @param stop_bits Number of stop bits (1 or 2)
 * @param rts_cts Enable RTS/CTS flow control (0 for disabled, 1 for enabled)
 */

// Function to set the number of data bits
unsigned int set_data_bits(unsigned int data_bits) {
    switch (data_bits) {
        case 5: return UART0_LCRH_WLEN_5BIT;
        case 6: return UART0_LCRH_WLEN_6BIT;
        case 7: return UART0_LCRH_WLEN_7BIT;
        case 8: return UART0_LCRH_WLEN_8BIT;
        default: return UART0_LCRH_WLEN_8BIT; // Default to 8 bits
    }
}

// Function to set the parity
unsigned int set_parity(char parity) {
    switch (parity) {
        case 'E': return UART0_LCRH_PEN; // Enable parity, even by default
        case 'O': return UART0_LCRH_PEN | UART0_LCRH_EPS; // Enable parity, odd
        default: return 0; // No parity
    }
}

// Function to set the number of stop bits
unsigned int set_stop_bits(unsigned int stop_bits) {
    return (stop_bits == 2) ? UART0_LCRH_STP2 : 0; // 2 stop bits, 0 otherwise
}

// Function to enable or disable RTS/CTS flow control
unsigned int set_rts_cts(char rts_cts) {
    if (rts_cts == 'E') {
        return UART0_CR_CTSEN | UART0_CR_RTSEN; // Enable RTS/CTS
    } else {
        return 0; // Disable RTS/CTS
    }
}

// Main function to set UART line control settings
void uart_set_line_control(unsigned int data_bits, char parity, unsigned int stop_bits, unsigned int rts_cts) {
    // Turn off UART0 before changing settings
    UART0_CR &= ~UART0_CR_UARTEN;
    // printf("UART0 disabled for configuration\n");

    unsigned int lcrh = UART0_LCRH_FEN; // Enable FIFO
    // printf("FIFO enabled\n");

    lcrh |= set_data_bits(data_bits); // Set data bits
    lcrh |= set_parity(parity);       // Set parity
    lcrh |= set_stop_bits(stop_bits); // Set stop bits

    // Set handshake control (RTS/CTS)
    lcrh |= set_rts_cts(rts_cts);

    // Apply line control settings
    UART0_LCRH = lcrh;
    // printf("UART line control settings applied\n");

    // Re-enable UART0 after configuration
    UART0_CR |= UART0_CR_UARTEN;
    // printf("UART0 re-enabled after configuration\n");
}

/**
 * Send a character
 */
void uart_sendc(char c) {

    /* Check Flags Register */
	/* And wait until transmitter is not full */
	do {
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_TXFF);

	/* Write our data byte out to the data register */
	UART0_DR = c ;
}

/**
 * Receive a character
 */
char uart_getc() {
    char c = 0;

    /* Check Flags Register */
    /* Wait until Receiver is not empty
     * (at least one byte data in receive fifo)*/
	do {
		asm volatile("nop");
    } while ( UART0_FR & UART0_FR_RXFE );

    /* read it and return */
    c = (unsigned char) (UART0_DR);

    /* convert carriage return to newline */
    return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while (*s) {
        /* convert newline to carriage return + newline */
        if (*s == '\n')
            uart_sendc('\r');
        uart_sendc(*s++);
    }
}

/**
* Display a value in hexadecimal format
*/
void uart_hex(unsigned int num) {
	uart_puts("0x");
	for (int pos = 28; pos >= 0; pos = pos - 4) {

		// Get highest 4-bit nibble
		char digit = (num >> pos) & 0xF;

		/* Convert to ASCII code */
		// 0-9 => '0'-'9', 10-15 => 'A'-'F'
		digit += (digit > 9) ? (-10 + 'A') : '0';
		uart_sendc(digit);
	}
}

/*
* Display a value in decimal format
*/
void uart_dec(int num)
{
	//A string to store the digit characters
	char str[33] = "";

	//Calculate the number of digits
	int len = 1;
	int temp = num;
	while (temp >= 10){
		len++;
		temp = temp / 10;
	}

	//Store into the string and print out
	for (int i = 0; i < len; i++){
		int digit = num % 10; //get last digit
		num = num / 10; //remove last digit from the number
		str[len - (i + 1)] = digit + '0';
	}
	str[len] = '\0';

	uart_puts(str);
}
