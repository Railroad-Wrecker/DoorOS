// -----------------------------------mbox.c -------------------------------------
#include "mbox.h" 
#include "gpio.h"
#include "uart.h"

/* Mailbox Data Buffer (each element is 32-bit)*/
/*
* The keyword attribute allows you to specify special attributes
*
* The aligned(N) attribute aligns the current data item on an address
* which is a multiple of N, by inserting padding bytes before the data item
*
* __attribute__((aligned(16)) : allocate the variable on a 16-byte boundary.
*
*
* We must ensure that our our buffer is located at a 16 byte aligned address,
* so only the high 28 bits contain the address
* (last 4 bits is ZERO due to 16 byte alignment)
*
*/

volatile unsigned int __attribute__((aligned(16))) mBuf[36];

/**
* Read from the mailbox
*/
uint32_t mailbox_read(unsigned char channel) {
    //Receiving message is buffer_addr & channel number
    uint32_t res;

    // Make sure that the message is from the right channel
    do {
    // Make sure there is mail to receive
        do {
            asm volatile("nop");
        } while (MBOX0_STATUS & MBOX_EMPTY);

        // Get the message
        res = MBOX0_READ;
    } while ( (res & 0xF) != channel);

    return res;
}

/**
* Write to the mailbox
*/
void mailbox_send(uint32_t msg, unsigned char channel) {
    //Sending message is buffer_addr & channel number

    // Make sure you can send mail
    do {
        asm volatile("nop");
    } while (MBOX1_STATUS & MBOX_FULL);

    // send the message
    MBOX1_WRITE = msg;
}

/**
* Make a mailbox call. Returns 0 on failure, non-zero on success
*/
int mbox_call(unsigned int buffer_addr, unsigned char channel) {
    //Check Buffer Address
    uart_puts("  Buffer Address: ");
    uart_hex(buffer_addr);
    uart_sendc('\n');

    //Prepare Data (address of Message Buffer)
    unsigned int msg = (buffer_addr & ~0xF) | (channel & 0xF);
    mailbox_send(msg, channel);

    /* now wait for the response */
    /* is it a response to our message (same address)? */
    if (msg == mailbox_read(channel)) {
        return (mBuf[1] == MBOX_RESPONSE);
    }
    
    return 0;
}

/**
 * Mailbox setup that supports unlimited numbers of arguments
 */

void mbox_buffer_setup(unsigned int buffer_addr, unsigned int tag_identifier, unsigned int **res_data, ...)
{
    va_list args;
    va_start(args, res_data);
    volatile unsigned int *mbox = (volatile unsigned int *)(uintptr_t)buffer_addr; // Cast to uintptr_t first

    mbox[1] = MBOX_REQUEST;

    switch (tag_identifier)
    {
    case MBOX_TAG_MACADDR: // get MAC Address
        mbox[0] = 7 * 4;   // Size of the entire buffer
        mbox[2] = tag_identifier;
        mbox[3] = 6; // Size of the value buffer
        mbox[4] = 0; // Request code
        mbox[5] = 0; // clear output buffer
        *res_data = (unsigned int *)&mbox[5];
        mbox[6] = MBOX_TAG_LAST;
        break;
    case MBOX_TAG_GETBOARDREVISION:
        mbox[0] = 7 * 4; // Size of the entire buffer
        mbox[2] = tag_identifier;
        mbox[3] = 4;
        mbox[4] = 0; // clear output buffer
        *res_data = (unsigned int *)&mbox[5];
        mbox[6] = MBOX_TAG_LAST;
        break;
    case MBOX_TAG_GETCLKRATE:
        mbox[0] = 8 * 4; // Size of the entire buffer
        mbox[2] = tag_identifier;
        mbox[3] = 8; // Size of the value buffer
        mbox[4] = 0; // Request code
        mbox[6] = 0; // clear output buffer
        mBuf[5] = va_arg(args, unsigned int);
        *res_data = (unsigned int *)&mbox[6];
        mbox[7] = MBOX_TAG_LAST;
        break;
    case MBOX_TAG_VCMEMORY:
        mbox[0] = 7 * 4; // Size of the entire buffer
        mbox[2] = tag_identifier;
        mbox[3] = 8; // Size of the value buffer
        mbox[4] = 0; // Request code
        mbox[5] = 0; // clear output buffer
        *res_data = (unsigned int *)&mbox[5];
        mbox[6] = MBOX_TAG_LAST;
        break;
    case MBOX_TAG_GETFIRMWAREREVISION:
        mbox[0] = 7 * 4; // Size of the entire buffer
        mbox[2] = tag_identifier;
        mbox[3] = 4; // Size of the value buffer
        mbox[4] = 0; // Request code
        mbox[5] = 0; // clear output buffer
        *res_data = (unsigned int *)&mbox[5];
        mbox[6] = MBOX_TAG_LAST;
        break;
    default:
        printf("Unsupported TAG: %d\n", tag_identifier);
        va_end(args);
        return;
    }

    va_end(args);
}

