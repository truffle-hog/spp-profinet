/**
 * @file
 * @brief PacketDissector implementation.
 *
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include "Dissector-int.h"
#include "Dissector.h"
#include "ProtocolTree.h"
#include "ProtocolTree-int.h"
#include "Buffy.h"

#include "FieldInfos.h"

#include "EthernetDissector.h"

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

void initializeSubDissectors(Dissector_t *this) {

    Dissector_t *ethernetDissector = EthernetDissector_new();

    this->ops->Dissector_registerSub(this, ethernetDissector);
}

/**
 * @see Dissector_new
 */
Dissector_t *
PacketDissector_new() {


	struct PacketDissector *packetDissector;

    packetDissector = malloc(sizeof(struct PacketDissector));
    check_mem(packetDissector);

    Dissector_t *disser = Dissector_new(&PacketDissectorOverride_ops);
    check_mem(disser);

    packetDissector->dissector = *disser;

    initializeSubDissectors((Dissector_t*) packetDissector);

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

	(void) dissector;
	// TODO implement
}

/**
 * @see Dissector_dissect
 */
int
PacketDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree) {


	struct FieldInfo *frameInfo = malloc(sizeof(struct FieldInfo));
	frameInfo->key = "frame_info";
	frameInfo->name = "Frame Information";	
	frameInfo->headerType = FRAME;	

    tree->ops->ProtocolTree_setFieldInfo(tree, frameInfo);

    Dissector_t *nextDissector = this->ops->Dissector_getSub(this, frameInfo->headerType);
    ProtocolItem_t *child = tree->ops->ProtocolNode_branch(tree);

    nextDissector->ops->Dissector_dissect(nextDissector, buf, child);

	printf("dissecting very bigtime\n");
	// TODO implement

	return 0;
}
