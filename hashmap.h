/* hashmap implementation, adapted from the K&R book */
#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_CAPACITY 255

struct hashnode {
	struct hashnode *next;
	char *key;
	void *value;
};

struct hashmap {
	struct hashnode *tab[HASHMAP_CAPACITY];
	int size;
};

/* creates a new empty hashmap */
struct hashmap *hashmap_new(void);
/* frees the entire hashmap */
void hashmap_free(struct hashmap *);
int *hashmap_getint(struct hashmap *, char *);
char *hashmap_getstr(struct hashmap *, char *);
void *hashmap_setint(struct hashmap *, char *, int);
void *hashmap_setstr(struct hashmap *, char *, char *);
/* utility function for bitfields */
void hashmap_addbittofield(struct hashmap *, char *, int);

#endif
