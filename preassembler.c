#include "preassembler.h"

#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "hashmap.h"
#include "strutil.h"
#include "errutil.h"

#define MCR "mcr"
#define ENDMCR "endmcr"

/* returns whether the given line at the current location is a valid `mcr` statement
	the value of `macroname` may be changed regardless of the result */
int isvalidmcr(char *, int *, int *, char **);
/* returns whether the given line at the current location is a valid `endmcr` statement */
int isvalidendmcr(char *, int *, int *);
/* counts amount of lines until nearest `endmcr` and allocates a string with enough memory for the macro content */
char *macrocontentalloc(FILE *);

FILE *preassembler(FILE *as, char *filename)
{
	char line[MAX_LINE_LENGTH + 2], /* current source code line including null terminator and possible newline */
		*newlineptr, /* used for removing final newline in macros (otherwise there will be duplicates) */
		*macroname, /*  name name in current macro definition */
		*macrocontent; /* macro content in current macro definition or replacement */
	int i, /* current character in current line */
		count, /* results from `skipwhitespace` and `countnonwhitespace` */
		lineoffset, /* used for printing parts of the current line to avoid printing macro names */
		macrodef = 0; /* whether we are in a macro definition */
	struct hashmap *macros = hashmap_new();

	FILE *am;
	strcat(filename, ".am");
	am = open(filename, "w");

	while (fgets(line, MAX_LINE_LENGTH + 2, as) != NULL) {
		lineoffset = 0;
		i = 0;

		skipwhitespace(line, &i);
		count = countnonwhitespace(line, &i);

		/* ignore comments and empty lines */
		if (line[i-count] == ';' || count == 0)
			continue;

		if (!macrodef)
			if (isvalidmcr(line, &i, &count, &macroname)) {
				macrocontent = macrocontentalloc(as);
				macrodef = 1;
			} else {
				/* while the line contains more content */
				while (count > 0) {
					macroname = strndupl(&line[i-count], count);
					/* if a macro of this name exists and a built-in constant of the same name doesn't exist */
					if ((macrocontent = hashmap_getstr(macros, macroname)) != NULL && symbols_get(macroname) == UNKNOWN_SYMBOL) {
						line[i-count] = '\0'; /* to print the current line up to the macro name */
						fputs(&line[lineoffset], am);
						fputs(macrocontent, am); /* print the macro content */
						lineoffset = i; /* offset the line to print the current line after the macro name */
					}
					free(macroname);
					if (skipwhitespace(line, &i) == 0)
						break;
					count = countnonwhitespace(line, &i);
				}

				fputs(&line[lineoffset], am);
			}
		else
			if (isvalidendmcr(line, &i, &count)) {
				macrodef = 0;
				/* remove final newline to avoid duplicates */
				newlineptr = strrchr(macrocontent, '\n');
				if (newlineptr != NULL)
					*newlineptr = '\0';
				hashmap_setstr(macros, macroname, macrocontent);
				/* free because the hashmap copied everything */
				free(macroname);
				free(macrocontent);
			} else
				strcat(macrocontent, line);
	}

	hashmap_free(macros);

	close(am);
	am = open(filename, "r");
	replaceextension(filename, "");

	return am;
}

char *macrocontentalloc(FILE *as)
{
	char line[MAX_LINE_LENGTH + 2]; /* current line in file */
	int i, /* current character in current line */
		lines = 0, /* amount of lines before nearest `endmcr` */
		count = 0; /* results from `skipwhitespace` and `countnonwhitespace` */
	fpos_t p; /* save current position to return to after counting lines */
	char *ptr; /* the memory will be allocated here */

	fgetpos(as, &p);

	/* count lines until we hit a valid `endmcr` */
	while (fgets(line, MAX_LINE_LENGTH + 2, as) != NULL) {
		i = 0;
		skipwhitespace(line, &i);
		count = countnonwhitespace(line, &i);
		if (isvalidendmcr(line, &i, &count))
			break;
		lines++;
	}

	fsetpos(as, &p);

	ptr = (char *) alloc(sizeof(char) * ((MAX_LINE_LENGTH + 1) * lines) + 1);
	*ptr = '\0';

	return ptr;
}

int isvalidmcr(char *line, int *i, int *count, char **macroname)
{
	/* if the given string between the indices `i` and `count` are equal to and of the same length as `mcr` */
	if (*count != sizeof(MCR)-1 || strncmp(MCR, &line[(*i)-(*count)], *count) != 0)
		return 0;
	if (skipwhitespace(line, i) == 0)
		return 0;
	/* get the macro name */
	if ((*count = countnonwhitespace(line, i)) == 0)
		return 0;
	*macroname = strndupl(&line[(*i)-(*count)], *count);
	skipwhitespace(line, i);
	/* make sure there's nothing after the macro name */
	return countnonwhitespace(line, i) == 0;
}

int isvalidendmcr(char *line, int *i, int *count)
{
	/* if the given string between the indices `i` and `count` are equal to and of the same length as `endmcr` */
	if (*count != sizeof(ENDMCR)-1 || strncmp(ENDMCR, &line[(*i)-(*count)], *count) != 0)
		return 0;
	skipwhitespace(line, i);
	return countnonwhitespace(line, i) == 0;
}
