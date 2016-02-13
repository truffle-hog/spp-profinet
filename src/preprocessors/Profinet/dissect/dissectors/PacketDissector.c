/**
 * @file
 * @brief PacketDissector implementation.
 */
#include <stdlib.h>
#include <stdio.h>

#include "Dissector-int.h"
#include "PacketDissector.h"
#include "EthernetDissector.h"

#include "tree/ProtocolTree.h"
#include "tree/ProtocolTree-int.h"

#include "Buffy.h"
#include "Buffy-int.h"

#include "ProtocolTypes.h"

#include "dbg.h"


/**
 * @brief The Dissector for Profi Real Time IO 0x8892.
 *
 */
struct PacketDissector {
    /** @brief Houses a Dissector internally for safe type casting. **/
    struct Dissector dissector;

};

void PacketDissector_free(Dissector_t *dissector);

int PacketDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

/**
 * @see Dissector_ops
 */
static const struct Dissector_ops PacketDissectorOverride_ops = {

    sizeof(struct PacketDissector), /* size */
    (uint64_t) 0xffffffffffffffff,
    (uint64_t) 0xffffffffffffffff,
    PacketDissector_free,
    Dissector_registerSub, // TODO can be NULL?
    Dissector_getSub, // TODO can be NULL?
    PacketDissector_dissect
};

void PacketDissector_initializeSubDissectors(Dissector_t *this) {

    Dissector_t *ethernetDissector = EthernetDissector_new();

    this->ops->Dissector_registerSub(this, ethernetDissector);
}

/**
 * @see Dissector_new
 */
Dissector_t *PacketDissector_new() {


	struct PacketDissector *packetDissector = malloc(sizeof(struct PacketDissector));;
    check_mem(packetDissector);

    packetDissector->dissector = *Dissector_new(&PacketDissectorOverride_ops);
    check_mem(&packetDissector->dissector);

    PacketDissector_initializeSubDissectors(&packetDissector->dissector);

    return (Dissector_t*) packetDissector;

error:
    return NULL;

}

/**
 * @see Dissector_free
 */
void
PacketDissector_free(Dissector_t *dissector) {

	struct PacketDissector *this = (struct PacketDissector*) dissector;

	free(this);
//	this->dissector.ops->Dissector_free(&this->dissector);
	// TODO implement
}

/**
 * @see Dissector_dissect
 */
int
PacketDissector_dissect(Dissector_t *this, Buffy_t *buf, struct ProtocolNode *node) {

	// TODO create a proper EMPTY value in the header that can be used as empty labels
	struct Value NONE;
	NONE.val.character = '0';

    //debug("%04X", buf->p->eh->ether_type);

    struct Value frameLength;
	frameLength.val.int32 = 0;
	frameLength.length = 32;
	frameLength.type = is_int32;

	check_mem(node->ops->ProtocolTree_branch(node, "frame_length", frameLength));

    struct Value timeDelta;
    timeDelta.val.float32 = 0.00023;
	timeDelta.length = 32;
	timeDelta.type = is_float;

	check_mem(node->ops->ProtocolTree_branch(node, "frame_time_delta", timeDelta));

    Dissector_t *nextDissector;

	// TODO check for the next package to be used
  //  if (isEthernet(buf)) {
    nextDissector = this->ops->Dissector_getSub(this, ETHERNET);
    check(nextDissector != NULL, "there has to be a next dissector");
  //  }

    ProtocolItem_t *child = node->ops->ProtocolTree_branch(node, "ethernet", NONE);
    check_mem(child);

    nextDissector->ops->Dissector_dissect(nextDissector, buf, child);

	// TODO implement

	return 0;
error:
    return -1;
}
