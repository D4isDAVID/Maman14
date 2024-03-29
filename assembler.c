/* assembler entry point */

#include <stdio.h>
#include <string.h>
#include "symbols.h"
#include "preassembler.h"
#include "firstphase.h"
#include "secondphase.h"
#include "hashmap.h"
#include "linkedlist.h"
#include "strutil.h"
#include "errutil.h"

int main(int argc, char **argv)
{
	int i; /* for looping over arguments */
	char *filename; /* current file name */
	FILE *as, *am, *ob; /* .as, .am, and .ob files of the current file */
	struct hashmap *labels, *labelattributes; /* labels and their attributes in the current file */
	struct listnode *instructions, *data; /* instructions and data of the current file */

	if (argc == 1) {
		printf("error: no files mentioned\n");
		return 1;
	}

	/* initial memory allocation for utilities that are used globally across all files */
	symbols_prepare();
	errutil_prepare();

	for (i = 1; i < argc; i++) {
		filename = alloc(sizeof(char) * (strlen(argv[i]) + 4 + 1)); /* enough space for filename + extension (max 4) + null terminator (1) */
		strcpy(filename, argv[i]);
		strcat(filename, ".as");
		as = open(filename, "r");

		replaceextension(filename, ""); /* preassembler needs filename without extension */
		am = preassembler(as, filename);
		close(as); /* we won't need this file anymore */

		labels = hashmap_new();
		labelattributes = hashmap_new();
		instructions = linkedlist_newnode(NULL, free); /* garbage value to start the list */
		data = linkedlist_newnode(NULL, free);

		ob = firstphase(am, filename, instructions, data, labels, labelattributes);

		fseek(am, 0, SEEK_SET);
		/* `ob` will be NULL if any errors are found in the first phase.
			`secondphase` will whether it found any errors.
			`close` should never return `EOF` but is included in the condition to reduce code duplication */
		if ((ob == NULL || secondphase(am, ob, filename, instructions, data, labels, labelattributes)) && close(am) != EOF) {
			strcat(filename, ".ent");
			remove(filename);
			replaceextension(filename, ".ext");
			remove(filename);
			replaceextension(filename, ".ob");
			remove(filename);
		}

		hashmap_free(labels);
		hashmap_free(labelattributes);
		linkedlist_free(instructions);
		linkedlist_free(data);
		free(filename);
	}

	symbols_free();
	errutil_free();
	return 0;
}
