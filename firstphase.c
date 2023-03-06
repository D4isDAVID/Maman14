/* first assembler phase */
#include "firstphase.h"

#include <stdarg.h>
#include <string.h>
#include "parser.h"
#include "strutil.h"

void printwarn(char *, int, int, char *, ...);
void printerr(char *, int, int, char *, ...);

/* phase entry point */
FILE *firstphase(FILE *am, char *filename, struct listnode **instructions, struct listnode **data, struct hashmap **labels, struct hashmap **labelattributes)
{
	char line[MAX_LINE_LENGTH + 2], /* current line */
		*labelname, /* label name in label definition */
		*opname; /* operation name in current line */
	int i, /* current character in current line */
		count, /* results from `skipwhitespace` and `countnonwhitespace` */
		labeldef, /* whether the current line has a label */
		paramamount, /* parameter amount of current operation */
		*labelattribute, /* hashmap values of labelattributes */
		linecount = 0, /* count of lines */
		datacount = 0, /* count of data instructions */
		instructioncount = 0, /* count of all instructions */
		haserrors = 0; /* whether an error has been detected somewhere in the file */
	enum symbol opcode; /* operation code in current line */
	struct listnode *tmp,
		*params = NULL,
		*instructionsptr = *instructions,
		*dataptr = *data;
	struct hashnode *labelattributesptr;
	FILE *ob;
	strcat(filename, ".ob");
	ob = fopen(filename, "w");
	*labels = hashmap_new();
	*labelattributes = hashmap_new();
	replaceextension(filename, ""); /* we need the filename without .ob extension to print in error messages */
	while (fgets(line, MAX_LINE_LENGTH + 2, am) != NULL) {
		linecount++;
		i = 0;
		labeldef = 0;
		skipwhitespace(line, &i);
		count = countnonwhitespace(line, &i);
		if (line[i-1] == ':') {
			labelname = strndupl(&line[i-count], count-1);
			if (!isvalidlabel(labelname)) {
				haserrors = 1;
				printerr(filename, linecount, i-count, "label names must start with a letter followed by letters or numbers (%s)", labelname);
			} else if (symbols_get(labelname) != UNKNOWN_SYMBOL) {
				haserrors = 1;
				printerr(filename, linecount, i-count, "label name must not be a pre-defined symbol (%s)", labelname);
			} else
				labeldef = 1;
			skipwhitespace(line, &i);
			count = countnonwhitespace(line, &i);
			if (count == 0) {
				haserrors = 1;
				printerr(filename, linecount, i-count, "label defined without instruction");
			}
		}
		if (count == 0)
			continue;
		opname = strndupl(&line[i-count], count);
		opcode = symbols_get(opname);
		skipwhitespace(line, &i);
		paramamount = symbols_getparamamount(opcode);
		switch (parseparams(line, &i, paramamount, &params)) {
		case PARSER_EEXPECTEDCOMMA:
			haserrors = 1;
			printerr(filename, linecount, i-count, "expected comma after space");
			break;
		case PARSER_ENOTENOUGHPARAMS:
			haserrors = 1;
			printerr(filename, linecount, i-count, "not enough parameters (expected %d)", paramamount);
			break;
		case PARSER_ETOOMANYPARAMS:
			haserrors = 1;
			printerr(filename, linecount, i-count, "too many parameters (expected %d)", paramamount);
			break;
		default:
			break;
		}
		labelattribute = hashmap_getint(*labelattributes, labelname);
		if (labeldef && (isoperation(opname) || isdatadirective(opcode)) && !(hashmap_getint(*labels, labelname) == NULL && (labelattribute == NULL || *labelattribute & LABEL_ENTRY))) {
			haserrors = 1;
			printerr(filename, linecount, i-count, "label is already defined (%s)", labelname);
		}
		if (isdirective(opname)) {
			if (isdatadirective(opcode)) {
				if (labeldef) {
					hashmap_setint(*labels, labelname, datacount);
					hashmap_setint(*labelattributes, labelname, LABEL_DATA);
				}
				if (opcode == DIRECTIVE_DATA) {
					while (params != NULL) {
						tmp = params->next;
						if (isvalidnum((char *) params->value)) {
							/* TODO:
							encodenum(params->value, &data, &datacount);
							 */
						} else {
							haserrors = 1;
							printerr(filename, linecount, i-count, "invalid number %s", params->value);
							break;
						}
						linkedlist_freenode(params);
						params = tmp;
					}
				} else {
					switch (encodestring(line, &dataptr, &datacount)) {
					case PARSER_EEXPECTEDQUOTES:
						haserrors = 1;
						printerr(filename, linecount, i-count, "string declarations must start with quotes (\")");
						break;
					case PARSER_EUNFINISHEDSTRING:
						haserrors = 1;
						printerr(filename, linecount, i-count, "string declarations must end with quotes (\")");
						break;
					case PARSER_INVALIDCHAR:
						haserrors = 1;
						printerr(filename, linecount, i-count, "strings must contain printable ascii characters");
						break;
					default:
						break;
					}
				}
			} else {
				if (labeldef)
					printwarn(filename, linecount, i-count, "useless label definition");
				skipwhitespace(line, &i);
				count = countnonwhitespace(line, &i);
				labelname = strndupl(&line[i-count], count);
				if ((labelattribute == NULL || !(*labelattribute & (LABEL_ENTRY | LABEL_EXTERNAL))) && (opcode != DIRECTIVE_EXTERN || hashmap_getint(*labels, labelname) == NULL))
					hashmap_setint(*labelattributes, labelname, (opcode == DIRECTIVE_EXTERN ? LABEL_EXTERNAL : LABEL_ENTRY));
				else {
					haserrors = 1;
					printerr(filename, linecount, i-count, "label is already defined (%s)", labelname);
				}
			}
		} else if (isoperation(opname)) {
			/* TODO */
			instructioncount++;
			if (labeldef)
				hashmap_setint(*labels, labelname, instructioncount);
		} else {
			haserrors = 1;
			printerr(filename, linecount, i-count, "unknown instruction %s", opname);
		}
	}
	for (i = 0; i < HASHMAP_CAPACITY; i++) {
		labelattributesptr = (*labelattributes)->tab[i];
		while (labelattributesptr != NULL) {
			if (*((int *) labelattributesptr->value) & LABEL_DATA)
				hashmap_setint(*labels, labelattributesptr->key, *hashmap_getint(*labels, labelattributesptr->key) + instructioncount);
			labelattributesptr = labelattributesptr->next;
		}
	}
	fprintf(ob, "%d %d\n", instructioncount, datacount);
	fclose(ob);
	ob = NULL;
	if (!haserrors && (instructioncount > 0 || datacount > 0)) {
		strcat(filename, ".ob");
		ob = fopen(filename, "r");
		replaceextension(filename, "");
	}
	return ob;
}

void prettyprint(char *prefix, char *filename, int line, int c, char *s, va_list args)
{
	fprintf(stderr, "%s: %s.am:%d:%d - ", prefix, filename, line, c);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
}

void printwarn(char *filename, int line, int c, char *s, ...)
{
	va_list args;
	va_start(args, s);
	prettyprint("warning", filename, line, c, s, args);
	va_end(args);
}

void printerr(char *filename, int line, int c, char *s, ...)
{
	va_list args;
	va_start(args, s);
	prettyprint("error", filename, line, c, s, args);
	va_end(args);
}
