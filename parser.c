/* utility parsing functions */
#include "parser.h"

/* converts the given line to a 14-bit word */
word linetoword(char *line)
{
	word w;
	return w;
}

/* returns whether the given character is either a space or a tab */
int isvalidspace(char c)
{
	return c == ' ' || c == '\t';
}

/* returns whether the given file is empty */
int isfileempty(FILE *f)
{
	int i;
	fseek(f, 0, SEEK_END);
	i = ftell(f) == 0;
	rewind(f);
	return i;
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
	for (; !isvalidspace(line[*i]) && line[*i] != '\n'; (*i)++, count++)
		;
	return count;
}
