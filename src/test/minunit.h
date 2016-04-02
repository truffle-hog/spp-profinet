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

#define mu_assert_int_equals(expected, actual, message) if (expected != actual) { log_err("%s - expected: %d but got %d", message, expected, actual); return message; }

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
        exit(result != 0);\
}


int tests_run;

#endif
