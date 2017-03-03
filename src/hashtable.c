#include "hashtable.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define LOAD_FACTOR_LIMIT 0.5

// defined in murmurhash.c
void
murmurhash3_x86_32(
	const void *key,
	int len,
	uint32_t seed,
	void *out
);

struct Entry {
	int used;
	const void *key;
	void *value;
};

struct HashTable {
	HashFunc hash;
	CompareFunc cmp;
	struct Entry *table;
	size_t len;
	size_t size;
};

uint32_t
str_hash(const void *key)
{
	static uint32_t seed = 0x110e0e67;  // just a random constant
	uint32_t hash;
	murmurhash3_x86_32(key, strlen(key ? key : 0), seed, &hash);
	return hash;
}

int
str_cmp(const void *a, const void *b)
{
	return strcmp(a, b);
}

struct HashTable*
hashtable_new(HashFunc hash, CompareFunc cmp, size_t initial_size)
{
	assert(hash != NULL);
	assert(cmp != NULL);
	assert(initial_size > 0);

	struct HashTable *ht = malloc(sizeof(struct HashTable));
	if (!ht) {
		return NULL;
	}

	ht->table = calloc(sizeof(struct Entry), initial_size);
	if (!ht->table) {
		free(ht);
		return NULL;
	}

	ht->hash = hash;
	ht->cmp = cmp;
	ht->len = 0;
	ht->size = initial_size;
	return ht;
}

static int
hashtable_resize(struct HashTable *ht, size_t newsize)
{
	assert(ht->size < newsize);

	struct Entry *newtable = calloc(sizeof(struct Entry), newsize);
	if (!newtable) {
		return 0;
	}

	for (size_t i = 0; i < ht->size; i++) {
		if (ht->table[i].used) {
			size_t j = ht->hash(ht->table[i].key) % newsize;
			while (newtable[j].used) {
				if (++j == newsize) {
					j = 0;
				}
			}
			newtable[j] = ht->table[i];
		}
	}
	free(ht->table);
	ht->table = newtable;
	ht->size = newsize;

	return 1;
}

int
hashtable_set(struct HashTable *ht, const void *key, void *value)
{
	assert(ht != NULL);

	if (ht->len / (float)ht->size > LOAD_FACTOR_LIMIT &&
	    !hashtable_resize(ht, ht->size * 2)) {
		return 0;
	}

	size_t i = ht->hash(key) % ht->size;
	while (ht->table[i].used) {
		if (ht->cmp(key, ht->table[i].key) == 0) {
			ht->table[i].key = key;
			ht->table[i].value = value;
			return 1;
		}

		if (++i == ht->size) {
			i = 0;
		}
	}
	ht->len++;
	ht->table[i].used = 1;
	ht->table[i].key = key;
	ht->table[i].value = value;

	return 1;
}

void*
hashtable_get(struct HashTable *ht, const void *key)
{
	assert(ht != NULL);

	size_t i = ht->hash(key) % ht->size;
	while (ht->table[i].used) {
		if (ht->cmp(key, ht->table[i].key) == 0) {
			return ht->table[i].value;
		}

		if (++i == ht->size) {
			i = 0;
		}
	}

	return NULL;
}

void*
hashtable_pop(struct HashTable *ht, const void *key)
{
	assert(ht != NULL);

	size_t i = ht->hash(key) % ht->size;
	while (ht->table[i].used) {
		if (ht->cmp(key, ht->table[i].key) == 0) {
			void *value = ht->table[i].value;
			size_t j = i;
			while (ht->table[j + 1 < ht->size ? j + 1 : 0].used) {
				if (++j == ht->size) {
					j = 0;
				}
			}
			memmove(ht->table + i, ht->table + j, sizeof(struct Entry));
			memset(ht->table + j, 0, sizeof(struct Entry));
			ht->len--;
			return value;
		}

		if (++i == ht->size) {
			i = 0;
		}
	}

	return NULL;
}

size_t
hashtable_len(struct HashTable *ht)
{
	assert(ht != NULL);

	return ht->len;
}

void
hashtable_free(struct HashTable *ht)
{
	free(ht);
}