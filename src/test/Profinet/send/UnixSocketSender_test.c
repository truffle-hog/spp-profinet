#include "minunit.h"

#include "send/Sender.h"
#include "send/Sender-int.h"
#include "send/UnixSocketSender.h"

Sender_t *sender = NULL;

char *beforeTestSuite() {

    sender = UnixSocketSender_new();
    return NULL;
}

char *afterTestSuite() {

    return NULL;
}

char *beforeEachTest() {


    return NULL;
}

char *afterEachTest() {

    log_warn("not implemented");
    return NULL;
}


char *test_template() {

    //mu_assert(0 == 1, "No tests implemented yet!");

    log_warn("not implemented");
    return NULL;
}

char *all_tests() {

    mu_suite_start();

    mu_run_test_set(beforeEachTest, afterEachTest, test_template);

    return NULL;
}

RUN_TESTS(beforeTestSuite, afterTestSuite, all_tests);
