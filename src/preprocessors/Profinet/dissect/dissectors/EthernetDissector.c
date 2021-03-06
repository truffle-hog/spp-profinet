/**
 * @file
 * @brief EthernetDissector implementation.
 *
 * This Dissector is the 0x8892 toplevel dissector, which will be followed by frame
 * and block dissectors.
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include "dissect/ProtocolTypes.h"

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"
#include "dissect/dissectors/EthernetDissector.h"
#include "dissect/dissectors/PNRTDissector.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"

#include "dissect/buffer/Buffy.h"
#include "dissect/buffer/Buffy-int.h"

#include "Profinet/dbg.h"

/**
 * @brief The Dissector for Profi Real Time IO 0x8892.
 *
 */
struct EthernetDissector {
    /** @brief Houses a Dissector internally for safe type casting. **/
    struct Dissector dissector;

};

void EthernetDissector_free(Dissector_t *dissector);

int EthernetDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

/**
 * @see Dissector_ops
 */
static const struct Dissector_ops EthernetDissectorOverride_ops = {

    sizeof(struct EthernetDissector), /* size */
    ETHERNET,
    ETHERNET,
    EthernetDissector_free,
    Dissector_registerSub, // TODO can be NULL?
    Dissector_getSub, // TODO can be NULL?
    EthernetDissector_dissect,
    "Ethernet Dissector",
    0x1
};


void EthernetDissector_initializeSubDissectors(Dissector_t *this) {

//	Buffy_t *buf = Buffy_new(NULL);
//	buf->ops->Buffy_createVirtual(buf, 4);

    Dissector_t *pnrtDissector = PNRTDissector_new();

    this->ops->Dissector_registerSub(this, pnrtDissector);
}

/**
 * @see Dissector_new
 */
Dissector_t *
EthernetDissector_new() {


	struct EthernetDissector *EthernetDissector;

    EthernetDissector = malloc(sizeof(struct EthernetDissector));
    check_mem(EthernetDissector);

    Dissector_t *disser = Dissector_new(&EthernetDissectorOverride_ops);
    check_mem(disser);

    EthernetDissector->dissector = *disser;

    EthernetDissector_initializeSubDissectors(disser);

    return (Dissector_t*) EthernetDissector;

    error:
    return NULL;

}

/**
 * @see Dissector_free
 */
void
EthernetDissector_free(Dissector_t *dissector) {

	struct EthernetDissector *this = (struct EthernetDissector*) dissector;

	free(this);
//	this->dissector.ops->Dissector_free(&this->dissector);

	(void) dissector;
	// TODO implement
}

/**
 * @see Dissector_dissect
 */
int
EthernetDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *node) {

    check(node != NULL, "The node must not be null.");
    check(this != NULL, "The calling dissector must not be null");
    check(buf != NULL, "The buffer must not be null");

	struct Value ethernet;
	ethernet.type = is_string;
	ethernet.val.string = "Ethernet";

	ProtocolItem_t *etherNode = node->ops->ProtocolTree_branch(node, "ethernet", ethernet, this);
    check_mem(etherNode);

	struct Value dest;
	dest.val.uint64 = 0;
	dest.type = is_uint64;

    //debug("%04X", buf->p->eh->ether_type);

    memcpy(&dest.val.uint64, buf->p->eh->ether_dst, 6);
    dest.val.uint64 = (htobe64(dest.val.uint64)) >> 16;

	check_mem(etherNode->ops->ProtocolTree_branchImportant(etherNode, "ether_dest", "ether_dest", dest, this));

	struct Value source;
	source.val.uint64 = 0;
	source.type = is_uint64;

	memcpy(&source.val.uint64, buf->p->eh->ether_src, 6);
	source.val.uint64 = (htobe64(source.val.uint64)) >> 16;

    check_mem(etherNode->ops->ProtocolTree_branchImportant(etherNode, "ether_source", "ether_source", source, this));

	struct Value ethertype;
	ethertype.val.uint16 = htobe16(buf->p->eh->ether_type);
	ethertype.type = is_uint16;

	check_mem(etherNode->ops->ProtocolTree_branchImportant(etherNode, "ether_type", "ether_type", ethertype, this));

    Dissector_t *nextDissector;

    nextDissector = this->ops->Dissector_getSub(this, ethertype.val.uint16);
    check_warn(nextDissector != NULL, "No dissector registered for ethertype 0x%04X", ethertype.val.uint16);

	Buffy_t *virtual = buf->ops->Buffy_createVirtual(buf, 14 * 8);

    return nextDissector->ops->Dissector_dissect(nextDissector, virtual, etherNode);

warn:
    return 14;

error:
    return -1;
}
