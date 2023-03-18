/* string utilities */
#ifndef STRUTIL_H
#define STRUTIL_H

#include <stdlib.h>

/* duplicates the first `lim` characters of the given string. avoids overlapping with POSIX function `strndup` */
char *strdupl(char *);
/* duplicates a given string. avoids overlapping with POSIX function `strdup` */
char *strndupl(char *, size_t);
/* replaces the current file extension (the characters after the last occurring dot `.`) with another given extension */
void replaceextension(char *, char *);

#endif
