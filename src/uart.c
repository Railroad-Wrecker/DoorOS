#include "uart.h"

// Define constants for UART configuration
#define UART_CLOCK 48000000 // Default UART clock frequency

/**
 * Configure UART with specified baud rate, data bits, and stop bits.
 * 
 * @param baud_rate The baud rate (e.g., 9600, 115200).
 * @param data_bits The number of data bits (7 or 8).
 * @param stop_bits The number of stop bits (1 or 2).
 */
void uart_configure(unsigned int baud_rate, unsigned int data_bits, unsigned int stop_bits)
{
    unsigned int r;

    /* Turn off UART0 */
    UART0_CR = 0x0;

    // Configure GPIO pins 14 and 15
    r = GPFSEL1;
    r &= ~((7 << 12) | (7 << 15));
    r |= (0b100 << 12)|(0b100 << 15);
    GPFSEL1 = r;

    // Enable GPIO 14, 15
    #ifdef RPI3
    GPPUD = 0;
    r = 150; while(r--) { asm volatile("nop"); }
    GPPUDCLK0 = (1 << 14)|(1 << 15);
    r = 150; while(r--) { asm volatile("nop"); }
    GPPUDCLK0 = 0;
    #else // RPI4
    r = GPIO_PUP_PDN_CNTRL_REG0;
    r &= ~((3 << 28) | (3 << 30));
    GPIO_PUP_PDN_CNTRL_REG0 = r;
    #endif

    // Mask all interrupts
    UART0_IMSC = 0;

    // Clear pending interrupts
    UART0_ICR = 0x7FF;

    // Calculate UART divider based on the specified baud rate
    unsigned int divider = UART_CLOCK / (16 * baud_rate);
    unsigned int fractional_part = (UART_CLOCK % (16 * baud_rate) * 64 + baud_rate / 2) / baud_rate;

    // Set integer & fractional part of Baud rate
    UART0_IBRD = divider;
    UART0_FBRD = fractional_part;

    // Configure the Line Control Register
    unsigned int lcrh = UART0_LCRH_FEN; // Enable FIFO
    if (data_bits == 8) {
        lcrh |= UART0_LCRH_WLEN_8BIT;
    } else {
        lcrh |= UART0_LCRH_WLEN_7BIT;
    }

    if (stop_bits == 2) {
        lcrh |= UART0_LCRH_STP2;
    }

    UART0_LCRH = lcrh;

    // Enable UART0, receive, and transmit
    UART0_CR = 0x301; // enable Tx, Rx, FIFO
}

/**
 * Initialize UART to default settings (115200 baud, 8 data bits, 1 stop bit).
 */
void uart_init()
{
    uart_configure(115200, 8, 1);
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
