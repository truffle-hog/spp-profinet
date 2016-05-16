#include "minunit.h"

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"

#include "dissect/DissectorRegister-int.h"
#include "dissect/DissectorRegister.h"

struct DissectorRegister *reg = NULL;

char *beforeTestSuite() {

	return NULL;
}

char *afterTestSuite() {

	return NULL;
}

char *beforeEachTest() {

	debug("CREATING DISSECTOR REGISTER");
	reg = DissectorRegister_new();

    return NULL;
}

char *afterEachTest() {

	debug("FREEING DISSECTOR REGISTER");

	DissectorRegister_free(reg);
	reg = NULL;

    return NULL;
}



void MockedDissector_free(struct Dissector *dissector) {

	free(dissector);
}

int MockedDissector_dissect(struct Dissector *dissector, struct Buffy *buffy, struct ProtocolNode *node) {

	(void) dissector;
	(void) buffy;
	(void) node;

	debug("mocked dissecting");
	return 0;
}


static const struct Dissector_ops mockedDissectorOverride_ops = {

    0l,
	1,
    5,
    MockedDissector_free,
    Dissector_registerSub, // TODO can be NULL?
    Dissector_getSub, // TODO can be NULL?
    MockedDissector_dissect,
    "Mocked Dissector",
    0x1
};


struct Dissector_ops *mock_Dissector_ops(uint64_t lower, uint64_t upper) {

	struct Dissector_ops *mockedOps = calloc(1, sizeof(struct Dissector_ops));
	mockedOps->Dissector_lower = lower;
	mockedOps->Dissector_upper = upper;
	mockedOps->Dissector_name = "mocked one";

	mockedOps->Dissector_free = MockedDissector_free;

	mockedOps->Dissector_dissect = MockedDissector_dissect;

	return mockedOps;
}

struct Dissector *mock_Dissector(uint64_t lower, uint64_t upper) {

	struct Dissector *mockedDissector = calloc(1, sizeof(struct Dissector));
	mockedDissector->ops = mock_Dissector_ops(lower, upper);

	return mockedDissector;
}


char *newDissectorRegisterInitializedCorrectly() {

	struct DissectorRegister *emptyReg = DissectorRegister_new();
	
	mu_assert_true(emptyReg->initialized, "a newly created register should be initialized");
	
	mu_assert_long_equals(0l, emptyReg->size, "a newly created register should have a size of 0");

	return NULL;
}

char *insertNewDissectorReturnsNullWhenRegisterIsEmpty() {

	struct Dissector *mockedDissector = mock_Dissector(0, 5);

	struct Dissector *existing = reg->ops->DissectorRegister_insert(reg, mockedDissector);
	mu_assert_null(existing, "there should not be any existing dissector registered");
	
	return NULL;
}

char *getDissectorReturnsCorrectDissector() {

	struct Dissector *mockedDissector = mock_Dissector(1, 5);
	reg->ops->DissectorRegister_insert(reg, mockedDissector);

	struct Dissector *returnedDissector = reg->ops->DissectorRegister_get(reg, 1);
	mu_assert(mockedDissector == returnedDissector, "the mocked dissector should be retrieved");

	returnedDissector = reg->ops->DissectorRegister_get(reg, 2);
	mu_assert(mockedDissector == returnedDissector, "the mocked dissector should be retrieved");

	returnedDissector = reg->ops->DissectorRegister_get(reg, 5);
	mu_assert(mockedDissector == returnedDissector, "the mocked dissector should be retrieved");

	returnedDissector = reg->ops->DissectorRegister_get(reg, 0);
	mu_assert_null(returnedDissector, "there should not be any dissector registered");

	returnedDissector = reg->ops->DissectorRegister_get(reg, 6);
	mu_assert_null(returnedDissector, "there should not be any dissector registered");

	struct Dissector *mockedSecondDissector = mock_Dissector(6, 10);

	returnedDissector = reg->ops->DissectorRegister_insert(reg, mockedSecondDissector);
	mu_assert_null(returnedDissector, "there should have been no previous dissector");

	returnedDissector = reg->ops->DissectorRegister_get(reg, 6);
	mu_assert(mockedSecondDissector == returnedDissector, "the second should be returned");

	mu_assert(mockedSecondDissector == reg->ops->DissectorRegister_get(reg, 8), "the second should be returned");

	mu_assert(mockedSecondDissector == reg->ops->DissectorRegister_get(reg, 10), "the second should be returned");

	return NULL;
}

char *all_tests() {

    mu_suite_start();

    mu_run_test(newDissectorRegisterInitializedCorrectly);

	mu_run_test_set(beforeEachTest, afterEachTest, insertNewDissectorReturnsNullWhenRegisterIsEmpty);

	mu_run_test_set(beforeEachTest, afterEachTest, getDissectorReturnsCorrectDissector);
    return NULL;
}

RUN_TESTS(beforeTestSuite, afterTestSuite, all_tests);
