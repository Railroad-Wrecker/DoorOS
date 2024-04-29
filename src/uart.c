#include "uart.h"

// Define constants for UART configuration
#define UART_CLOCK 48000000 // Default UART clock frequency

// Define default UART settings
unsigned int baud_rate = 115200; // Default baud rate
unsigned int data_bits = 8; // Default data bits
char parity = 'N'; // Default parity
unsigned int stop_bits = 1; // Default stop bits
char rts_cts = 'N'; // Default RTS/CTS flow control

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
    uart_set_baud_rate(baud_rate);

    // Set default line control settings in one operation
    unsigned int lcrh = UART0_LCRH_FEN; // Start with enabling FIFO
    lcrh |= set_data_bits(data_bits);   // Add data bits configuration
    lcrh |= set_parity(parity);         // Add parity configuration
    lcrh |= set_stop_bits(stop_bits);   // Add stop bits configuration
    UART0_LCRH = lcrh;                  // Set the line control register

    set_rts_cts(rts_cts);               // Configure RTS/CTS flow control separately

    // Enable UART0, receive, and transmit
    UART0_CR = 0x301; // Enable Tx, Rx, FIFO
}

/**
 * Set UART baud rate
 * @param baud_rate Baud rate
 */
void uart_set_baud_rate(unsigned int baud_rate)
{
    // Wait for the end of transmission or reception to avoid corruption
    while(UART0_FR & UART_FR_BUSY) {
        asm volatile("nop");
    }

    // Disable the UART before making changes
    UART0_CR = 0x0;

    // If FIFOs are enabled, flush them here
    if (UART0_LCRH & UART0_LCRH_FEN) {
        UART0_LCRH &= ~UART0_LCRH_FEN; // Clear FIFO enable bit
        // Flush the FIFOs by clearing the FIFO enable bit in the line control register
    }

    unsigned int ibrd, fbrd, divider;
    divider = UART_CLOCK / (16 * baud_rate);
    ibrd = (unsigned int)(divider);                 // Integer part of divider
    fbrd = (unsigned int)((divider - ibrd) * 64 + 0.5); // Fractional part of divider, rounded

    // Set integer and fractional parts of baud rate
    UART0_IBRD = ibrd;
    UART0_FBRD = fbrd;

    // Re-enable the UART with the new baud rate
    UART0_CR = 0x301; // Enable Tx, Rx, FIFO
}

// Function to set the number of data bits
unsigned int set_data_bits(unsigned int data_bits) {
    switch (data_bits) {
        case 5: return UART0_LCRH_WLEN_5BIT; // 5-bit data
        case 6: return UART0_LCRH_WLEN_6BIT; // 6-bit data
        case 7: return UART0_LCRH_WLEN_7BIT; // 7-bit data
        default: return UART0_LCRH_WLEN_8BIT; // 8-bit data
    }
}

// Function to set the parity
unsigned int set_parity(char parity) {
    switch (parity) {
        case 'E': return UART0_LCRH_EPS | UART0_LCRH_PEN; // Even parity
        case 'O': return UART0_LCRH_PEN; // Odd parity
        default: return 0; // No parity
    }
}

// Function to set the number of stop bits
unsigned int set_stop_bits(unsigned int stop_bits) {
    return (stop_bits == 2) ? UART0_LCRH_STP2 : 0; // 2 stop bits, 0 otherwise
}

// Function to return the bit mask for RTS/CTS flow control
unsigned int set_rts_cts(char rts_cts) {
    if (rts_cts == 'E') {
        return UART0_CR_RTSEN | UART0_CR_CTSEN; // Bit mask to enable RTS/CTS flow control
    } else {
        return 0; // No bits set, flow control is disabled
    }
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
