#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H

#include <stdio.h>

void sort(char *data, char *sours, int length, FILE *fp);
FILE *preassembler(FILE *fd, char *nameF);
#endif
