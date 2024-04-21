#include "utility.h"

// Custom string comparison function
int strncmp(const char *string1, const char *string2, size_t n)
{
    // Compare characters until either they differ, one of the strings ends, or n characters have been compared
    while (n > 0 && *string1 && (*string1 == *string2))
    {
        string1++;
        string2++;
        n--;
    }

    // If n characters have been compared or both strings end, return 0 (equal)
    if (n == 0 || (*string1 == '\0' && *string2 == '\0')) {
        return 0; // Equal
    }

    // Calculate the difference between the differing characters and return it
    return *(unsigned char *)string1 - *(unsigned char *)string2;
}

// Custom string copying function
char *strncpy(char *destination, const char *source, size_t n)
{
    // Keep track of the original destination pointer
    char *original_destination = destination;

    // Copy characters from the source to the destination until a null terminator is encountered
    while (*source && n > 0)
    {
        *destination = *source;
        destination++;
        source++;
        n--;
    }

    // Ensure the destination string is null-terminated
    if (n > 0)
    {
        *destination = '\0';
    }

    // Return the original destination pointer
    return original_destination;
}

// Custom string length calculation function
size_t strlen(const char *string)
{
    // Initialize a pointer to traverse the string
    const char *pointer = string;

    // Count characters until a null terminator is reached
    while (*pointer)
    {
        pointer++;
    }

    // Calculate and return the length by subtracting the starting pointer from the current pointer
    return pointer - string;
}

// Function to check if a character is a delimiter
int is_delimiter(char c, const char *delimiter) {
    for (int i = 0; delimiter[i]; i++) {
        if (c == delimiter[i]) {
            return 1;
        }
    }
    return 0;
}

// Custom string tokenization function with reentrancy support
char *strtok_r(char *string, const char *delimiter, char **saveptr)
{
    if (!string && !*saveptr)
    {
        return NULL;
    }

    // Determine the start of the next token
    char *token_start = string ? string : *saveptr;
    char *end = token_start;

    // Skip leading delimiters
    while (*token_start)
    {
        if (is_delimiter(*token_start, delimiter))
        {
            token_start++;
        }
        else
        {
            break;
        }
    }

    // If no more tokens are found, reset the state and return NULL
    if (!*token_start)
    {
        *saveptr = NULL;
        return NULL;
    }

    // Find the end of the current token
    end = token_start;
    while (*end)
    {
        if (is_delimiter(*end, delimiter))
        {
            *end = '\0';
            *saveptr = end + 1;
            break;
        }
        end++;
    }

    // If no more delimiters are found, set saveptr to NULL
    if (!*end)
    {
        *saveptr = NULL;
    }

    // Return the start of the current token
    return token_start;
}

// Custom string searching function
char *strstr(const char *haystack, const char *needle)
{
    // If the needle is an empty string, return the entire haystack
    if (!*needle)
    {
        return (char *)haystack;
    }

    // Iterate through the haystack
    while (*haystack)
    {
        const char *haystack_pointer = haystack;
        const char *needle_pointer = needle;

        // Compare characters of the haystack and needle until they match
        while (*haystack_pointer && *needle_pointer && (*haystack_pointer == *needle_pointer))
        {
            haystack_pointer++;
            needle_pointer++;
        }

        // If we reached the end of the needle, a match is found
        if (!*needle_pointer)
        {
            return (char *)haystack;
        }

        // Move to the next character in the haystack
        haystack++;
    }

    // Return NULL if no match is found
    return NULL;
}

// Function to map color names to ANSI escape codes
const char *mapColorToCodeText(const char *colorName) {
    // Implement a mapping here from color names to ANSI escape codes
    // For example, you can use a switch statement or a lookup table
    if (strncmp(colorName, "black", 5) == 0) {
        return "\033[1;30m";
    } else if (strncmp(colorName, "red", 3) == 0) {
        return "\033[1;31m";
    } else if (strncmp(colorName, "green", 5) == 0) {
        return "\033[1;32m";
    } else if (strncmp(colorName, "yellow", 6) == 0) {
        return "\033[1;33m";
    } else if (strncmp(colorName, "blue", 4) == 0) {
        return "\033[1;34m";
    } else if (strncmp(colorName, "purple", 6) == 0) {
        return "\033[1;35m";
    } else if (strncmp(colorName, "cyan", 4) == 0) {
        return "\033[1;36m";
    } else if (strncmp(colorName, "white", 5) == 0) {
        return "\033[1;37m";
    }
    
    return NULL; // Return NULL
}

// Function to map color names to ANSI escape codes
const char *mapColorToCodeBackground(const char *colorName) {
    // Implement a mapping here from color names to ANSI escape codes
    // For example, you can use a switch statement or a lookup table
    if (strncmp(colorName, "black", 5) == 0) {
        return "\x1b[40m";
    } else if (strncmp(colorName, "red", 3) == 0) {
        return "\x1b[41m";
    } else if (strncmp(colorName, "green", 5) == 0) {
        return "\x1b[42m";
    } else if (strncmp(colorName, "yellow", 6) == 0) {
        return "\x1b[43m";
    } else if (strncmp(colorName, "blue", 4) == 0) {
        return "\x1b[44m";
    } else if (strncmp(colorName, "purple", 6) == 0) {
        return "\x1b[45m";
    } else if (strncmp(colorName, "cyan", 4) == 0) {
        return "\x1b[46m";
    } else if (strncmp(colorName, "white", 5) == 0) {
        return "\x1b[47m";
    }
        
    return NULL; // Return NULL
}