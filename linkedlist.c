#include "linkedlist.h"

#include <stdlib.h>
#include "errutil.h"

struct listnode *linkedlist_newnode(void *value, void (*mfree)(void *))
{
	struct listnode *n = (struct listnode *) alloc(sizeof(*n));
	n->value = value;
	n->next = NULL;
	n->free = mfree;
	return n;
}

struct listnode *linkedlist_freenext(struct listnode *n)
{
	struct listnode *tmp;
	if (n == NULL)
		return NULL;
	tmp = n->next;
	n->free(n->value);
	free(n);
	return tmp;
}

void linkedlist_free(struct listnode *n)
{
	while (n != NULL)
		n = linkedlist_freenext(n);
}
