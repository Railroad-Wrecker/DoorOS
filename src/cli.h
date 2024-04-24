#include "utility.h"

// Function declarations for existing commands and utilities
void processCommand(const char *cmd);
void help();
void home();
void printCommandHelp(const char *cmd);
void clear();
void setColor(const char *textColor, const char *backgroundColor);
void showInfo();
void autoComplete(char *buffer, int *index);

// UART configuration functions
void setBaudRate(const char *rate);
void setDataBits(const char *bits);
void setStopBits(const char *bits);
void setParity(const char *parity);
void setHandshaking(const char *state);
void showUartConfig();
void processUartCommand(const char *cmd);
