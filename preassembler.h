/* preassembler: parses macros, removes empty lines and comment lines */
#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H

#include <stdio.h>

/* preassembler entry point */
FILE *preassembler(FILE *fd, char *nameF);

#endif
