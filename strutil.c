/* string utilities */
#include "strutil.h"

#include <string.h>
#include "errutil.h"

/* duplicates the first `lim` characters of the given string. avoids overlapping with POSIX function `strndup` */
char *strndupl(char *s, size_t lim)
{
	char *n = (char *) alloc(sizeof(char) * (lim + 1));
	strncpy(n, s, lim);
	n[lim] = '\0';
	return n;
}

/* duplicates a given string. avoids overlapping with POSIX function `strdup` */
char *strdupl(char *s)
{
	return strndupl(s, strlen(s));
}

/* replaces the current file extension (the characters after the last occurring dot `.`) with another given extension */
void replaceextension(char *s, char *ext)
{
	*strrchr(s, '.') = '\0';
	strcat(s, ext);
}
