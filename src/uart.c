#include "uart.h"

// Define constants for common baud rates, data bits, and stop bits
#define BAUD_9600   9600
#define BAUD_19200  19200
#define BAUD_38400  38400
#define BAUD_57600  57600
#define BAUD_115200 115200

// Data bits definitions
#define DATA_BITS_5 5
#define DATA_BITS_6 6
#define DATA_BITS_7 7
#define DATA_BITS_8 8

// Stop bits definitions
#define STOP_BITS_1 1
#define STOP_BITS_2 2

/**
 * Make the baud rate configurable.
 */
void uart_init(unsigned int baud_rate, unsigned int data_bits, unsigned int stop_bits) {
    unsigned int r;
    unsigned int lcr_value;
    const unsigned int system_clk_freq = 250000000; // System clock frequency is 250 MHz

    /* Set data bits */
    switch (data_bits) {
        case DATA_BITS_5:
            lcr_value = 2; // 0b10
            break;
        case DATA_BITS_6:
            lcr_value = 3; // 0b11
            break;
        case DATA_BITS_7:
            lcr_value = 0; // 0b00
            break;
        case DATA_BITS_8:
        default:
            lcr_value = 1; // 0b01
            break;
    }

    /* Set stop bits */
    if (stop_bits == STOP_BITS_2) {
        lcr_value |= (1 << 2); // Set bit 2 for 2 stop bits
    }

    /* initialize UART */
    AUX_ENABLE |= 1;     // Enable mini UART (UART1)
    AUX_MU_CNTL = 0;     // Stop transmitter and receiver
    AUX_MU_LCR  = lcr_value; // Set data and stop bits
    AUX_MU_MCR  = 0;     // Clear RTS (not hardware controlled)
    AUX_MU_IER  = 0;     // Disable interrupts
    AUX_MU_IIR  = 0xc6;  // Enable and clear FIFOs
    AUX_MU_BAUD = system_clk_freq / (8 * baud_rate) - 1; // Configure baud rate

    /* map UART1 to GPIO pins 14 and 15 */
    r = GPFSEL1;
    r &= ~( (7 << 12)|(7 << 15) ); // Clear bits for FSEL15 and FSEL14
    r |= (0b010 << 12)|(0b010 << 15); // Set value 0b010 (select ALT5: TXD1/RXD1)
    GPFSEL1 = r;

    /* enable GPIO 14, 15 */
#ifdef RPI3
    GPPUD = 0;             // No pull up/down control
    r = 150; while(r--) { asm volatile("nop"); } // Waiting 150 cycles
    GPPUDCLK0 = (1 << 14)|(1 << 15); // Enable clock for GPIO 14, 15
    r = 150; while(r--) { asm volatile("nop"); } // Waiting 150 cycles
    GPPUDCLK0 = 0;         // Flush GPIO setup
#else
    r = GPIO_PUP_PDN_CNTRL_REG0;
    r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
    GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

    AUX_MU_CNTL = 3;       // Enable transmitter and receiver (Tx, Rx)
}

/**
 * Send a character
 */
void uart_sendc(char c) {
    // wait until transmitter is empty
    do {
    	asm volatile("nop");
    } while ( !(AUX_MU_LSR & 0x20) );

    // write the character to the buffer 
    AUX_MU_IO = c;
}

/**
 * Receive a character
 */
char uart_getc() {
    char c;

    // wait until data is ready (one symbol)
    do {
    	asm volatile("nop");
    } while ( !(AUX_MU_LSR & 0x01) );

    // read it and return
    c = (unsigned char)(AUX_MU_IO);

    // convert carriage return to newline character
    return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while (*s) {
        // convert newline to carriage return + newline
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
**
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


