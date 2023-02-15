/* */
#include "firstpass.h"

#include <string.h>
#include "hashmap.h"
#include "parser.h"

typedef struct word {
	unsigned int field : 14;
} word;

void firstpass(FILE *am)
{
	char line[MAX_LINE_LENGTH + 2];
	int i,
		count;
	struct hashmap *labels = hashmap_new();
}
