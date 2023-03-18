/* first assembler phase: garners any information it can about the code (instructions, data, labels) to later be used in the second phase
	everything aside from labels is encoded in this phase */
#ifndef FIRSTPHASE_H
#define FIRSTPHASE_H

#include <stdio.h>
#include "hashmap.h"
#include "linkedlist.h"

/* phase entry point */
FILE *firstphase(FILE *, char *, struct listnode *, struct listnode *, struct hashmap *, struct hashmap *);

#endif
