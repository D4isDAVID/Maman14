/* preassembler macro list */
#include <stdlib.h>
#include <string.h>
#include "mlist.h"

struct mlist {
	struct mlist *next;
	char *name;
	char *content;
};

static struct mlist *head = NULL;

void mlist_add(char *name, char *content) {
	struct mlist *temp = (struct mlist *) malloc(sizeof(struct mlist));
	temp->name = (char *) malloc(sizeof(char) * strlen(name));
	temp->name = strcpy(temp->name, name);
	temp->content = (char *) malloc(sizeof(char) * strlen(content));
	temp->content = strcpy(temp->content, content);
	temp->next = head;
	head = temp;
}

char *mlist_lookup(char *s) {
	struct mlist *np = head;
	while (np != NULL) {
		if (strcmp(s, np->name) == 0)
			return np->content;
		np = np->next;
	}
	return NULL;
}

void mlist_clear() {
	struct mlist *temp;
	while (head != NULL) {
		temp = head->next;
		free(head);
		head = temp;
	}
}
