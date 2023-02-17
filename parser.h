#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "symbols.h"

#define MAX_LINE_LENGTH 80 /* max line length without terminator and without possible final newline */

typedef struct word {
	unsigned int field : 14;
} word;

enum addressmethod determineaddressmethod(char *s);
int isfileempty(FILE *);
int skipwhitespace(char [], int *);
int countnonwhitespace(char [], int *);

#endif
