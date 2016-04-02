#include "minunit.h"

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"

#include "dissect/DissectorRegister-int.h"
#include "dissect/DissectorRegister.h"

char *beforeEachTest() {

    log_warn("not implemented");
    return NULL;
}

char *afterEachTest() {

    log_warn("not implemented");
    return NULL;
}


char *test_template() {

    mu_assert(0 == 1, "No tests implemented yet!");

    log_warn("not implemented");

    return NULL;
}

char *all_tests() {

    mu_suite_start();

    mu_run_test(test_template);

    return NULL;
}

RUN_TESTS(beforeEachTest, afterEachTest, all_tests);
