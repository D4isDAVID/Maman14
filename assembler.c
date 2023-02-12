#include <stdio.h>
#include <stdlib.h>
#include "preassembler.h"

int main(int argc, char **argv) {
	int i;
	FILE *file, *temp;
	if (argc == 1) {
		fprintf(stderr, "Error: no files mentioned\n");
		return 1;
	}
	for (i = 0; i+1 < argc; i++) {
		temp = fopen(argv[i+1], "r");
		if (temp == NULL) {
			fprintf(stderr, "error in opening %s.as\n", argv[i+1]);
			exit(1);
		}
		file = preassembler(temp, argv[i+1]);
		fclose(temp);
	}
	return 0;
}
