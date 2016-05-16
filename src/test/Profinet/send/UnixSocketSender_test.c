#include "minunit.h"

#include "send/Sender.h"
#include "send/Sender-int.h"
#include "send/UnixSocketSender.h"

Sender_t *sender = NULL;

char *beforeTestSuite() {

    return NULL;
}

char *afterTestSuite() {

    return NULL;
}

char *beforeEachTest() {

    sender = UnixSocketSender_new();
    return NULL;
}

char *afterEachTest() {

    return NULL;
}

char *unixSocketSenderInitializedCorrectly() {

	//struct UnixSocketSender *usender = (struct UnixSocketSender *) UnixSocketSender_new();

  struct Sender *s = UnixSocketSender_new();

  struct UnixSocketSender *us = (struct UnixSocketSender *) s;

	mu_assert_true(s->initialized, "initialized variable not set correctly");

	mu_assert_false(us->socketData.client_detected, "There should not be any client detected");

	s->ops->Sender_free(s);

  //UnixSocketSender_free(s);

	return NULL;
}

char *all_tests() {

    mu_suite_start();

    mu_run_test(unixSocketSenderInitializedCorrectly);

    //mu_run_test_set(beforeEachTest, afterEachTest, test_template);

    return NULL;
}

RUN_TESTS(beforeTestSuite, afterTestSuite, all_tests);
