/* uses the information gathered in the first phase to print out the binary file
	labels get encoded in this phase */
#ifndef SECONDPHASE_H
#define SECONDPHASE_H

#include <stdio.h>
#include "hashmap.h"
#include "linkedlist.h"

/* phase entry point */
int secondphase(FILE *ob, char *filename, struct listnode *instructions, struct listnode *data, struct hashmap *labels, struct hashmap *labelattributes);

#endif
