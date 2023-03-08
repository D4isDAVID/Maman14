/* hashmap implementation, adapted from the K&R book */
#include "hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int hash(char *key)
{
	unsigned int hashval;
	for (hashval = 0; *key != '\0'; key++)
		hashval = *key + 31 * hashval;
	return hashval % HASHMAP_CAPACITY;
}

struct hashmap *hashmap_new(void)
{
	struct hashmap *m = (struct hashmap *) malloc(sizeof(*m));
	m->size = 0;
	return m;
}

void hashmap_free(struct hashmap *m)
{
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
	}
	free(m);
}

void hashmap_copy(struct hashmap *dest, struct hashmap *src)
{
	int i;
	struct hashnode *n, *tmp;
	if (dest == NULL || src == NULL)
		return;
	for (i = 0; i < HASHMAP_CAPACITY; i++) {
		n = src->tab[i];
		while (n != NULL) {
			tmp = n->next;
			switch (n->type) {
			case HASHMAP_VAL_INT:
				hashmap_setint(dest, n->key, *((int *) (n->value)));
				break;
			case HASHMAP_VAL_STR:
				hashmap_setstr(dest, n->key, n->value);
				break;
			}
			n = tmp;
		}
	}
}

struct hashnode *getnode(struct hashmap *m, char *key)
{
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
	return m == NULL || (n = getnode(m, key)) == NULL || n->type != HASHMAP_VAL_INT ? NULL : (int *) n->value;
}

char *hashmap_getstr(struct hashmap *m, char *key)
{
	struct hashnode *n;
	return m == NULL || (n = getnode(m, key)) == NULL || n->type != HASHMAP_VAL_STR ? NULL : (char *) n->value;
}

struct hashnode *preparenode(struct hashmap *m, char *key)
{
	struct hashnode *n;
	unsigned int hashval;
	if (m == NULL)
		return NULL;
	if ((n = getnode(m, key)) == NULL) {
		n = (struct hashnode *) malloc(sizeof(*n));
		if (n == NULL) {
			fprintf(stderr, "error: failed to allocate memory for hashnode");
			exit(1);
		}
		n->key = (char *) malloc(sizeof(char) * strlen(key));
		if (n->key == NULL) {
			fprintf(stderr, "error: failed to allocate memory for hashnode value");
			exit(1);
		}
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
	struct hashnode *n = preparenode(m, key);
	if (m == NULL || n == NULL)
		return NULL;
	n->value = malloc(sizeof(value));
	*((int *) n->value) = value;
	n->type = HASHMAP_VAL_INT;
	return n->value;
}

void *hashmap_setstr(struct hashmap *m, char *key, char *value)
{
	struct hashnode *n = preparenode(m, key);
	if (m == NULL || n == NULL)
		return NULL;
	n->value = malloc(sizeof(char) * strlen(value));
	strcpy(n->value, value);
	n->type = HASHMAP_VAL_STR;
	return n->value;
}

void hashmap_addbittofield(struct hashmap *m, char *key, int value)
{
	int *n;
	if (m == NULL)
		return;
	n = hashmap_getint(m, key);
	hashmap_setint(m, key, n == NULL ? value : ((*n) | value));
}
