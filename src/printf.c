#include "printf.h"

// Define the maximum buffer size
#define MAX_SIZE 10000

// Function to format and print an integer into a buffer
void printInteger(char *buffer, int *buffer_index, int x, int width, int flag_zero_padding, int flag_left_justify, int flag_width) {
    char temp_buffer[12]; // Assuming 32-bit integers
    int temp_index = 11;
    int negative = 0;

    if (x < 0) {
        x = -x;
        negative = 1;
    }

    // Convert integer to string
    do {
        temp_buffer[temp_index--] = (x % 10) + '0';
        x /= 10;
    } while (x != 0);

    // Calculate the number of padding spaces needed
    int diff = width - (11 - temp_index);
    if (negative || (flag_width && width > 11)) {
        diff--;
    }

    // Copy the integer string into the buffer
    if (!flag_left_justify) {
        addPadding(buffer, buffer_index, diff, negative, flag_zero_padding);
    }

    while (temp_index < 11) {
        buffer[(*buffer_index)++] = temp_buffer[++temp_index];
    }

    // Copy the padding spaces if left-justified
    if (flag_left_justify) {
        addPadding(buffer, buffer_index, diff, negative, flag_zero_padding);
    }
}

// Function to format and print a character into a buffer
void printCharacter(char *buffer, int *buffer_index, int c) {
    // Add the character to the buffer
    buffer[(*buffer_index)++] = c;
}

// Function to format and print a floating-point number into a buffer with width and precision
void printFloat(char *buffer, int *buffer_index, double num, int width, int precision, int flag_left_justify, int flag_zero_padding)
{
    char temp_buffer[32]; // Sufficient buffer size for double
    int temp_index = 0;

    // Check if the number is negative
    if (num < 0)
    {
        num = -num;
        addPadding(buffer, buffer_index, 0, 1, flag_zero_padding);
    }

    int int_part = (int)num;
    double float_part = num - int_part;

    int exp = 1;
    for (; (int_part / exp) > 0; exp *= 10)
        ;
    while (int_part > 0)
    {
        exp /= 10;
        int digit = int_part / exp;
        int_part -= digit * exp;
        temp_buffer[temp_index++] = digit + '0';
    }

    temp_buffer[temp_index++] = '.';
    for (int i = 0; i < precision; i++)
    {
        float_part *= 10;
        int digit = (int)float_part;
        float_part -= digit;
        temp_buffer[temp_index++] = digit + '0';
    }

    // Round the last digit if needed
    if (float_part >= 0.5)
    {
        temp_buffer[temp_index - 1]++;

        // Check for carry over
        for (int i = temp_index - 1; i >= 0; i--)
        {
            if (temp_buffer[i] > '9')
            {
                temp_buffer[i] = '0';
                if (i > 0)
                {
                    temp_buffer[i - 1]++;
                }
            }
            else
            {
                break;
            }
        }
    }

    int diff = width - temp_index;
    if (diff <= 0 && num < 1){
        addPadding(buffer, buffer_index, 1, 0, 1);
    }

    if (!flag_left_justify)
    {
        addPadding(buffer, buffer_index, diff, 0, flag_zero_padding);
    }
    
    // Copy the number string into the buffer
    for (int i = 0; i < temp_index; i++)
    {
        buffer[(*buffer_index)++] = temp_buffer[i];
    }

    if (flag_left_justify)
    {
        addPadding(buffer, buffer_index, diff, 0, flag_zero_padding);
    }
}

// Function to format and print a hexadecimal number into a buffer with width and zero padding
void printHex(char *buffer, int *buffer_index, unsigned int num, int width, int flag_zero_padding, int flag_left_justify) {
    char temp_buffer[8 * sizeof(unsigned int)]; // Hex buffer size
    int temp_index = sizeof(temp_buffer) - 1;

    // Convert to hexadecimal string
    do {
        unsigned int hex = num & 0xF;
        temp_buffer[temp_index--] = hex < 10 ? hex + '0' : hex - 10 + 'a';
        num >>= 4;
    } while (num != 0);

    // Calculate the number of padding characters needed
    int diff = width - (sizeof(temp_buffer) - temp_index - 1);

    if (!flag_left_justify) {
        // Add padding characters (zeros or spaces)
        while (diff > 0) {
            if (flag_zero_padding) {
                buffer[(*buffer_index)++] = '0';
            } else {
                buffer[(*buffer_index)++] = ' ';
            }
            diff--;
        }
    }

    // No 0x prefix added here

    // Copy hexadecimal string to the buffer
    while (temp_index < sizeof(temp_buffer) - 1) {
        buffer[(*buffer_index)++] = temp_buffer[++temp_index];
    }

    if (flag_left_justify) {
        // Add padding spaces
        while (diff > 0) {
            buffer[(*buffer_index)++] = ' ';
            diff--;
        }
    }
}

