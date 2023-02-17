/* assembler entry point */

#include <stdio.h>
#include <string.h>
#include "preassembler.h"
#include "parser.h"
#include "firstphase.h"
#include "strutil.h"
#include "hashmap.h"

void deleteoutputfiles(char *);

int main(int argc, char **argv)
{
	int i;
	FILE *as, *am, *ob;
	struct hashmap *labels, *entext;
	if (argc == 1) {
		fprintf(stderr, "Error: no files mentioned\n");
		return 1;
	}
	symbols_prepare();
	for (i = 1; i < argc; i++) {
		strcat(argv[i], ".as");
		as = fopen(argv[i], "r");
		if (as == NULL) {
			fprintf(stderr, "Error in opening %s\n", argv[i]);
			return 1;
		}
		replaceextension(argv[i], "");
		am = preassembler(as, argv[i]);
		fclose(as);
		ob = firstphase(am, argv[i], &labels, &entext);
		fclose(am);
		if (ob == NULL)
			deleteoutputfiles(argv[i]);
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
