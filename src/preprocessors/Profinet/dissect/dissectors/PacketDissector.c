/**
 * @file
 * @brief PacketDissector implementation.
 */
#include <stdlib.h>
#include <stdio.h>

#include "dissect/Dissector-int.h"
#include "dissect/dissectors/PacketDissector.h"
#include "dissect/dissectors/EthernetDissector.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"

#include "dissect/buffer/Buffy.h"
#include "dissect/buffer/Buffy-int.h"

#include "dissect/ProtocolTypes.h"

#include "dbg.h"



/**
 * @brief The Dissector for Profi Real Time IO 0x8892.
 *
 */
struct PacketDissector {
    /** @brief Houses a Dissector internally for safe type casting. **/
	struct Dissector dissector;
	long lastTimeSec;
	long lastTimeUSec;
	long packageCount;

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

	packetDissector->lastTimeSec = 0;
	packetDissector->lastTimeUSec = 0;

	packetDissector->packageCount = 0;

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

	struct PacketDissector* thisAsPacketDissector = (struct PacketDissector*) this;
	// TODO create a proper EMPTY value in the header that can be used as empty labels
	thisAsPacketDissector->packageCount++;

	node->value.val.int64 = thisAsPacketDissector->packageCount;
	node->value.type = is_int64;

    //debug("%04X", buf->p->eh->ether_type);

    struct Value frameLength;
	frameLength.val.int32 = buf->p->pkth->pktlen;
	frameLength.length = 32;
	frameLength.type = is_int32;

	check_mem(node->ops->ProtocolTree_branch(node, "frame_length", frameLength));



//	sprintf("%ld.%06ld\n", buf->p->pkth->ts.tv_sec, buf->p->pkth->ts.tv_usec);

    struct Value timeSec;
    timeSec.val.int64 = (long) buf->p->pkth->ts.tv_sec;
	timeSec.length = 64;
	timeSec.type = is_int64;

	check_mem(node->ops->ProtocolTree_branch(node, "frame_timestamp_sec", timeSec));

	struct Value timeUSec;
	timeUSec.val.int64 = (long) buf->p->pkth->ts.tv_usec;
	timeUSec.length = 64;
	timeUSec.type = is_int64;

	check_mem(node->ops->ProtocolTree_branch(node, "frame_timestamp_usec", timeUSec));

	struct Value deltaSecVal;
	deltaSecVal.val.int64 = timeSec.val.int64 - thisAsPacketDissector->lastTimeSec;
	deltaSecVal.type = is_int64;

	struct Value deltaUSecVal;
	deltaUSecVal.val.int64 = timeUSec.val.int64 - thisAsPacketDissector->lastTimeUSec;
	deltaUSecVal.type = is_int64;

	thisAsPacketDissector->lastTimeSec = timeSec.val.int64;
	thisAsPacketDissector->lastTimeUSec = timeUSec.val.int64;

	check_mem(node->ops->ProtocolTree_branch(node, "frame_delta_last_sec", deltaSecVal));
	check_mem(node->ops->ProtocolTree_branch(node, "frame_delta_last_usec", deltaUSecVal));

    Dissector_t *nextDissector;

	// TODO check for the next package to be used
  //  if (isEthernet(buf)) {
    nextDissector = this->ops->Dissector_getSub(this, ETHERNET);
    check(nextDissector != NULL, "there has to be a next dissector");
  //  }


    return nextDissector->ops->Dissector_dissect(nextDissector, buf, node);

	// TODO implement

	//return 0;
error:
    return -1;
}
