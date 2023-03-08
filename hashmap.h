#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_CAPACITY 255

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
	int size;
};

struct hashmap *hashmap_new(void);
void hashmap_free(struct hashmap *);
int *hashmap_getint(struct hashmap *, char *);
char *hashmap_getstr(struct hashmap *, char *);
void *hashmap_setint(struct hashmap *, char *, int);
void *hashmap_setstr(struct hashmap *, char *, char *);
void hashmap_addbittofield(struct hashmap *, char *, int);

#endif
