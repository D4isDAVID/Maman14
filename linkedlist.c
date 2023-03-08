/* linkedlist nodes implementation for saving program data */
#include "linkedlist.h"

#include <stdlib.h>

/* creates a new list node with the given value */
struct listnode *linkedlist_newnode(void *value)
{
	struct listnode *n = (struct listnode *) malloc(sizeof(*n));
	n->value = value;
	n->next = NULL;
	return n;
}

/* frees the current node in the list and returns the next one */
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

/* frees the entire list */
void linkedlist_free(struct listnode *n)
{
	while (n != NULL)
		n = linkedlist_freenext(n);
}
