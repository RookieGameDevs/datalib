#include "hashtable.h"
#include <check.h>

#define int_to_ptr(i) ((void*)(unsigned long long)i)
#define ptr_to_int(p) ((int)(unsigned long long)p)

START_TEST(test_hashtable_string)
{
	struct HashTable *ht = hashtable_new(str_hash, str_cmp, 8);
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

Suite*
hashtable_suite(void)
{
	Suite *s = suite_create("hashtable");

	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_hashtable_string);
	suite_add_tcase(s, tc_core);

	return s;
}
