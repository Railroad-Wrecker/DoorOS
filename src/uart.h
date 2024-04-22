#include "gpio0.h"

/* PL011 UART (UART0) registers */
#define UART0_DR        (*(volatile unsigned int *)(MMIO_BASE + 0x00201000))
#define UART0_RSRECR    (*(volatile unsigned int *)(MMIO_BASE + 0x00201004))
#define UART0_FR        (*(volatile unsigned int *)(MMIO_BASE + 0x00201018))
#define UART0_ILPR      (*(volatile unsigned int *)(MMIO_BASE + 0x00201020))
#define UART0_IBRD      (*(volatile unsigned int *)(MMIO_BASE + 0x00201024))
#define UART0_FBRD      (*(volatile unsigned int *)(MMIO_BASE + 0x00201028))
#define UART0_LCRH      (*(volatile unsigned int *)(MMIO_BASE + 0x0020102C))
#define UART0_CR        (*(volatile unsigned int *)(MMIO_BASE + 0x00201030))
#define UART0_IFLS      (*(volatile unsigned int *)(MMIO_BASE + 0x00201034))
#define UART0_IMSC      (*(volatile unsigned int *)(MMIO_BASE + 0x00201038))
#define UART0_RIS       (*(volatile unsigned int *)(MMIO_BASE + 0x0020103C))
#define UART0_MIS       (*(volatile unsigned int *)(MMIO_BASE + 0x00201040))
#define UART0_ICR       (*(volatile unsigned int *)(MMIO_BASE + 0x00201044))
#define UART0_DMACR     (*(volatile unsigned int *)(MMIO_BASE + 0x00201048))
#define UART0_ITCR      (*(volatile unsigned int *)(MMIO_BASE + 0x00201080))
#define UART0_ITIP      (*(volatile unsigned int *)(MMIO_BASE + 0x00201084))
#define UART0_ITOP      (*(volatile unsigned int *)(MMIO_BASE + 0x00201088))
#define UART0_TDR       (*(volatile unsigned int *)(MMIO_BASE + 0x0020108C))

/* Function prototypes */
void uart_init();
void uart_sendc(char c);
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned int num);
void uart_dec(int num);
