#include "uart.h"

// Global configuration variables (with defaults)
static unsigned int current_baud_rate = 115200;
static unsigned int current_data_bits = 8;
static unsigned int current_stop_bits = 1;
static char current_parity = 'N'; // N (none), E (even), O (odd)
static int current_handshaking = 0; // 0 for false, 1 for true

/**
 * Initialize UART with configurable settings
 */
void uart_init() {
    unsigned int r;

    /* Turn off UART0 */
    UART0_CR = 0x0;

    // Setup GPIO pins 14 and 15 (Assuming Raspberry Pi setup as in previous function)

    /* Mask all interrupts */
    UART0_IMSC = 0;

    /* Clear pending interrupts */
    UART0_ICR = 0x7FF;

    /* Calculate baud rate divisors */
    unsigned int ibrd = 3000000 / (16 * current_baud_rate); // Example with a fixed UART clock
    unsigned int fbrd = ((3000000 % (16 * current_baud_rate)) * 64 + current_baud_rate / 2) / current_baud_rate;

    UART0_IBRD = ibrd;
    UART0_FBRD = fbrd;

    /* Configure Line Control Register */
    unsigned int lcrh = UART0_LCRH_FEN; // Enable FIFO
    switch (current_data_bits) {
        case 5: lcrh |= UART0_LCRH_WLEN_5BIT; break;
        case 6: lcrh |= UART0_LCRH_WLEN_6BIT; break;
        case 7: lcrh |= UART0_LCRH_WLEN_7BIT; break;
        case 8: lcrh |= UART0_LCRH_WLEN_8BIT; break;
    }
    if (current_stop_bits == 2) {
        lcrh |= UART0_LCRH_STP2;
    }
    if (current_parity == 'E') {
        lcrh |= (UART0_LCRH_PEN | UART0_LCRH_EPS);
    } else if (current_parity == 'O') {
        lcrh |= UART0_LCRH_PEN;
    }
    UART0_LCRH = lcrh;

    /* Enable UART0 with or without handshaking */
    r = UART0_CR_UARTEN | UART0_CR_TXE | UART0_CR_RXE;
    if (current_handshaking) {
        r |= UART0_CR_RTS;
    }
    UART0_CR = r;
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
