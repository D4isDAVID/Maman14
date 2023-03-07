#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct listnode {
	struct listnode *next;
	void *value;
};

struct listnode *linkedlist_newnode(void *);
struct listnode *linkedlist_freenext(struct listnode *);
void linkedlist_free(struct listnode *);

#endif
