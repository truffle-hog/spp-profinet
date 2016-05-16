#include "minunit.h"

#include "util/ScopeAlloc.h"

char *beforeTestSuite() {

	return NULL;
}

char *afterTestSuite() {

	return NULL;
}

char *beforeEachTest() {

    log_warn("not implemented");
    return NULL;
}

char *afterEachTest() {

    log_warn("not implemented");
    return NULL;
}

char *freeingUninitializedScopeReturnsCorrectErrorCode() {

	mu_assert_int_equals(-1, appScopeFree(), "Freeing uninitialized scope should fail");
	mu_assert_int_equals(-1, packetScopeFree(), "Freeing uninitialized scope should fail");
	mu_assert_int_equals(-1, clientScopeFree(), "Freeing uninitialized scope should fail");

	return NULL;
}

char *all_tests() {

    mu_suite_start();

    mu_run_test(freeingUninitializedScopeReturnsCorrectErrorCode);

    return NULL;
}

RUN_TESTS(beforeTestSuite, afterTestSuite, all_tests);
