/* preassembler code */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mlist.h"
#include "parser.h"
#include "preassembler.h"

#define MCR "mcr"
#define ENDMCR "endmcr"

int isvalidmcr(char *, int *, int *, char **);
int isvalidendmcr(char *, int *, int *);
char *getmacrocontentptr(FILE *);

/* preassembler entry point */
FILE *preassembler(FILE *as, char *filename) {
	char line[MAX_LINE_LENGTH + 2], /* current source code line including null terminator and possible newline */
		*macroname, /*  name name in current macro definition */
		*macrocontent; /* macro content in current macro definition or replacement */
	int i, /* current character in current line */
		count, /* results from `skipwhitespace` and `countnonwhitespace` */
		macrodef = 0; /* whether we are in a macro definition */
	FILE *am;
	strcat(filename, ".am");
	am = fopen(filename, "w+");
	while (fgets(line, MAX_LINE_LENGTH + 2, as) != NULL) {
		i = 0;
		skipwhitespace(line, &i);
		count = countnonwhitespace(line, &i);
		if (count == 0 || line[i-count] == ';')
			continue;
		if (!macrodef)
			if (isvalidmcr(line, &i, &count, &macroname)) {
				macrocontent = getmacrocontentptr(as);
				macrodef = 1;
			} else {
				macroname = (char *) malloc(sizeof(char) * (count+1));
				strncpy(macroname, &line[i-count], count);
				macroname[count] = '\0';
				if ((macrocontent = mlist_lookup(macroname)) != NULL)
					fputs(macrocontent, am);
				else
					fputs(line, am);
				free(macroname);
			}
		else
			if (isvalidendmcr(line, &i, &count)) {
				macrodef = 0;
				mlist_add(macroname, macrocontent);
				free(macroname);
				free(macrocontent);
			} else
				strcat(macrocontent, line);
	}
	mlist_clear();
	fclose(am);
	am = fopen(filename, "r");
	return am;
}

/* counts amount of lines until nearest `endmcr` and returns a string with enough memory for content of the macro */
char *getmacrocontentptr(FILE *as)
{
	char line[MAX_LINE_LENGTH + 2]; /* current line in file */
	int i, /* current character in current line */
		lines = 0, /* amount of lines before nearest `endmcr` */
		count = 0; /* results from `skipwhitespace` and `countnonwhitespace` */
	fpos_t p; /* save current position to return to after counting lines */
	fgetpos(as, &p);
	while (fgets(line, MAX_LINE_LENGTH + 2, as) != NULL) {
		i = 0;
		skipwhitespace(line, &i);
		count = countnonwhitespace(line, &i);
		if (isvalidendmcr(line, &i, &count))
			break;
		lines++;
	}
	fsetpos(as, &p);
	return (char *) malloc(sizeof(char) * ((MAX_LINE_LENGTH + 1) * lines) + 1);
}

/* returns whether the given line at the current location is a valid `mcr` statement
	the value of `macroname` may be changed regardless of the result */
int isvalidmcr(char *line, int *i, int *count, char **macroname)
{
	if (*count != sizeof(MCR)-1 || strncmp(MCR, &line[(*i)-(*count)], *count) != 0)
		return 0;
	if (skipwhitespace(line, i) == 0)
		return 0;
	if ((*count = countnonwhitespace(line, i)) == 0)
		return 0;
	*macroname = (char *) malloc(sizeof(char) * ((*count)+1));
	strncpy(*macroname, &line[(*i)-(*count)], *count);
	(*macroname)[(*count)+1] = '\0';
	skipwhitespace(line, i);
	return countnonwhitespace(line, i) == 0;
}

/* returns whether the given line at the current location is a valid `endmcr` statement */
int isvalidendmcr(char *line, int *i, int *count)
{
	if (*count != sizeof(ENDMCR)-1 || strncmp(ENDMCR, &line[(*i)-(*count)], *count) != 0)
		return 0;
	skipwhitespace(line, i);
	return countnonwhitespace(line, i) == 0;
}
