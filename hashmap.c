#include "hashmap.h"

#include <stdlib.h>
#include <string.h>
#include "errutil.h"

/* returns a hash of the given string */
unsigned int hash(char *key)
{
	/* adapted from the K&R book */
	unsigned int hashval;
	for (hashval = 0; *key != '\0'; key++)
		hashval = *key + 31 * hashval;
	return hashval % HASHMAP_CAPACITY;
}

struct hashmap *hashmap_new(void)
{
	int i;
	struct hashmap *m = (struct hashmap *) alloc(sizeof(*m));
	m->size = 0;
	/* initialize the `tab` array to avoid seg fault */
	for (i = 0; i < HASHMAP_CAPACITY; i++)
		m->tab[i] = NULL;
	return m;
}

void hashmap_free(struct hashmap *m)
{
	/* adapted from the K&R book */
	struct hashnode *n, *tmp;
	int i;
	if (m == NULL)
		return;
	for (i = 0; i < HASHMAP_CAPACITY; i++) {
		n = m->tab[i];
		while (n != NULL) {
			tmp = n->next;
			free(n->key);
			free(n->value);
			free(n);
			n = tmp;
		}
		m->tab[i] = NULL;
	}
	free(m);
}

struct hashnode *getnode(struct hashmap *m, char *key)
{
	/* adapted from the K&R book */
	struct hashnode *n;
	if (m == NULL)
		return NULL;
	for (n = m->tab[hash(key)]; n != NULL; n = n->next)
		if (strcmp(n->key, key) == 0)
			return n;
	return NULL;
}

int *hashmap_getint(struct hashmap *m, char *key)
{
	struct hashnode *n;
	/* if nothing is NULL then we can return the value, otherwise NULL */
	return m == NULL || (n = getnode(m, key)) == NULL ? NULL : (int *) n->value;
}

char *hashmap_getstr(struct hashmap *m, char *key)
{
	struct hashnode *n;
	/* if nothing is NULL then we can return the value, otherwise NULL */
	return m == NULL || (n = getnode(m, key)) == NULL ? NULL : (char *) n->value;
}

/* prepares a new node and returns it to later have its value set */
struct hashnode *preparenode(struct hashmap *m, char *key)
{
	/* adapted from the K&R book */
	struct hashnode *n;
	unsigned int hashval;
	if (m == NULL)
		return NULL;
	if ((n = getnode(m, key)) == NULL) {
		n = (struct hashnode *) alloc(sizeof(*n));
		n->key = (char *) alloc(sizeof(char) * strlen(key));
		strcpy(n->key, key);
		hashval = hash(key);
		n->next = m->tab[hashval];
		m->tab[hashval] = n;
		m->size++;
	} else
		free(n->value);
	return n;
}

void *hashmap_setint(struct hashmap *m, char *key, int value)
{
	struct hashnode *n;
	if (m == NULL || (n = preparenode(m, key)) == NULL)
		return NULL;
	n->value = alloc(sizeof(value));
	*((int *) n->value) = value;
	return n->value;
}

void *hashmap_setstr(struct hashmap *m, char *key, char *value)
{
	struct hashnode *n;
	if (m == NULL || (n = preparenode(m, key)) == NULL)
		return NULL;
	n->value = alloc(sizeof(char) * strlen(value));
	strcpy(n->value, value);
	return n->value;
}

void hashmap_addbittofield(struct hashmap *m, char *key, int value)
{
	int *n;
	if (m == NULL)
		return;
	/* since we are trying to *add* to a bitfield, try to get its existing value */
	n = hashmap_getint(m, key);
	/* if there is no existing value, then assign it to the given `value` parameter, otherwise add it to the existing one */
	hashmap_setint(m, key, n == NULL ? value : ((*n) | value));
}
