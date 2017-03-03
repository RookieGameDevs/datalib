#pragma once

#include <stdint.h>

struct HashTable;

typedef uint32_t (*HashFunc)(const void *key);

struct HashTable*
hashtable_new(HashFunc h);

void
hashtable_free(struct HashTable *ht);