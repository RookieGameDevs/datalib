#include "hashtable.h"
#include <check.h>
#include <stdlib.h>

#define int_to_ptr(i) ((void*)(unsigned long long)i)
#define ptr_to_int(p) ((int)(unsigned long long)p)

START_TEST(test_hashtable_simple)
{
	struct HashTable *ht = hashtable_new(str_hash, str_cmp, 0);
	ck_assert(ht != NULL);

	ck_assert(hashtable_set(ht, "hello", int_to_ptr(1234)));
	ck_assert(hashtable_set(ht, "world", int_to_ptr(4321)));
	ck_assert_uint_eq(hashtable_len(ht), 2);

	int hello = ptr_to_int(hashtable_get(ht, "hello"));
	ck_assert_int_eq(hello, 1234);

	int world = ptr_to_int(hashtable_get(ht, "world"));
	ck_assert_int_eq(world, 4321);

	hello = ptr_to_int(hashtable_pop(ht, "hello"));
	ck_assert_int_eq(hello, 1234);
	ck_assert_uint_eq(hashtable_len(ht), 1);

	world = ptr_to_int(hashtable_pop(ht, "world"));
	ck_assert_int_eq(world, 4321);
	ck_assert_uint_eq(hashtable_len(ht), 0);

	hashtable_free(ht);
}
END_TEST

START_TEST(test_hashtable_grow)
{
	struct HashTable *ht = hashtable_new(int_hash, int_cmp, 0);
	ck_assert(ht != NULL);

	size_t n_insertions = 1e5;
	const void *keys[n_insertions];

	for (int i = 0; i < n_insertions; i++) {
		const void *key = int_to_ptr(rand());
		if (!hashtable_get(ht, key)) {
			ck_assert(hashtable_set(ht, key, int_to_ptr(1)));
			keys[i] = key;
		}
	}

	ck_assert_uint_eq(hashtable_len(ht), n_insertions);
	ck_assert_uint_lt(hashtable_len(ht), hashtable_size(ht));

	for (int i = 0; i < n_insertions; i++) {
		ck_assert(hashtable_get(ht, keys[i]));
	}

	hashtable_free(ht);
}
END_TEST

START_TEST(test_hashtable_iter)
{
	struct HashTable *ht = hashtable_new(int_hash, int_cmp, 0);
	ck_assert(ht != NULL);

	size_t items = 100;
	const void *keys[items];

	for (int i = 0; i < items; i++) {
		const void *key = int_to_ptr(rand());
		if (!hashtable_get(ht, key)) {
			ck_assert(hashtable_set(ht, key, int_to_ptr(1)));
			keys[i] = key;
		}
	}

	struct HashTableIter it;
	hashtable_iter_init(ht, &it);
	const void *key = NULL;
	size_t iterations = 0;
	while (hash_table_iter_next(&it, &key, NULL)) {
		int found = 0;
		for (size_t i = 0; i < items; i++) {
			if (int_cmp(key, keys[i]) == 0) {
				found = 1;
				keys[i] = NULL;
				break;
			}
		}
		ck_assert(found);
		iterations++;
	}
	ck_assert_uint_eq(items, iterations);

	hashtable_free(ht);
}
END_TEST

Suite*
hashtable_suite(void)
{
	Suite *s = suite_create("hashtable");

	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_hashtable_simple);
	tcase_add_test(tc_core, test_hashtable_grow);
	tcase_add_test(tc_core, test_hashtable_iter);
	suite_add_tcase(s, tc_core);

	return s;
}
