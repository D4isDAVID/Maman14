#include <stdio.h>
#include "preassembler.h"

int main(int argc, char **argv) {
	int i;
	FILE *file, *temp;
	if (argc == 1) {
		fprintf(stderr, "Error: no files mentioned\n");
		return 1;
	}
	for (i = 1; i < argc; i++) {
		temp = fopen(argv[i], "r");
		if (temp == NULL) {
			fprintf(stderr, "Error in opening %s.as\n", argv[i]);
			return 1;
		}
		file = preassembler(temp, argv[i]);
		fclose(temp);
	}
	return 0;
}
