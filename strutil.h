/* string utilities */
#ifndef STRUTIL_H
#define STRUTIL_H

#include <stdlib.h>

/* duplicates the first `lim` characters of the given string. avoids overlapping with POSIX function `strndup` which isn't available on every OS */
char *strdupl(char *);
/* duplicates a given string. avoids overlapping with POSIX function `strdup` which isn't available on every OS */
char *strndupl(char *, size_t);
/* replaces the current file extension (the characters after the last occurring dot `.`) with another given extension */
void replaceextension(char *, char *);

#endif
