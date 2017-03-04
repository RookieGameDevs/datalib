#include "hash_table.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MIN_SIZE 32
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

uint32_t
int_hash(const void *key)
{
	return (uint32_t)key;
}

int
int_cmp(const void *a, const void *b)
{
	if (a < b) {
		return -1;
	} else if (a == b) {
		return 0;
	}
	return 1;
}

struct HashTable*
hash_table_new(HashFunc hash, CompareFunc cmp, size_t initial_size)
{
	assert(hash != NULL);
	assert(cmp != NULL);

	// start with a meaningful initial size, in order to avoid unnecessary
	// reallocations when table is small
	initial_size = initial_size < MIN_SIZE ? MIN_SIZE : initial_size;

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
hash_table_resize(struct HashTable *ht, size_t newsize)
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
hash_table_set(struct HashTable *ht, const void *key, void *value)
{
	assert(ht != NULL);

	if (ht->len / (float)ht->size > LOAD_FACTOR_LIMIT &&
	    !hash_table_resize(ht, ht->size * 2)) {
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
hash_table_get(struct HashTable *ht, const void *key)
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
hash_table_pop(struct HashTable *ht, const void *key)
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
hash_table_len(struct HashTable *ht)
{
	assert(ht != NULL);

	return ht->len;
}

size_t
hash_table_size(struct HashTable *ht)
{
	assert(ht != NULL);

	return ht->size;
}

void
hash_table_iter_init(struct HashTable *ht, struct HashTableIter *it)
{
	assert(ht != NULL);
	assert(it != NULL);

	it->ht = ht;
	it->cur = 0;
}

int
hash_table_iter_next(struct HashTableIter *it, const void **k, void **v)
{
	assert(it != NULL);

	while (!it->ht->table[it->cur].used && it->cur < it->ht->size) {
		it->cur++;
	}

	if (it->cur < it->ht->size) {
		struct Entry ent = it->ht->table[it->cur];
		if (k) {
			*k = ent.key;
		}
		if (v) {
			*v = ent.value;
		}
		it->cur++;
		return 1;
	}

	return 0;
}

void
hash_table_free(struct HashTable *ht)
{
	free(ht->table);
	free(ht);
}