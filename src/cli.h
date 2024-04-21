#include "utility.h"

void processCommand(const char *cmd);
void help();
void printCommandHelp(const char *cmd);
void clear();
void setColor(const char *textColor, const char *backgroundColor);
void showInfo();
void autoComplete(char *buffer, int *index);
