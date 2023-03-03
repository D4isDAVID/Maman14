/* assembler entry point */

#include <stdio.h>
#include <string.h>
#include "preassembler.h"
#include "parser.h"
#include "firstphase.h"
#include "strutil.h"
#include "hashmap.h"
#include "linkedlist.h"

void deleteoutputfiles(char *);

int main(int argc, char **argv)
{
	int i;
	char *filename;
	FILE *as, *am, *ob;
	struct listnode *instructions, *data;
	struct hashmap *labels, *labelattributes;
	if (argc == 1) {
		fprintf(stderr, "Error: no files mentioned\n");
		return 1;
	}
	symbols_prepare();
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
		if (ob == NULL) {
			deleteoutputfiles(filename);
		}
		linkedlist_free(instructions);
		linkedlist_free(data);
		hashmap_free(labels);
		hashmap_free(labelattributes);
	}
	symbols_free();
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
