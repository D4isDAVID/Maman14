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

enum parsererrno encodeoperation(char *, enum symbol, struct listnode **, struct listnode **, int *);
int encodenum(char *, struct listnode **, int *);
word *encodelabel(int val);
enum parsererrno encodestring(char *, struct listnode **, int *);
enum parsererrno parseparams(char *, int *, int, struct listnode **);
enum addressmethod determineaddressmethod(char *s);
int isvalidnum(char *);
int isvalidlabel(char *);
int skipwhitespace(char [], int *);
int countnonwhitespace(char [], int *);
char *dupluntil(char *, char);

#endif
