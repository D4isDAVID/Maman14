#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
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
	PARSER_INVALIDCHAR
};

typedef struct word {
	unsigned int field : 14;
} word;

typedef struct instruction {
	void *value;
	int islabel;
} instruction;

int encodenum(char *, struct listnode **, int *);
enum parsererrno encodestring(char *, struct listnode **, int *);
enum parsererrno parseparams(char *, int *, int, struct listnode **);
enum addressmethod determineaddressmethod(char *s);
int isvalidnum(char *);
int isvalidlabel(char *);
int skipwhitespace(char [], int *);
int countnonwhitespace(char [], int *);
char *dupluntil(char *, char);

#endif
