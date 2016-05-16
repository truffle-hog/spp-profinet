#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message) if (!(test)) { log_err(message); return message; }
#define mu_run_test(test) debug("\n-----%s", " " #test); \
    message = test(); tests_run++; if (message) return message;
#define mu_run_test_set(before, after, test) debug("\n-----%s", " " #test); \
    before(); message = test(); after(); tests_run++; if (message) return message;

#define mu_assert_null(actual, message) if (actual != NULL) { log_err("%s - value should be NULL", message); return message; }

#define mu_assert_true(actual, message) if (actual != 1) { log_err("%s - should be true", message); return message; }

#define mu_assert_int_equals(expected, actual, message) if (expected != actual) { log_err("%s - expected: %d but got %d", message, expected, actual); return message; }

#define mu_assert_long_equals(expected, actual, message) if (expected != actual) { log_err("%s - expected: %ld but got %ld", message, expected, actual); return message; }

#define mu_assert_string_equals(expected, actual, message) if (strcmp(expected, actual) != 0) { log_err("%s - expected: %s but got %s", message, expected, actual); return message; }

#define RUN_TESTS(before, after, name) int main(int argc, char *argv[]) {\
    (void)argc; \
    (void)argv; \
    argc = 1; \
    debug("----- RUNNING: %s", argv[0]);\
        printf("----\nRUNNING: %s\n", argv[0]);\
        before(); \
        char *result = name();\
        after(); \
        if (result != 0) {\
            printf("FAILED: %s\n", result);\
        }\
        else {\
            printf("ALL TESTS PASSED\n");\
        }\
    printf("Tests run: %d\n", tests_run);\
        int ret = 0;\
        if (result != 0) {\
            ret = 0;\
        }\
        else {\
            ret = tests_run;\
        }\
        exit(ret);\
}


int tests_run;

#endif
