#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "parser.h"

struct listnode {
	struct listnode *next;
	void *value;
};

struct listnode *linkedlist_newnode(void *);
void linkedlist_freenode(struct listnode *);
void linkedlist_free(struct listnode *);

#endif
