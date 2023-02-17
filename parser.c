/* utility parsing functions */
#include "parser.h"

#include <ctype.h>

/* returns the address method of the given string */
enum addressmethod determineaddressmethod(char *s)
{
	if (s[0] == '#') {
		if (s[1] != '-' && s[1] != '+' && !isdigit(s[1]))
			return ADDRESS_ERROR;
		for (s = &s[2]; *s != '\0'; s++)
			if (!isdigit(*s))
				return ADDRESS_ERROR;
		return ADDRESS_INSTANT;
	} else if (s[0] == 'r' && s[1] >= '0' && s[1] <= '7' && s[2] == '\0')
		return ADDRESS_DIRECT_REGISTER;
	return ADDRESS_DIRECT;
}

/* returns whether the given character is either a space or a tab */
int isvalidspace(char c)
{
	return c == ' ' || c == '\t';
}

/* skips over spaces and tabs in the given line at the given position, and returns their amount */
int skipwhitespace(char line[], int *i)
{
	int count = 0;
	for (; isvalidspace(line[*i]); (*i)++, count++)
		;
	return count;
}

/* skips over non-space, non-tab, and non-newline characters in the given line at the given position,
	and returns their amount */
int countnonwhitespace(char line[], int *i)
{
	int count = 0;
	for (; !isvalidspace(line[*i]) && line[*i] != '\n' && line[*i] != '\0'; (*i)++, count++)
		;
	return count;
}
