#include "mbox.h"

int strncmp(const char *string1, const char *string2, size_t n);
char *strncpy(char *destination, const char *source, size_t n);
size_t strlen(const char *string);
int is_delimiter(char c, const char *delimiter);
char *strtok_r(char *string, const char *delimiter, char **saveptr);
char *strstr(const char *haystack, const char *needle);

const char *mapColorToCodeText(const char *colorName);
const char *mapColorToCodeBackground(const char *colorName);
