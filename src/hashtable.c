#include "hashtable.h"
#include <stdlib.h>

struct HashTable {
	HashFunc h;
};

struct HashTable*
hashtable_new(HashFunc h)
{
	struct HashTable *ht = malloc(sizeof(struct HashTable));
	if (!ht) {
		return NULL;
	}
	ht->h = h;
	return ht;
}

void
hashtable_free(struct HashTable *ht)
{
	free(ht);
}