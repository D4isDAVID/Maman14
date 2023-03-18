/* utility parsing functions */
#ifndef PARSER_H
#define PARSER_H

#include "linkedlist.h"
#include "symbols.h"

#define MAX_LINE_LENGTH 80 /* max line length without terminator and without possible final newline */

enum parsererrno {
	PARSER_OK,
	PARSER_EUNEXPECTEDSPACE,
	PARSER_EUNEXPECTEDCOMMA,
	PARSER_ENOTENOUGHPARAMS,
	PARSER_ETOOMANYPARAMS,
	PARSER_EEXPECTEDQUOTES,
	PARSER_EUNFINISHEDSTRING,
	PARSER_EINVALIDCHAR,
	PARSER_EJUMPPARAMS,
	PARSER_EINVALIDNUMBER,
	PARSER_EINVALIDLABEL
};

typedef struct word {
	unsigned int field : 14;
} word;

typedef struct instruction {
	void *value;
	int islabel;
} instruction;

word *encodelabel(int);
/* encodes the given operation and its parameters and appends it to the linked list */
enum parsererrno encodeoperation(char *, enum symbol, struct listnode **, struct listnode **, int *);
/* encodes the given string as a number and appends it to the linked list.
	returns whether the function was given a valid number or not */
int encodenum(char *, struct listnode **, int *);
/* encodes the given string as an array of characters and appends it to the linked list.
	returns a parser error number */
enum parsererrno encodestring(char *, struct listnode **, int *);

/* parses parameters in the given string from `i` and appends them to the given linked list.
	returns a parser error number */
enum parsererrno parseparams(char *, int *, int, struct listnode **);
/* returns the address method of the given string */
enum addressmethod determineaddressmethod(char *s);

/* returns whether the given string is a valid number (operator `+`/`-` followed by digits `0-9`) */
int isvalidnum(char *);
/* returns whether the given string is a valid label (30 characters long and starts with a letter followed by letters or digits) */
int isvalidlabel(char *);
/* skips over spaces and tabs in the given line at the given position, and returns their amount */
int skipwhitespace(char [], int *);
/* skips over non-space, non-tab, and non-newline characters in the given line at the given position,
	and returns their amount */
int countnonwhitespace(char [], int *);
/* counts the given string until the given character */
int countuntil(char *, char);
/* duplicates the given string until the given character */
char *dupluntil(char *, char);

#endif
