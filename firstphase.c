#include "firstphase.h"

#include <string.h>
#include "parser.h"
#include "strutil.h"
#include "errutil.h"

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
		toolong = 0, /* whether the current line is too long */
		haserrors = 0; /* whether an error has been detected somewhere in the file */
	enum symbol opcode; /* operation code in current line */
	struct listnode *params,
		*instructionptr = instructions,
		*dataptr = data;
	struct hashnode *attributesptr;
	FILE *ob;

	while (fgets(line, MAX_LINE_LENGTH + 1, am) != NULL) {
		linecount++;
		toolong = 0;

		while (strchr(line, '\n') == NULL && strchr(line, EOF) == NULL) {
			if (fgets(line, MAX_LINE_LENGTH + 1, am) == NULL)
				break;
			if (!toolong) {
				printerr(filename, linecount, ERROR_LINEOVERFLOW);
				toolong = 1;
				haserrors = 1;
			}
		}
		if (toolong)
			continue;

		i = 0;
		labeldef = 0;
		labelname = NULL;
		params = NULL;

		skipwhitespace(line, &i);
		count = countuntil(&line[i], ':');

		if (line[i+count] == ':') {
			labelname = strndupl(&line[i], count);
			if (symbols_get(labelname) != UNKNOWN_SYMBOL) {
				haserrors = 1;
				printerr(filename, linecount, ERROR_LABELSYMBOL, labelname);
			} else if (!isvalidlabel(labelname)) {
				haserrors = 1;
				printerr(filename, linecount, ERROR_LABELINVALIDNAME, labelname);
			} else
				labeldef = 1;
			i += count+1;
			skipwhitespace(line, &i);
			count = countnonwhitespace(line, &i);
			if (count == 0)
				printwarn(filename, linecount, WARNING_LABELEMPTY, labelname);
		} else
			count = countnonwhitespace(line, &i);

		if (count == 0)
			continue;

		opname = strndupl(&line[i-count], count);
		opcode = symbols_get(opname);
		skipwhitespace(line, &i);

		paramamount = symbols_getparamamount(opcode);
		switch (parseparams(line, &i, paramamount, &params)) {
		case PARSER_EUNEXPECTEDSPACE:
			haserrors = 1;
			printerr(filename, linecount, ERROR_PARAMSUNEXPECTEDSPACE);
			continue;
		case PARSER_EUNEXPECTEDCOMMA:
			haserrors = 1;
			printerr(filename, linecount, ERROR_PARAMSUNEXPECTEDCOMMA);
			continue;
		case PARSER_ENOTENOUGHPARAMS:
			haserrors = 1;
			printerr(filename, linecount, paramamount == PARAM_JUMP ? ERROR_PARAMSJUMP : ERROR_PARAMSNOTENOUGH, paramamount);
			continue;
		case PARSER_ETOOMANYPARAMS:
			haserrors = 1;
			printerr(filename, linecount, paramamount == PARAM_JUMP ? ERROR_PARAMSJUMP : ERROR_PARAMSTOOMANY, paramamount);
			continue;
		case PARSER_EJUMPPARAMS:
			haserrors = 1;
			printerr(filename, linecount, ERROR_PARAMSJUMP);
			continue;
		default:
			break;
		}

		if (labeldef) {
			labelattribute = hashmap_getint(labelattributes, labelname);
			if ((isoperation(opname) || isdatadirective(opcode)) && (hashmap_getint(labels, labelname) != NULL || (labelattribute != NULL && *labelattribute & LABEL_EXTERNAL))) {
				haserrors = 1;
				printerr(filename, linecount, ERROR_LABELDEFINED, labelname);
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
						if (!encodenum((char *) params->value, &dataptr, &datacount)) {
							haserrors = 1;
							printerr(filename, linecount, ERROR_DATAINVALIDNUMBER, (char *) params->value);
						}
						params = linkedlist_freenext(params);
					}
				} else {
					switch (encodestring(&line[i], &dataptr, &datacount)) {
					case PARSER_EEXPECTEDQUOTES:
						haserrors = 1;
						printerr(filename, linecount, ERROR_STRINGSTARTQUOTES);
						break;
					case PARSER_EUNFINISHEDSTRING:
						haserrors = 1;
						printerr(filename, linecount, ERROR_STRINGUNFINISHED);
						break;
					case PARSER_EINVALIDCHAR:
						haserrors = 1;
						printerr(filename, linecount, ERROR_STRINGASCII);
						break;
					default:
						break;
					}
				}
			} else {
				if (labeldef)
					printwarn(filename, linecount, WARNING_LABELUSELESS, labelname);
				labelname = strdupl((char *) params->value);
				labelattribute = hashmap_getint(labelattributes, labelname);
				if (!isvalidlabel(labelname)) {
					haserrors = 1;
					printerr(filename, linecount, ERROR_LABELINVALIDNAME, labelname);
				} else if ((opcode == DIRECTIVE_ENTRY || hashmap_getint(labels, labelname) == NULL) && (labelattribute == NULL || !(*labelattribute & (LABEL_ENTRY | LABEL_EXTERNAL))))
					hashmap_addbittofield(labelattributes, labelname, (opcode == DIRECTIVE_EXTERN ? LABEL_EXTERNAL : LABEL_ENTRY));
				else {
					haserrors = 1;
					printerr(filename, linecount, ERROR_LABELDEFINED, labelname);
				}
			}
		} else if (isoperation(opname)) {
			if (labeldef) {
				hashmap_setint(labels, labelname, MEMORY_START + instructioncount);
				hashmap_addbittofield(labelattributes, labelname, LABEL_INSTRUCTION);
			}
			switch (encodeoperation(opname, opcode, linecount, &params, &instructionptr, &instructioncount)) {
			case PARSER_EINVALIDNUMBER:
				printerr(filename, linecount, ERROR_DATAINVALIDNUMBER, (char *) params->value);
				break;
			case PARSER_EINVALIDLABEL:
				printerr(filename, linecount, ERROR_LABELINVALIDNAME, (char *) params->value);
				break;
			case PARSER_EINVALIDSOURCEPARAM:
				printerr(filename, linecount, ERROR_OPINVALIDSOURCE, opname);
				break;
			case PARSER_EINVALIDDESTPARAM:
				printerr(filename, linecount, ERROR_OPINVALIDDEST, opname);
				break;
			default:
				break;
			}
		} else {
			haserrors = 1;
			printerr(filename, linecount, ERROR_UNKNOWNINSTRUCTION, opname);
		}

		if (labeldef)
			free(labelname);
		free(opname);
		linkedlist_free(params);
	}

	for (i = 0; i < HASHMAP_CAPACITY; i++) {
		for (attributesptr = labelattributes->tab[i]; attributesptr != NULL; attributesptr = attributesptr->next) {
			labelattribute = (int *) attributesptr->value;
			if (labelattribute != NULL && *labelattribute & LABEL_DATA)
				hashmap_setint(labels, attributesptr->key, *hashmap_getint(labels, attributesptr->key) + MEMORY_START + instructioncount);
		}
	}

	if (instructioncount + datacount > MEMORY_END-MEMORY_START) {
		haserrors = 1;
		printerr(filename, linecount, ERROR_BINARYOVERFLOW);
	}

	strcat(filename, ".ob");
	ob = open(filename, "w");
	replaceextension(filename, "");

	fprintf(ob, "%d %d\n", instructioncount, datacount);
	if (haserrors) {
		close(ob);
		ob = NULL;
	}
	return ob;
}
