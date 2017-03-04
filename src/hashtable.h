#pragma once

#include <stddef.h>
#include <stdint.h>

struct HashTable;

struct HashTableIter {
	struct HashTable *ht;
	size_t cur;
};

typedef uint32_t (*HashFunc)(const void *key);
typedef int (*CompareFunc)(const void *a, const void *b);

uint32_t
str_hash(const void *key);

int
str_cmp(const void *a, const void *b);

uint32_t
int_hash(const void *key);

int
int_cmp(const void *a, const void *b);

struct HashTable*
hashtable_new(HashFunc hash, CompareFunc cmp, size_t initial_size);

int
hashtable_set(struct HashTable *ht, const void *key, void *value);

void*
hashtable_get(struct HashTable *ht, const void *key);

void*
hashtable_pop(struct HashTable *ht, const void *key);

size_t
hashtable_len(struct HashTable *ht);

size_t
hashtable_size(struct HashTable *ht);

void
hashtable_iter_init(struct HashTable *ht, struct HashTableIter *it);

int
hash_table_iter_next(struct HashTableIter *it, const void **k, void **v);

void
hashtable_free(struct HashTable *ht);