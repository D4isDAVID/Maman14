#ifndef PARSER_H
#define PARSER_H

#define MAX_LINE_LENGTH 80 /* max line length without terminator and without possible final newline */

int skipwhitespace(char [], int *);
int countnonwhitespace(char [], int *);

#endif
