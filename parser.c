/* utility parsing functions */
#include "parser.h"

#include <string.h>
#include <ctype.h>
#include "strutil.h"

enum parsererrno encodestring(char *s, struct listnode **data, int *datacount)
{
	struct listnode *tmp, *n = *data;
	char *end;
	word *w;
	int i = 0;
	if (*s != '"')
		return PARSER_EEXPECTEDQUOTES;
	end = strrchr(s, '"') + 1;
	skipwhitespace(end, &i);
	if (s == end-1 || countnonwhitespace(end, &i) > 0)
		return PARSER_EUNFINISHEDSTRING;
	for (s++; s != end; s++) {
		if (!isprint(*s))
			return PARSER_INVALIDCHAR;
		w = (word *) malloc(sizeof(*w));
		w->field = *s;
		tmp = linkedlist_newnode(&w);
		if (n != NULL)
			(n)->next = tmp;
		else
			*data = tmp;
		n = tmp;
		(*datacount)++;
	}
	return PARSER_OK;
}

enum parsererrno parseparams(char *line, int *i, int paramamount, struct listnode **n)
{
	int count, ii;
	char *param;
	struct listnode *tmp;
	if (paramamount == PARAM_UNKNOWN)
		return PARSER_OK;
	/* TODO: rework loop */
	while ((count = countnonwhitespace(line, i)) > 0 && paramamount != 0) {
		param = dupluntil(&line[(*i)-count], ',');
		*i -= count;
		ii = 0;
		if (countnonwhitespace(param, &ii) == 0)
			return PARSER_EUNEXPECTEDCOMMA;
		if ((count = skipwhitespace(param, &ii)) > 0) {
			if (countnonwhitespace(param, &ii) > 0)
				return PARSER_EUNEXPECTEDSPACE;
			param[ii-count] = '\0';
		}
		tmp = linkedlist_newnode(param);
		if (*n != NULL)
			(*n)->next = tmp;
		*n = tmp;
		*i += ii;
		if (line[*i] == ',')
			(*i)++;
		skipwhitespace(line, i);
		paramamount--;
	}
	if (paramamount > 0)
		return PARSER_ENOTENOUGHPARAMS;
	else if (paramamount == 0 && count > 0)
		return PARSER_ETOOMANYPARAMS;
	return PARSER_OK;
}

/* returns the address method of the given string */
enum addressmethod determineaddressmethod(char *s)
{
	if (s[0] == '#')
		return isvalidnum(&s[1]) ? ADDRESS_INSTANT : ADDRESS_ERROR;
	else if (s[0] == 'r' && s[1] >= '0' && s[1] <= '7' && s[2] == '\0')
		return ADDRESS_DIRECT_REGISTER;
	return ADDRESS_DIRECT;
}

int isvalidnum(char *s)
{
	if (*s != '-' && *s != '+' && !isdigit(*s))
		return 0;
	for (s++; *s != '\0' && *s != EOF; s++)
		if (!isdigit(*s))
			return 0;
	return 1;
}

int isvalidlabel(char *s)
{
	if (!isalpha(*s))
		return 0;
	for (s++; *s != '\0' && *s != EOF; s++)
		if (!isalnum(*s))
			return 0;
	return 1;
}

/* returns whether the given character is either a space or a tab */
int isvalidspace(char c)
{
	return c == ' ' || c == '\t';
}

/* returns whether the given character terminates the line in any way */
int islineterminator(char c)
{
	return c == '\n' || c == '\0' || c == EOF;
}

/* skips over spaces and tabs in the given line at the given position, and returns their amount */
int skipwhitespace(char line[], int *i)
{
	int count = 0;
	for (; isvalidspace(line[*i]) && !islineterminator(line[*i]); (*i)++, count++)
		;
	return count;
}

/* skips over non-space, non-tab, and non-newline characters in the given line at the given position,
	and returns their amount */
int countnonwhitespace(char line[], int *i)
{
	int count = 0;
	for (; !isvalidspace(line[*i]) && !islineterminator(line[*i]); (*i)++, count++)
		;
	return count;
}

char *dupluntil(char *s, char c)
{
	int count;
	for (count = 0; s[count] != c && !islineterminator(s[count]); count++)
		;
	return strndupl(s, count);
}
