#include <stdio.h>

enum errutil_errno {
	ERROR_LABELINVALIDNAME,
	ERROR_LABELSYMBOL,
	ERROR_LABELDEFINED,
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

	ERRORCOUNT /* amount of errutil_errno values for later use */
};

void *alloc(size_t);
FILE *open(char *filename, char *mode);

void errutil_prepare(void);
void errutil_free(void);

void printwarn(char *, int, int, enum errutil_errno, ...);
void printerr(char *, int, int, enum errutil_errno, ...);
