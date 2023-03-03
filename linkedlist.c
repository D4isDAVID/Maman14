/* linkedlist nodes implementation for saving program data */
#include "linkedlist.h"

#include <stdlib.h>

struct listnode *linkedlist_newnode(void *value)
{
	struct listnode *n = (struct listnode *) malloc(sizeof(*n));
	n->value = value;
	return n;
}

void linkedlist_freenode(struct listnode *n)
{
	if (n == NULL)
		return;
	free(n->value);
	free(n);
}

void linkedlist_free(struct listnode *n)
{
	struct listnode *tmp;
	while (n != NULL) {
		tmp = n->next;
		linkedlist_freenode(n);
		n = tmp;
	}
}
