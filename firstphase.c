/* first assembler phase */
#include "firstphase.h"

#include <string.h>
#include "parser.h"
#include "strutil.h"
#include "errutil.h"

/* phase entry point */
FILE *firstphase(FILE *am, char *filename, struct listnode *instructions, struct listnode *data, struct hashmap *labels, struct hashmap *labelattributes)
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
	struct listnode *params,
		*instructionsptr = instructions,
		*dataptr = data;
	struct hashnode *attributesptr;
	FILE *ob;

	strcat(filename, ".ob");
	ob = fopen(filename, "w");
	replaceextension(filename, ""); /* we need the filename without .ob extension to print in error messages */

	while (fgets(line, MAX_LINE_LENGTH + 2, am) != NULL) {
		linecount++;
		i = 0;
		labeldef = 0;
		labelname = NULL;
		params = NULL;

		skipwhitespace(line, &i);
		count = countnonwhitespace(line, &i);

		if (line[i-1] == ':') {
			labelname = strndupl(&line[i-count], count-1);
			if (!isvalidlabel(labelname)) {
				haserrors = 1;
				printerr(filename, linecount, i-count, ERROR_LABELINVALIDNAME, labelname);
			} else if (symbols_get(labelname) != UNKNOWN_SYMBOL) {
				haserrors = 1;
				printerr(filename, linecount, i-count, ERROR_LABELSYMBOL, labelname);
			} else
				labeldef = 1;
			skipwhitespace(line, &i);
			count = countnonwhitespace(line, &i);
			if (count == 0)
				printwarn(filename, linecount, i-count, WARNING_LABELEMPTY, labelname);
		}

		if (count == 0)
			continue;

		opname = strndupl(&line[i-count], count);
		opcode = symbols_get(opname);
		skipwhitespace(line, &i);

		paramamount = symbols_getparamamount(opcode);
		switch (parseparams(line, &i, paramamount, &params)) {
		case PARSER_EUNEXPECTEDSPACE:
			haserrors = 1;
			printerr(filename, linecount, i-count, ERROR_PARAMSUNEXPECTEDSPACE);
			break;
		case PARSER_EUNEXPECTEDCOMMA:
			haserrors = 1;
			printerr(filename, linecount, i-count, ERROR_PARAMSUNEXPECTEDCOMMA);
			break;
		case PARSER_ENOTENOUGHPARAMS:
			haserrors = 1;
			printerr(filename, linecount, i-count, ERROR_PARAMSNOTENOUGH, paramamount);
			break;
		case PARSER_ETOOMANYPARAMS:
			haserrors = 1;
			printerr(filename, linecount, i-count, ERROR_PARAMSTOOMANY, paramamount);
			break;
		default:
			break;
		}

		if (labeldef) {
			labelattribute = hashmap_getint(labelattributes, labelname);
			if ((isoperation(opname) || isdatadirective(opcode)) && (hashmap_getint(labels, labelname) != NULL || (labelattribute != NULL && *labelattribute & LABEL_EXTERNAL))) {
				haserrors = 1;
				printerr(filename, linecount, i-count, ERROR_LABELDEFINED, labelname);
			}
		}

		if (isdirective(opname)) {
			if (isdatadirective(opcode)) {
				if (labeldef) {
					hashmap_setint(labels, labelname, datacount);
					hashmap_addbittofield(labelattributes, labelname, LABEL_DATA);
				}
				if (opcode == DIRECTIVE_DATA) {
					while (params != NULL) {
						if (isvalidnum((char *) params->value)) {
							/* TODO:
							encodenum(params->value, &data, &datacount);
							 */
						} else {
							haserrors = 1;
							printerr(filename, linecount, i-count, ERROR_DATAINVALIDNUMBER, (char *) params->value);
						}
						params = linkedlist_freenext(params);
					}
				} else {
					switch (encodestring(&line[i], &dataptr, &datacount)) {
					case PARSER_EEXPECTEDQUOTES:
						haserrors = 1;
						printerr(filename, linecount, i-count, ERROR_STRINGSTARTQUOTES);
						break;
					case PARSER_EUNFINISHEDSTRING:
						haserrors = 1;
						printerr(filename, linecount, i-count, ERROR_STRINGUNFINISHED);
						break;
					case PARSER_INVALIDCHAR:
						haserrors = 1;
						printerr(filename, linecount, i-count, ERROR_STRINGASCII);
						break;
					default:
						break;
					}
				}
			} else {
				if (labeldef)
					printwarn(filename, linecount, i-count, WARNING_LABELUSELESS, labelname);
				labelname = strdupl((char *) params->value);
				labelattribute = hashmap_getint(labelattributes, labelname);
				if (!isvalidlabel(labelname)) {
					haserrors = 1;
					printerr(filename, linecount, i-count, ERROR_LABELINVALIDNAME, labelname);
				} else if ((opcode == DIRECTIVE_ENTRY || hashmap_getint(labels, labelname) == NULL) && (labelattribute == NULL || !(*labelattribute & (LABEL_ENTRY | LABEL_EXTERNAL))))
					hashmap_addbittofield(labelattributes, labelname, (opcode == DIRECTIVE_EXTERN ? LABEL_EXTERNAL : LABEL_ENTRY));
				else {
					haserrors = 1;
					printerr(filename, linecount, i-count, ERROR_LABELDEFINED, labelname);
				}
			}
		} else if (isoperation(opname)) {
			if (labeldef) {
				hashmap_setint(labels, labelname, instructioncount);
				hashmap_addbittofield(labelattributes, labelname, LABEL_INSTRUCTION);
			}
			/* TODO */
			instructioncount++;
		} else {
			haserrors = 1;
			printerr(filename, linecount, i-count, ERROR_UNKNOWNINSTRUCTION, opname);
		}

		if (labeldef)
			free(labelname);
		free(opname);
		linkedlist_free(params);
	}

	for (i = 0; i < HASHMAP_CAPACITY; i++) {
		attributesptr = (labelattributes)->tab[i];
		while (attributesptr != NULL) {
			labelattribute = (int *) attributesptr->value;
			if (labelattribute != NULL && *labelattribute & LABEL_DATA)
				hashmap_setint(labels, attributesptr->key, *hashmap_getint(labels, attributesptr->key) + instructioncount);
			attributesptr = attributesptr->next;
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
