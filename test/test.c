#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Test definitions
// ----------------------------------------------------------------------------
typedef struct {
	char *testName;
	void (*test_func)(void);
} test_t;

#define TEST_EXAMPLE "testExample"
static void Test_testExample(void);

// Do not modify this one. This will help the program determine that the end
// of tests has been reached.
#define END_OF_TESTS_STR "0_END_OF_TESTS"
static bool Test_reachedEndOfTests(test_t _test);
static test_t end_of_tests = {END_OF_TESTS_STR, NULL};

// Main and auxiliary function implementations
// ----------------------------------------------------------------------------
/* The way tests works is as follows: if you call this program with no
 * arguments, it will run all existing tests. If you call it with one or more
 * arguments, if the first argument correspond to a test string, then that
 * corresponding test function will be invoked. Otherwise, nothing happens. It
 * is a simple unitary test program that is not very flexible, but does the
 * job.
 */
int main(int argc, char *argv[]) {
	/* Add the corresponding test argument to the corresponding test function in
	 * the array below (e.g. {TEST_MODULE, &Test_testModule}). Define test
	 * strings in the Test Strings definitions above. The signature of test
	 * functions should be f: void -> void. You are encouraged to include printf
	 * statements, asserts, and exits with error in the test functions.
	 */
	test_t tests[] = {{TEST_EXAMPLE, &Test_testExample}, end_of_tests};

	printf("Tests have started\n");
	if (argc <= 1) {
		int i = 0;
		test_t test = tests[i++];
		while (!Test_reachedEndOfTests(test)) {
			test.test_func();
			test = tests[i++];
		}
	} else {
		char *targetTest = argv[1];
		int i = 0;
		test_t test = tests[i++];
		while (!Test_reachedEndOfTests(test)) {
			if (strcmp(targetTest, test.testName) == 0) {
				test.test_func();
			}
			test = tests[i++];
		}
	}
}

static bool Test_reachedEndOfTests(test_t _test) {
	return (strcmp(_test.testName, END_OF_TESTS_STR) == 0);
}

// Tests
// ----------------------------------------------------------------------------
static void Test_testExample(void) {
	printf(
	    "This is a test example. It can include function calls, asserts, and \
much more! This one does nothing.");
	return;
}
