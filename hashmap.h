/* hashmap implementation, adapted from the K&R book.
	setters/getters for integers and strings are implemented seperately because the strings/integers are copied to a completely new variable.
	this means that the way integers and strings have memory allocated for them must be different, thus different setters are implemented.
	the getters have no difference and are a utility alternative to using casting. */
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
/* returns the value of the string key as an int */
int *hashmap_getint(struct hashmap *, char *);
/* returns the value of the string key as a string */
char *hashmap_getstr(struct hashmap *, char *);
/* assigns the given int value to the string key */
void *hashmap_setint(struct hashmap *, char *, int);
/* assigns the given string value to the string key */
void *hashmap_setstr(struct hashmap *, char *k, char *v);
/* utility function for bitfields */
void hashmap_addbittofield(struct hashmap *, char *, int);

#endif
