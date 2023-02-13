/* utility parsing functions */

#include "parser.h"

int isvalidspace(char c)
{
	return c == ' ' || c == '\t';
}

int skipwhitespace(char line[], int *i)
{
	int count = 0;
	for (; isvalidspace(line[*i]); (*i)++, count++)
		;
	return count;
}

int countnonwhitespace(char line[], int *i)
{
	int count = 0;
	for (; !isvalidspace(line[*i]); (*i)++, count++)
		;
	return count;
}
