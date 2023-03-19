#include "errutil.h"

#include <stdlib.h>
#include <stdarg.h>

void *alloc(size_t s)
{
	void *ptr = malloc(s);
	if (ptr == NULL) {
		printf("error: failed to allocate memory\n");
		exit(1);
	}
	return ptr;
}

FILE *open(char *filename, char *mode)
{
	FILE *f = fopen(filename, mode);
	if (f == NULL) {
		printf("error: failed to open file (%s)\n", filename);
		exit(1);
	}
	return f;
}

int close(FILE *file)
{
	int i = fclose(file);
	if (i == EOF) {
		printf("error: failed to close file\n");
		exit(1);
	}
	return i;
}

char **msg;

void errutil_prepare(void)
{
	msg = (char **) alloc(sizeof(*msg) * ERRORCOUNT);

	msg[ERROR_LABELINVALIDNAME] = "label names can't be more than 30 characters long, and must start with a letter followed by letters or numbers (%s)";
	msg[ERROR_LABELSYMBOL] = "label name must not be a pre-defined symbol (%s)";
	msg[ERROR_LABELDEFINED] = "label is already defined (%s)";
	msg[ERROR_LABELNOTDEFINED] = "label is not defined (%s)";
	msg[WARNING_LABELEMPTY] = "label defined without instruction (%s)";
	msg[WARNING_LABELUSELESS] = "useless label definition (%s)";

	msg[ERROR_PARAMSUNEXPECTEDSPACE] = "unexpected space in params list";
	msg[ERROR_PARAMSUNEXPECTEDCOMMA] = "unexpected comma in params list";
	msg[ERROR_PARAMSNOTENOUGH] = "not enough parameters (expected %d)";
	msg[ERROR_PARAMSTOOMANY] = "too many parameters (expected %d)";
	msg[ERROR_PARAMSJUMP] = "jump operations must receive a label followed by either 2 parameters inside parentheses, or no parameters at all";

	msg[ERROR_OPINVALIDSOURCE] = "invalid source parameter for %s";
	msg[ERROR_OPINVALIDDEST] = "invalid destination parameter for %s";
	msg[ERROR_DATAINVALIDNUMBER] = "invalid number (%s)";

	msg[ERROR_STRINGSTARTQUOTES] = "string declarations must start with quotes (\")";
	msg[ERROR_STRINGUNFINISHED] = "unfinished string (strings must end with quotes \")";
	msg[ERROR_STRINGASCII] = "strings must only contain printable ascii characters";

	msg[ERROR_LINEOVERFLOW] = "line exceeds max length of 80";
	msg[ERROR_BINARYOVERFLOW] = "binary file exceeds max memory of 256";

	msg[ERROR_UNKNOWNINSTRUCTION] = "unknown instruction (%s)";
}

void errutil_free(void)
{
	free(msg);
}

void prettyprint(char *, char *, int, char *, va_list);

void printwarn(char *filename, int line, enum errutil_errno e, ...)
{
	va_list args;
	char *s = msg[e];
	va_start(args, e);
	prettyprint("warning", filename, line, s, args);
	va_end(args);
}

void printerr(char *filename, int line, enum errutil_errno e, ...)
{
	va_list args;
	char *s = msg[e];
	va_start(args, e);
	prettyprint("error", filename, line, s, args);
	va_end(args);
}

/* prints the message of the given errno with the given prefix in the given filename in the given line.
	supports `printf`-like formatting */
void prettyprint(char *prefix, char *filename, int line, char *s, va_list args)
{
	printf("%s: %s.am:%d - ", prefix, filename, line);
	vprintf(s, args);
	printf("\n");
}
