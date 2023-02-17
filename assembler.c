/* assembler entry point */

#include <stdio.h>
#include <string.h>
#include "preassembler.h"
#include "parser.h"
#include "firstphase.h"

int main(int argc, char **argv)
{
	int i;
	FILE *as, *am;
	if (argc == 1) {
		fprintf(stderr, "Error: no files mentioned\n");
		return 1;
	}
	for (i = 1; i < argc; i++) {
		strcat(argv[i], ".as");
		as = fopen(argv[i], "r");
		if (as == NULL) {
			fprintf(stderr, "Error in opening %s\n", argv[i]);
			return 1;
		}
		*strrchr(argv[i], '.') = '\0'; /* the file extension is no longer needed */
		am = preassembler(as, argv[i]);
		fclose(as);
		firstphase(am);
		fclose(am);
		if (isfileempty(am)) {
			strcat(argv[i], ".am");
			remove(argv[i]);
			continue;
		}
	}
	return 0;
}
