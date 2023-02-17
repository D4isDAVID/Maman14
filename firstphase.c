/* first assembler phase */
#include "firstphase.h"

#include <string.h>
#include "hashmap.h"
#include "parser.h"

/* phase entry point */
FILE *firstphase(FILE *am, char *filename, struct hashmap **labels, struct hashmap **entext)
{
	char line[MAX_LINE_LENGTH + 2];
	int i,
		count;
	struct hashmap *labels = hashmap_new();
}
