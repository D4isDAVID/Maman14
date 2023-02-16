/* first assembler phase */
#include "firstphase.h"

#include <string.h>
#include "hashmap.h"
#include "parser.h"

typedef struct word {
	unsigned int field : 14;
} word;

/* phase entry point */
void firstphase(FILE *am)
{
	char line[MAX_LINE_LENGTH + 2];
	int i,
		count;
	struct hashmap *labels = hashmap_new();
}
