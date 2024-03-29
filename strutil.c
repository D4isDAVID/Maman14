#include "strutil.h"

#include <string.h>
#include "errutil.h"

char *strndupl(char *s, size_t lim)
{
	char *n = (char *) alloc(sizeof(char) * (lim + 1));
	strncpy(n, s, lim);
	n[lim] = '\0';
	return n;
}

char *strdupl(char *s)
{
	return strndupl(s, strlen(s));
}

void replaceextension(char *s, char *ext)
{
	*strrchr(s, '.') = '\0';
	strcat(s, ext);
}
