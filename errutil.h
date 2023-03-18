/* error message utilities for the first & second phases */
#ifndef ERRUTIL_H
#define ERRUTIL_H

#include <stdio.h>

enum errutil_errno {
	ERROR_LABELINVALIDNAME,
	ERROR_LABELSYMBOL,
	ERROR_LABELDEFINED,
	ERROR_LABELNOTDEFINED,
	WARNING_LABELEMPTY,
	WARNING_LABELUSELESS,

	ERROR_PARAMSUNEXPECTEDSPACE,
	ERROR_PARAMSUNEXPECTEDCOMMA,
	ERROR_PARAMSNOTENOUGH,
	ERROR_PARAMSTOOMANY,
	ERROR_PARAMSJUMP,

	ERROR_DATAINVALIDNUMBER,

	ERROR_STRINGSTARTQUOTES,
	ERROR_STRINGUNFINISHED,
	ERROR_STRINGASCII,

	ERROR_UNKNOWNINSTRUCTION,

	ERROR_LINEOVERFLOW,
	ERROR_BINARYOVERFLOW,

	ERRORCOUNT /* amount of errutil_errno values for later use */
};

/* `malloc` wrapper that if failed, prints an error and exits the program */
void *alloc(size_t);
/* `fopen` wrapper that if failed, prints an error and exits the program */
FILE *open(char *, char *);
/* `fclose` wrapper that if failed, prints an error and exits the program */
int close(FILE *);

void errutil_prepare(void);
void errutil_free(void);

void printwarn(char *, int, enum errutil_errno, ...);
void printerr(char *, int, enum errutil_errno, ...);

#endif
