#include <check.h>
#include <stdlib.h>

Suite*
hashtable_suite(void);

int
main(int argc, char *argv[])
{
	// initialize a master suite and a suite runner
	Suite *s = suite_create("suite");
	SRunner *sr = srunner_create(s);

	// add external suites
	srunner_add_suite(sr, hashtable_suite());

	// execute all suites
	srunner_run_all(sr, CK_NORMAL);
	int failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}