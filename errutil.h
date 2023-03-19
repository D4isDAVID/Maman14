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

	ERROR_OPINVALIDSOURCE,
	ERROR_OPINVALIDDEST,
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

/* initializes the error message storage */
void errutil_prepare(void);
/* frees the allocated storage */
void errutil_free(void);

/* prints the message of the given errno as a warning in the given filename in the given line.
	supports `printf`-like formatting */
void printwarn(char *, int, enum errutil_errno, ...);
/* prints the message of the given errno as a error in the given filename in the given line.
	supports `printf`-like formatting */
void printerr(char *, int, enum errutil_errno, ...);

#endif
