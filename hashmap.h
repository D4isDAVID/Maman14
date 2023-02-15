#ifndef HASHMAP_H
#define HASHMAP_H

struct hashmap;

struct hashmap *hashmap_new(void);
int *hashmap_getint(struct hashmap *, char *);
char *hashmap_getstr(struct hashmap *, char *);
void *hashmap_setint(struct hashmap *, char *, int);
void *hashmap_setstr(struct hashmap *, char *, char *);
void hashmap_free(struct hashmap *);

#endif