// Function to add padding based on flags
void addPadding(char *buffer, int *buffer_index, int diff, int negative, int flag_zero_padding) {
    // Check flag_zero_padding
    if (flag_zero_padding) {
        // Add negative sign if applicable
        if (negative) {
            buffer[(*buffer_index)++] = '-';
            diff--; // Decrease the padding count for the negative sign
        }
        // Add padding zeros
        while (diff > 0) {
            buffer[(*buffer_index)++] = '0';
            diff--;
        }
    } else {
        // Add negative sign if applicable
        if (negative) {
            buffer[(*buffer_index)++] = '-';
            diff--; // Decrease the padding count for the negative sign
        }
        // Add padding spaces
        while (diff > 0) {
            buffer[(*buffer_index)++] = ' ';
            diff--;
        }
    }
}

// Updated printFormatted function
void printFormatted(char *buffer, const char *format, va_list args) {
int buffer_index = 0;

// Loop through the format string
while (*format) 
{
    // Check if the buffer is full
    if (buffer_index == MAX_SIZE - 1)
        break;

    // If the current character is not '%', copy it to the buffer
    if (*format != '%') {
        buffer[buffer_index++] = *format++;
        continue;
    }

    format++;

    // Check for double percent sign
    if (*format == '%') {
        buffer[buffer_index++] = '%';
        format++;
        continue;
    }

    // Initialize variables to hold formatting information
    int width = 0;
    int precision = 6; // Default precision is 6
    int flag_width = 0;
    int flag_zero_padding = 0; // Flag for zero padding
    int flag_left_justify = 0; // Flag for left-justified output

    // Check for left-justified flag
    if (*format == '-') {
        flag_left_justify = 1;
        format++;
    }

    // Check for zero padding flag
    if (*format == '0') 
    {
        flag_zero_padding = 1;
        format++;
    }

    // Check for width specifier
    if (*format >= '0' && *format <= '9') {
        flag_width = 1;
        while (*format >= '0' && *format <= '9') {
            width = width * 10 + (*format++ - '0');
        }
    } else if (*format == '*') {
        flag_width = 1;
        width = va_arg(args, int);
        format++;
    }

    // Check for precision specifier
    if (*format == '.') {
        format++;

        if (*format == '*') {
            precision = va_arg(args, int);
            format++;
        } else {
            precision = 0;
            while (*format >= '0' && *format <= '9') {
                precision = precision * 10 + (*format++ - '0');
            }
        }
    }

    // Handle different format specifiers
    switch (*format) {
        // Handle integer formatting
        case 'd':
            printInteger(buffer, &buffer_index, va_arg(args, int), width, flag_zero_padding, flag_left_justify, flag_width);
            format++; // Increment format pointer
            break;

        case 'c':
            printCharacter(buffer, &buffer_index, va_arg(args, int));
            format++; // Increment format pointer
            break;

        case 's':
        {
            char *str = va_arg(args, char *);
            int str_len = 0;
            while (str[str_len])
                str_len++;

            int diff = width - str_len;

            // Add padding spaces
            while (diff > 0)
            {
                buffer[buffer_index++] = ' ';
                diff--;
            }

            // Copy the string to the buffer
            while (*str)
            {
                buffer[buffer_index++] = *str++;
            }

            format++; // Increment format pointer
            break;
        }

        case 'f':
            printFloat(buffer, &buffer_index, va_arg(args, double), width, precision, flag_left_justify, flag_zero_padding);
            format++; // Increment format pointer
            break;

        case 'x':
            printHex(buffer, &buffer_index, va_arg(args, unsigned int), width, flag_zero_padding, flag_left_justify);
            format++; // Increment format pointer
            break;

        default:
            buffer[buffer_index++] = '%';
            buffer[buffer_index++] = *format++;
            break;
    }
}


    // Null-terminate the buffer
    buffer[buffer_index] = '\0';
}


// Custom printf function
void printf(char *format, ...) {
    va_list args;
    va_start(args, format);

    // Create a buffer to store the formatted string
    char buffer[MAX_SIZE];

    // Format the input and store it in the buffer
    printFormatted(buffer, format, args);

    va_end(args);

    // Output the formatted string
    uart_puts(buffer);
}
