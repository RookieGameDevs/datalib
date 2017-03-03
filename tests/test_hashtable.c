#include "hashtable.h"
#include <check.h>

uint32_t
dummy_hash(const void *key)
{
	return (uint32_t)key;
}

START_TEST(test_hashtable)
{
	struct HashTable *ht = hashtable_new(dummy_hash);
	ck_assert(ht != NULL);
	hashtable_free(ht);
}
END_TEST

Suite*
hashtable_suite(void)
{
	Suite *s = suite_create("hashtable");

	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_hashtable);
	suite_add_tcase(s, tc_core);

	return s;
}
