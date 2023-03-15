#ifndef SECONDPHASE_H
#define SECONDPHASE_H

#include <stdio.h>
#include "hashmap.h"
#include "linkedlist.h"

int secondphase(FILE *ob, char *filename, struct listnode *instructions, struct listnode *data, struct hashmap *labels, struct hashmap *labelattributes);

#endif
