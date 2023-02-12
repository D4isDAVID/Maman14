#include <stdio.h>
#include "preassembler.h"

int main(int argc, char **argv) {
	int i;
	FILE *as, *am;
	if (argc == 1) {
		fprintf(stderr, "Error: no files mentioned\n");
		return 1;
	}
	for (i = 1; i < argc; i++) {
		as = fopen(argv[i], "r");
		if (as == NULL) {
			fprintf(stderr, "Error in opening %s.as\n", argv[i]);
			return 1;
		}
		am = preassembler(as, argv[i]);
		fclose(as);
	}
	return 0;
}
