#include "secondphase.h"

#include <string.h>
#include "parser.h"
#include "strutil.h"
#include "errutil.h"

/* receives an int and prints its bits into the given file (0s and 1s represented with `.` and `/` respectively) */
void encode(unsigned int num, FILE *ob);

int secondphase(FILE *am, FILE *ob, char *filename, struct listnode *instructions, struct listnode *data, struct hashmap *labels, struct hashmap *labelattributes)
{
	struct listnode *listptr = instructions->next; /* pointer to both the `instructions` and `data` lists */
	char *labelname, /* the name of the current label */
		line[MAX_LINE_LENGTH + 2], /* the current line in the code */
		*op; /* the operation in the current line */
	int *labelattribute, /* a value from `labelattributes` */
		*labelvalue, /* a value from `labels` */
		binarycount = MEMORY_START, /* the number of the word (14 bits) currently being printed */
		linecount = 0, /* the current line in the source code */
		haserrors = 0, /* whether errors have been found */
		isdata = 0, /* whether `listptr` is pointing to `data` or not */
		hasext = 0, /* whether the code uses externals */
		hasent = 0, /* whether the code defines entries */
		i, /* the current character in the current line */
		count; /* results from `skipwhitespace` and `countnonwhitespace` */
	FILE *ent, *ext; /* the `.ent` and `.ext` files */
	instruction *inst; /* the value of `listptr`, if it's pointing to `instructions` */

	strcat(filename, ".ent");
	ent = open(filename, "w");
	replaceextension(filename, ".ext");
	ext = open(filename, "w");
	replaceextension(filename, "");

	/* ensure valid `.entry` statements */
	while (fgets(line, MAX_LINE_LENGTH + 2, am)) {
		linecount++;
		i = 0;

		/* we can skip anything not regarding `.entry` */
		if ((op = strstr(line, ".entry")) == NULL)
			continue;

		/* skip from `.entry` to the label name */
		countnonwhitespace(op, &i);
		skipwhitespace(op, &i);

		/* copy the label name and its attributes */
		count = countnonwhitespace(op, &i);
		labelname = strndupl(&op[i-count], count);
		labelattribute = hashmap_getint(labelattributes, labelname);

		/* we can ignore this if the label isn't defined as an entry */
		if (labelattribute != NULL && *labelattribute & LABEL_ENTRY) {
			/* this is valid if the label is defined as data or an instruction */
			if (*labelattribute & (LABEL_DATA | LABEL_INSTRUCTION)) {
				hasent = 1;
				fprintf(ent,"%s\t%d\n", labelname, *hashmap_getint(labels, labelname));
			} else {
				haserrors = 1;
				printerr(filename, linecount, ERROR_LABELNOTDEFINED, labelname);
			}
		}

		free(labelname);
	}

	/* ensure valid label usages and also output binary into the `.ob` file */
	linecount = 0;
	while (listptr != NULL) {
		/* output the binary count */
		fprintf(ob, "0%d\t", binarycount);

		/* if `listptr` is pointing to an instruction */
		if (!isdata) {
			labelname = NULL;
			inst = listptr->value;

			/* save the linecount if the current word has one associated with it */
			if (inst->line)
				linecount = inst->line;

			/* if this is a label then check its validity */
			if (inst->islabel) {
				/* copy over the label name, value, and attributes */
				labelname = (char *) inst->value;
				labelattribute = hashmap_getint(labelattributes, labelname);
				labelvalue = hashmap_getint(labels, labelname);

				/* this is invalid if the label doesn't have a value (externals don't have one) and isn't an external */
				if (labelvalue == NULL && (labelattribute == NULL || !(*labelattribute & LABEL_EXTERNAL))) {
					haserrors = 1;
					printerr(filename, linecount, ERROR_LABELNOTDEFINED, labelname);
				}

				/* ensure that `labelattribute` isn't NULL for some reason */
				if (labelattribute != NULL) {
					/* we can simply output `............./` if this is an external */
					if (*labelattribute & LABEL_EXTERNAL) {
						encode(ENC_EXTERNAL, ob);
						hasext = 1;
						fprintf(ext,"%s\t%d\n", labelname, binarycount);
					} else /* otherwise encode the label */
						encode(encodelabel(*labelvalue), ob);
				}
			} else /* otherwise this is a word and can be simply printed */
				encode(((word *)inst->value)->field, ob);

			/* if there are no more instructions then loop over `data` */
			if (!listptr->next) {
				isdata = 1;
				listptr = data;
			}
		} else /* otherwise this is data */
			encode(((word *) listptr->value)->field, ob);

		fputc('\n', ob);

		binarycount++;
		listptr = listptr->next;
	}

	close(ob);
	close(ent);
	close(ext);

	if (!hasent) {
		strcat(filename, ".ent");
		remove(filename);
		replaceextension(filename, "");
	}

	if (!hasext) {
		strcat(filename, ".ext");
		remove(filename);
		replaceextension(filename, "");
	}

	return haserrors;
}

void encode(unsigned int num, FILE *ob)
{
	int i;
	for (i = 1 << (WORD_LENGTH-1); i > 0; i /= 2)
		fputc((num & i) ? '/' : '.', ob);
}
