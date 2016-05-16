#include "minunit.h"

#include "dissect/buffer/Buffy.h"
#include "dissect/buffer/Buffy-int.h"



#include "plugbase.h" 
#include "decode.h"


const uint8_t pkt[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; 

struct Buffy *buffy = NULL;

Packet *mockPacket() {

	Packet *mockedPacket = calloc(1, sizeof(Packet));

	mockedPacket->pkt = &pkt[0]; 


	return mockedPacket;
}


char *beforeTestSuite() {
	return NULL;
}

char *afterTestSuite() {

	return NULL;
}

char *beforeEachTest() {

	Packet *p = mockPacket();
	buffy = Buffy_new(p);

    return NULL;
}

char *afterEachTest() {

	free(buffy->p);

	buffy->ops->Buffy_free(buffy);

	buffy = NULL;

    return NULL;
}


char *newBuffyInitializedCorrectly() {

	Packet *p = mockPacket();
	struct Buffy *buf = Buffy_new(p);

	mu_assert(buf->head->prev == buf, "the head should be the first buffer");
	
	mu_assert(buf->head->next == buf, "the head should be the first buffer");
	
	mu_assert(0 == buf->position, "the position should be 0 at first");

	mu_assert(buf->head == buf->next, "there should be no next buffer yet (dummy only)");

	mu_assert(buf->head == buf->prev, "there should be no prev buffer yet (dummy only)");

	mu_assert(buf->data == &pkt[0], "the buffer data should point to the raw packet start");

	mu_assert(buf->p == p, "the backing package should be the mocked one");

	return NULL;
}

char *getBits8ReturnsCorrectly() {

	uint8_t bits = buffy->ops->Buffy_getBits8(buffy, 0);
	
	mu_assert_int_equals(0, bits, "should be the first byte");

	bits = buffy->ops->Buffy_getBits8(buffy, 1);

	mu_assert_int_equals(1, bits, "should be the second byte");

	bits = buffy->ops->Buffy_getBits8(buffy, 9);

	mu_assert_int_equals(9, bits, "should be the last byte");

	return NULL;
}

char *getBitsWalk8WalksAndReturnsCorrectly() {

	uint8_t bits;

	int bytesDissected = 0;

	for (int i = 0; i < 10; i++) {

		bits = buffy->ops->Buffy_getBitsWalk8(buffy, &bytesDissected);

		mu_assert_int_equals(i, bits, "should be the correct byte position");
	}

	return NULL;
}

char *all_tests() {

    mu_suite_start();

	mu_run_test(newBuffyInitializedCorrectly);

	mu_run_test_set(beforeEachTest, afterEachTest, getBits8ReturnsCorrectly);

	mu_run_test_set(beforeEachTest, afterEachTest, getBitsWalk8WalksAndReturnsCorrectly);

    return NULL;
}

RUN_TESTS(beforeTestSuite, afterTestSuite, all_tests);
