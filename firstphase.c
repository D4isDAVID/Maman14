/* first assembler phase */
#include "firstphase.h"

#include <stdarg.h>
#include <string.h>
#include "hashmap.h"
#include "parser.h"
#include "strutil.h"

void printwarn(char *, int, int, char *, ...);
void printerr(char *, int, int, char *, ...);

/* phase entry point */
FILE *firstphase(FILE *am, char *filename, struct hashmap **labels, struct hashmap **entext)
{
	char line[MAX_LINE_LENGTH + 2],
		*labelname,
		*opname;
	int i,
		count,
		labeldef,
		linecount = 0,
		datacount = 0,
		instructioncount = 0,
		haserrors = 0;
	enum symbol opcode;
	FILE *ob, *instr, *data;
	strcat(filename, ".ob");
	ob = fopen(filename, "w");
	instr = fopen("instr.temp", "w");
	data = fopen("data.temp", "w");
	*labels = hashmap_new();
	*entext = hashmap_new();
	while (fgets(line, MAX_LINE_LENGTH + 2, am)) {
		linecount++;
		if (line[0] == ';')
			continue;
		i = 0;
		labeldef = 0;
		skipwhitespace(line, &i);
		count = countnonwhitespace(line, &i);
		if (line[i-1] == ':') {
			labelname = strndupl(&line[i-count], count-1);
			if (symbols_get(labelname) != UNKNOWN_SYMBOL) {
				haserrors = 1;
				printerr(filename, linecount, i-count, "label name must not be a defined symbol (%s)", labelname);
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
		if (isoperation(opname)) {
			instructioncount++;
			if (labeldef)
				hashmap_setint(*labels, labelname, instructioncount);
		} else if (isdirective(opname)) {
			if (isdatadirective(opcode)) {
				datacount++;
			} else {
				if (labeldef)
					printwarn(filename, linecount, i-count, "useless label definition");
				hashmap_setint(*entext, labelname, opcode == DIRECTIVE_ENTRY ? LABEL_ENTRY : LABEL_EXTERNAL);
			}
		} else {
			haserrors = 1;
			printerr(filename, linecount, i-count, "unknown instruction %s", opname);
		}
	}
	fclose(ob);
	ob = NULL;
	if (!haserrors && (instructioncount > 0 && datacount > 0))
		ob = fopen(filename, "r");
	replaceextension(filename, "");
	return ob;
}

void prettyprint(char *prefix, char *filename, int line, int c, char *s, va_list args)
{
	fprintf(stderr, "%s: %s.as:%d:%d - ", prefix, filename, line, c);
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
