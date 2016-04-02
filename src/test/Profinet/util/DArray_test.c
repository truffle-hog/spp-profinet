#include "minunit.h"

#include "util/DArray.h"

struct DArray *dArray = NULL;

char *beforeEachTest() {

    dArray = DArray_new(20);
    return NULL;
}

char *afterEachTest() {
    DArray_free(dArray);
    dArray = NULL;
    return NULL;
}

char *test_DArray_new() {

    struct DArray *mArray = DArray_new(20);
    mu_assert(mArray != NULL, "Failed to create new dynamic array");

    mu_assert(mArray->allocated == 20, "Failed to set allocation count");
    mu_assert(mArray->size == 0, "Failed to set size to zero during initialization");
    mu_assert(mArray->ptrArray != NULL, "Failed to create poitner array for initial size");

    return NULL;
}

char *test_DArray_free() {
    struct DArray *dArray = DArray_new(20);

    DArray_free(dArray);
    // TODO how to check if free really works?

    return NULL;
}

char *test_DArray_clear() {
    int a = 0;
    int b = 1;
    int c = 2;

    DArray_pushBack(dArray, &a);
    DArray_pushBack(dArray, &b);
    DArray_pushBack(dArray, &c);

    mu_assert(dArray->size == 3, "adding elements failed");

    DArray_clear(dArray);

    mu_assert(dArray->size == 0, "clearing the array failed");

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_DArray_new);
    mu_run_test(test_DArray_free);

    mu_run_test(test_DArray_clear);

    return NULL;
}

RUN_TESTS(beforeEachTest, afterEachTest, all_tests);
