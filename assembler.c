#include <stdio.h>
#include <string.h>
#include "preassembler.c"

int main(int argc, char **argv) {
	int i;
	FILE *as, *am;
	if (argc == 1) {
		fprintf(stderr, "error: no files mentioned\n");
		return 1;
	}
	for (i = 1; i < argc; i++) {
		strcat(argv[i], ".as"); /* the filename is given without an extension */
		as = fopen(argv[i], "r");
		if (as == NULL) {
			fprintf(stderr, "error in opening %s\n", argv[i]);
			return 1;
		}
		*strrchr(argv[i], '.') = '\0'; /* the following operations will need a different file extension */
		am = preassembler(as, argv[i]);
		fclose(as);
	}
	return 0;
}
