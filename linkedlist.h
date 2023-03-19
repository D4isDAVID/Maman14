/* linkedlist nodes implementation for saving program data */
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct listnode {
	struct listnode *next;
	void *value;
	void (*free)(void *);
};

/* creates a new list node with the given value.
	it is assumed that the value can be `free()`'d */
struct listnode *linkedlist_newnode(void *, void (*)(void *));
/* frees the current node in the list and returns the next one */
struct listnode *linkedlist_freenext(struct listnode *);
/* frees the entire list */
void linkedlist_free(struct listnode *);

#endif
