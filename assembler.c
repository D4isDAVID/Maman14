/* assembler entry point */

#include <stdio.h>
#include <string.h>
#include "preassembler.h"
#include "parser.h"
#include "firstphase.h"
#include "strutil.h"
#include "hashmap.h"
#include "linkedlist.h"
#include "errutil.h"

void deleteoutputfiles(char *);

int main(int argc, char **argv)
{
	int i;
	char *filename;
	FILE *as, *am, *ob;
	struct hashmap *labels, *labelattributes;
	struct listnode *instructions = linkedlist_newnode(0), *data = linkedlist_newnode(0); /* garbage 0 values to start the list */
	if (argc == 1) {
		fprintf(stderr, "Error: no files mentioned\n");
		return 1;
	}
	symbols_prepare();
	errutil_prepare();
	for (i = 1; i < argc; i++) {
		filename = malloc(sizeof(char) * (strlen(argv[i]) + 5)); /* enough space for filename + extension + null terminator */
		strcpy(filename, argv[i]);
		strcat(filename, ".as");
		as = fopen(filename, "r");
		if (as == NULL) {
			fprintf(stderr, "Error in opening %s\n", filename);
			return 1;
		}
		replaceextension(filename, "");
		am = preassembler(as, filename);
		fclose(as);
		ob = firstphase(am, filename, &instructions, &data, &labels, &labelattributes);
		fclose(am);
		instructions = instructions->next; /* skip garbage value */
		data = data->next;
		if (ob == NULL)
			deleteoutputfiles(filename);
		linkedlist_free(instructions);
		linkedlist_free(data);
		hashmap_free(labels);
		hashmap_free(labelattributes);
	}
	symbols_free();
	errutil_free();
	return 0;
}

void deleteoutputfiles(char *filename)
{
	strcat(filename, ".am");
	remove(filename);
	replaceextension(filename, ".ent");
	remove(filename);
	replaceextension(filename, ".ext");
	remove(filename);
	replaceextension(filename, ".ob");
	remove(filename);
}
