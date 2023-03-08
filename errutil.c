/* error message utilities for the first & second phases */
#include "errutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "hashmap.h"

void *alloc(size_t s)
{
	void *ptr = malloc(s);
	if (ptr == NULL) {
		fprintf(stderr, "error: failed to allocate memory");
		exit(1);
	}
	return ptr;
}

char **msg;

void errutil_prepare(void)
{
	msg = (char **) alloc(sizeof(*msg) * ERRORCOUNT);

	msg[ERROR_LABELINVALIDNAME] = "label names can't be more than 30 characters long, and must start with a letter followed by letters or numbers (%s)";
	msg[ERROR_LABELSYMBOL] = "label name must not be a pre-defined symbol (%s)";
	msg[ERROR_LABELDEFINED] = "label is already defined (%s)";
	msg[WARNING_LABELEMPTY] = "label defined without instruction (%s)";
	msg[WARNING_LABELUSELESS] = "useless label definition (%s)";

	msg[ERROR_PARAMSUNEXPECTEDSPACE] = "unexpected space in params list";
	msg[ERROR_PARAMSUNEXPECTEDCOMMA] = "unexpected comma in params list";
	msg[ERROR_PARAMSNOTENOUGH] = "not enough parameters (expected %d)";
	msg[ERROR_PARAMSTOOMANY] = "too many parameters (expected %d)";

	msg[ERROR_DATAINVALIDNUMBER] = "invalid number (%s)";

	msg[ERROR_STRINGSTARTQUOTES] = "string declarations must start with quotes (\")";
	msg[ERROR_STRINGUNFINISHED] = "unfinished string (strings must end with quotes \")";
	msg[ERROR_STRINGASCII] = "strings must only contain printable ascii characters";

	msg[ERROR_UNKNOWNINSTRUCTION] = "unknown instruction (%s)";
}

void errutil_free(void)
{
	free(msg);
}

void prettyprint(char *prefix, char *filename, int line, int c, char *s, va_list args)
{
	fprintf(stderr, "%s: %s.am:%d:%d - ", prefix, filename, line, c);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
}

void printwarn(char *filename, int line, int c, enum errutil_errno e, ...)
{
	va_list args;
	char *s = msg[e];
	va_start(args, e);
	prettyprint("warning", filename, line, c, s, args);
	va_end(args);
}

void printerr(char *filename, int line, int c, enum errutil_errno e, ...)
{
	va_list args;
	char *s = msg[e];
	va_start(args, e);
	prettyprint("error", filename, line, c, s, args);
	va_end(args);
}
