/* preassembler macro list */
#include "mlist.h"

#include <stdlib.h>
#include <string.h>

struct mlist {
	struct mlist *next;
	char *name;
	char *content;
};

static struct mlist *head = NULL;

/* adds a new item into the list */
void mlist_add(char *name, char *content)
{
	struct mlist *temp = (struct mlist *) malloc(sizeof(temp));
	temp->name = (char *) malloc(sizeof(char) * strlen(name));
	strcpy(temp->name, name);
	temp->content = (char *) malloc(sizeof(char) * strlen(content));
	strcpy(temp->content, content);
	temp->next = head;
	head = temp;
}

/* searches for an item in the list with the given name and returns the associated value.
	returns `NULL` if an item was not found */
char *mlist_lookup(char *name)
{
	struct mlist *np = head;
	while (np != NULL) {
		if (strcmp(name, np->name) == 0)
			return np->content;
		np = np->next;
	}
	return NULL;
}

/* frees all items in the entire list */
void mlist_clear()
{
	struct mlist *temp;
	while (head != NULL) {
		temp = head->next;
		free(head->name);
		free(head->content);
		free(head);
		head = temp;
	}
}
