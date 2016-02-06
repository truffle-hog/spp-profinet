/**
 * @file
 * @brief PacketDissector implementation.
 *
 * This Dissector is the 0x8892 toplevel dissector, which will be followed by frame
 * and block dissectors.
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include "Dissector-int.h"
#include "Dissector.h"
#include "ProtocolTree.h"
#include "Buffy.h"

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
    (uint64_t) 0x8892,
    (uint64_t) 0x8892,
    PacketDissector_free,
    Dissector_registerSub, // TODO can be NULL?
    Dissector_getSub, // TODO can be NULL?
    PacketDissector_dissect
};

/**
 * @see Dissector_new
 */
Dissector_t *
PacketDissector_new() {


	struct PacketDissector *PacketDissector;

	PacketDissector = malloc(sizeof(struct PacketDissector));
    check_mem(PacketDissector);

    Dissector_t *disser = Dissector_new(&PacketDissectorOverride_ops);
    check_mem(disser);

    PacketDissector->dissector = *disser;

    return (Dissector_t*) PacketDissector;

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

	(void) this;
	(void) buf;
	(void) tree;

    FieldInfo *frameInfo = FrameInfo_new(buf->packet);
    tree->ops->ProcolTree_setFieldInfo(tree, framInfo);

    Dissector_t *nextDissector = this->ops->Dissector_getSub(this, frameInfo->headerType);

    ProtocolItem_t *child = tree->ops->ProtocolTree_branch(tree);

    nextDissector->ops->Dissector_dissect(nextDissector, buf, child);

	printf("dissecting very bigtime\n");
	// TODO implement

	return 0;
}
