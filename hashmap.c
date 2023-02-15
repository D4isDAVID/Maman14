/* hashmap implementation, adapted from the K&R book */
#include "hashmap.h"

#include <stdlib.h>
#include <string.h>

#define HASHMAP_CAPACITY 255

unsigned int hash(char *key)
{
	unsigned int hashval;
	for (hashval = 0; *key != '\0'; key++)
		hashval = *key + 31 * hashval;
	return hashval % HASHMAP_CAPACITY;
}

enum hashmap_valuetype {
	HASHMAP_VAL_INT,
	HASHMAP_VAL_STR
};

struct hashnode {
	struct hashnode *next;
	char *key;
	void *value;
	enum hashmap_valuetype type;
};

struct hashmap {
	struct hashnode *tab[HASHMAP_CAPACITY];
};

struct hashmap *hashmap_new(void)
{
	struct hashmap *m = (struct hashmap *) malloc(sizeof(*m));
	return m;
}

struct hashnode *getnode(struct hashmap *m, char *key)
{
	struct hashnode *n;
	for (n = m->tab[hash(key)]; n != NULL; n = n->next)
		if (strcmp(n->key, key) == 0)
			return n;
	return NULL;
}

int *hashmap_getint(struct hashmap *m, char *key)
{
	struct hashnode *n;
	return (n = getnode(m, key)) == NULL || n->type != HASHMAP_VAL_INT ? NULL : (int *) n->value;
}

char *hashmap_getstr(struct hashmap *m, char *key)
{
	struct hashnode *n;
	return (n = getnode(m, key)) == NULL || n->type != HASHMAP_VAL_STR ? NULL : (char *) n->value;
}

struct hashnode *preparenode(struct hashmap *m, char *key)
{
	struct hashnode *n;
	unsigned int hashval;
	if ((n = getnode(m, key)) == NULL) {
		n = (struct hashnode *) malloc(sizeof(*n));
		if (n == NULL)
			return NULL;
		n->key = (char *) malloc(sizeof(char) * strlen(key));
		if (n->key == NULL)
			return NULL;
		strcpy(n->key, key);
		hashval = hash(key);
		n->next = m->tab[hashval];
		m->tab[hashval] = n;
	} else
		free(n->value);
	return n;
}

void *hashmap_setint(struct hashmap *m, char *key, int value)
{
	struct hashnode *n = preparenode(m, key);
	if (n == NULL)
		return NULL;
	n->value = malloc(sizeof(value));
	*((int *) n->value) = value;
	n->type = HASHMAP_VAL_INT;
	return n->value;
}

void *hashmap_setstr(struct hashmap *m, char *key, char *value)
{
	struct hashnode *n = preparenode(m, key);
	if (n == NULL)
		return NULL;
	n->value = malloc(sizeof(char) * strlen(value));
	strcpy(n->value, value);
	n->type = HASHMAP_VAL_STR;
	return n->value;
}

void hashmap_free(struct hashmap *m)
{
	struct hashnode *n, *tmp;
	int i;
	for (i = 0; i < HASHMAP_CAPACITY; i++) {
		n = m->tab[i];
		while (n != NULL) {
			tmp = n->next;
			free(n->key);
			free(n->value);
			free(n);
			n = tmp;
		}
	}
	free(m);
}
