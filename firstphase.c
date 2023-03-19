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
		labeldef, /* whether the current line has a label definition */
		paramamount, /* parameter amount of current operation */
		*labelattribute, /* hashmap values of labelattributes */
		linecount = 0, /* count of lines */
		datacount = 0, /* count of data instructions */
		instructioncount = 0, /* count of all instructions */
		toolong = 0, /* whether the current line is too long */
		haserrors = 0; /* whether an error has been detected somewhere in the file */
	enum symbol opcode; /* operation code in current line */
	struct listnode *params, /* parameters of the current operation */
		*instructionptr = instructions, /* pointer to the final node in the `instructions` list */
		*dataptr = data; /* pointer to the final node in the `data` list */
	struct hashnode *attributesptr; /* pointer to a node in the `labelattributes` hashmap */
	FILE *ob; /* the `.ob` file */

	while (fgets(line, MAX_LINE_LENGTH + 2, am) != NULL) {
		linecount++;
		toolong = 0;

		/* if we didn't encounter a newline and there are too much characters then the line is too long */
		while (strchr(line, '\n') == NULL && strlen(line) > MAX_LINE_LENGTH) {
			if (!toolong) {
				/* print the message */
				printerr(filename, linecount, ERROR_LINEOVERFLOW);
				toolong = 1;
				haserrors = 1;
			}
			/* wait until the line ends */
			if (fgets(line, MAX_LINE_LENGTH + 2, am) == NULL)
				break;
		}
		if (toolong)
			continue;

		i = 0;
		labeldef = 0;
		labelname = NULL;
		params = NULL;

		skipwhitespace(line, &i);
		count = countuntil(&line[i], ':');

		/* if we have a lable definition */
		if (line[i+count] == ':') {
			labelname = strndupl(&line[i], count);
			/* label names can't be the same as a built-in constant */
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

		/* check for duplicate label definitions (checks for `.entry` and `.extern` are done later) */
		if (labeldef && (isoperation(opname) || isdatadirective(opcode))) {
			labelattribute = hashmap_getint(labelattributes, labelname);
			/* if the current label is already defined or is already an external, then it is a duplicate */
			if (hashmap_getint(labels, labelname) != NULL || (labelattribute != NULL && *labelattribute & LABEL_EXTERNAL)) {
				haserrors = 1;
				printerr(filename, linecount, ERROR_LABELDEFINED, labelname);
			}
		}

		/* `.string`, `.data`, `.entry`, `.extern` */
		if (isdirective(opname)) {

			if (isdatadirective(opcode)) { /* `.string`, `.data` */
				if (labeldef) {
					/* store this as a data label */
					hashmap_setint(labels, labelname, datacount);
					hashmap_addbittofield(labelattributes, labelname, LABEL_DATA);
				}

				if (opcode == DIRECTIVE_DATA) { /* `.data` */
					while (params != NULL) {
						if (!encodenum((char *) params->value, &dataptr, &datacount)) {
							haserrors = 1;
							printerr(filename, linecount, ERROR_DATAINVALIDNUMBER, (char *) params->value);
						}
						params = linkedlist_freenext(params);
					}
				} else { /* `.string` */
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
			} else { /* `.entry`, `.extern` */
				if (labeldef) {
					printwarn(filename, linecount, WARNING_LABELUSELESS, labelname);
					free(labelname);
					labeldef = 0;
				}

				labelname = strdupl((char *) params->value);
				labelattribute = hashmap_getint(labelattributes, labelname);
				if (!isvalidlabel(labelname)) {
					haserrors = 1;
					printerr(filename, linecount, ERROR_LABELINVALIDNAME, labelname);
				/* check for duplicate entries and externals. this is not a duplicate if:
					1. this is an entry definition or the label isn't defined (we can't define an already defined label as an external, but we can define them as entries)
					2. label attributes either aren't stored for this label (they are stored only when defined), and if they are then the label isn't an entry and isn't an external */
				} else if ((opcode == DIRECTIVE_ENTRY || hashmap_getint(labels, labelname) == NULL) && (labelattribute == NULL || !(*labelattribute & (LABEL_ENTRY | LABEL_EXTERNAL))))
					/* store this as an entry or external, depending on the opcode */
					hashmap_addbittofield(labelattributes, labelname, (opcode == DIRECTIVE_EXTERN ? LABEL_EXTERNAL : LABEL_ENTRY));
				else {
					haserrors = 1;
					printerr(filename, linecount, ERROR_LABELDEFINED, labelname);
				}
				free(labelname);
			}

		} else if (isoperation(opname)) {
			if (labeldef) {
				/* store this as an instruction label */
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
			/* here the given instruction isn't defined */
			haserrors = 1;
			printerr(filename, linecount, ERROR_UNKNOWNINSTRUCTION, opname);
		}

		if (labeldef)
			free(labelname);
		free(opname);
		linkedlist_free(params);
	}

	/* add the instruction counter to all data labels */
	for (i = 0; i < HASHMAP_CAPACITY; i++) {
		for (attributesptr = labelattributes->tab[i]; attributesptr != NULL; attributesptr = attributesptr->next) {
			labelattribute = (int *) attributesptr->value;
			if (labelattribute != NULL && *labelattribute & LABEL_DATA)
				hashmap_setint(labels, attributesptr->key, *hashmap_getint(labels, attributesptr->key) + MEMORY_START + instructioncount);
		}
	}

	/* ensure that the program isn't using too much memory */
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
