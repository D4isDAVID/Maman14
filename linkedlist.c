/* linkedlist nodes implementation for saving program data */
#include "linkedlist.h"

#include <stdlib.h>

struct listnode *linkedlist_newnode(void *value)
{
	struct listnode *n = (struct listnode *) malloc(sizeof(*n));
	n->value = value;
	n->next = NULL;
	return n;
}

struct listnode *linkedlist_freenext(struct listnode *n)
{
	struct listnode *tmp;
	if (n == NULL)
		return NULL;
	tmp = n->next;
	free(n->value);
	free(n);
	return tmp;
}

void linkedlist_free(struct listnode *n)
{
	while (n != NULL)
		n = linkedlist_freenext(n);
}
