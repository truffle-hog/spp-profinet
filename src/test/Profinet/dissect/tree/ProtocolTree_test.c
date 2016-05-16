#include "minunit.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"

struct ProtocolNode *proto = NULL;

char *beforeTestSuite() {

    return NULL;
}

char *afterTestSuite() {

    return NULL;
}

char *beforeEachTest() {

    proto = ProtocolTree_new("root_test", (Dissector_t *) "mockedDissector");
    return NULL;
}

char *afterEachTest() {

    ProtocolTree_free(proto);
    proto = NULL;
    return NULL;
}

char *test_ProtocolTree_new() {

    Dissector_t *mockedDissector = (Dissector_t *) calloc(1, sizeof(Dissector_t));

    struct ProtocolNode *protoTree = ProtocolTree_new("root_test", mockedDissector);

    mu_assert(protoTree->treeData->nodePointers[0] == protoTree, "the first nodepointer must point to the initial node");

    mu_assert(!protoTree->isImportant, "the initial node must never be important");
    mu_assert_string_equals("", protoTree->importantKey, "key tag wrong");

    mu_assert_string_equals("root_test", protoTree->name, "name wrong");
    mu_assert_int_equals(0, protoTree->id, "id wrong");
    mu_assert_int_equals(0, protoTree->childCount, "childCount wrong");

    mu_assert(NULL == protoTree->parent, "the parent of the root must be null");
    mu_assert(NULL == protoTree->children, "initially the children of the root must be null");

    mu_assert(mockedDissector == protoTree->dissectedBy, "the dissector is not correct");

    mu_assert_int_equals(1, protoTree->treeData->size, "treedata size wrong");

    return NULL;
}

char *test_ProtocolTree_branch() {

    struct Value *v = calloc(1, sizeof(struct Value));
    Dissector_t *mockedDissector = (Dissector_t *) calloc(1, sizeof(Dissector_t));

    struct ProtocolNode *branch = proto->ops->ProtocolTree_branch(proto, "testBranch", *v, mockedDissector);

	mu_assert(mockedDissector == branch->dissectedBy, "the dissector is not correct");

	mu_assert(branch->value.type == v->type, "the value is wrong");

	mu_assert(branch->value.length == v->length, "the length of the value is wrong");

	mu_assert(branch->value.val.int64 == v->val.int64, "the val is not correct");	

	mu_assert_int_equals(2, proto->treeData->size, "size wrong");

    return NULL;
}

char *test_ProtocolTree_branchImportant() {

    struct Value *v = calloc(1, sizeof(struct Value));
    Dissector_t *mockedDissector = (Dissector_t *) calloc(1, sizeof(Dissector_t));

	struct ProtocolNode *branch = proto->ops->ProtocolTree_branchImportant(proto, "testBranch", "test_key", *v, mockedDissector); 

	log_warn("not implemented");

	return NULL;
}

char *test_ProtocolTree_getValue() {

	struct Value val = proto->ops->ProtocolTree_getValue(proto);

	mu_assert(0 == val.val.int64, "wrong value recovered");
	mu_assert(is_int64 == val.type, "wrong type");

	return NULL;
}

char *test_template() {


    //mu_assert(0 == 1, "No tests implemented yet!");

    log_warn("not implemented");
    return NULL;
}

char *all_tests() {

    mu_suite_start();

    mu_run_test(test_ProtocolTree_new);

	mu_run_test_set(beforeEachTest, afterEachTest, test_ProtocolTree_branch);

    mu_run_test_set(beforeEachTest, afterEachTest, test_template);

	mu_run_test_set(beforeEachTest, afterEachTest, test_ProtocolTree_getValue);

    return NULL;
}

RUN_TESTS(beforeTestSuite, afterTestSuite, all_tests);
