#include "../gcclib/stddef.h"
#include "../gcclib/stdint.h"
#include "../gcclib/stdarg.h"
#include "uart.h"

void printInteger(char *buffer, int *buffer_index, int x, int width, int flag_zero_padding, int flag_left_justify, int flag_width);
void printCharacter(char *buffer, int *buffer_index, int c);
void printFloat(char *buffer, int *buffer_index, double num, int width, int precision, int flag_left_justify, int flag_zero_padding);
void printHex(char *buffer, int *buffer_index, unsigned int num, int width, int flag_zero_padding, int flag_left_justify);
void addPadding(char *buffer, int *buffer_index, int diff, int negative, int flag_zero_padding);
void printFormatted(char *buffer, const char *format, va_list args);
void printf(char *string, ...);