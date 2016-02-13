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
    EthernetDissector_dissect
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

    // TODO create a central NONE value for every dissector to access
    struct Value NONE;
	NONE.val.character = '0';

	struct Value dest;
	dest.length = 48;
	dest.val.uint64 = 0;
	dest.type = is_uint64;

    //debug("%04X", buf->p->eh->ether_type);

    memcpy(&dest.val.uint64, buf->p->eh->ether_dst, 6);
    dest.val.uint64 = (htobe64(dest.val.uint64)) >> 16;

	check_mem(node->ops->ProtocolTree_branch(node, "ether_dest", dest));

	struct Value source;
	source.length = 48;
	source.val.uint64 = 0;
	source.type = is_uint64;

	memcpy(&source.val.uint64, buf->p->eh->ether_src, 6);
	source.val.uint64 = (htobe64(source.val.uint64)) >> 16;

    check_mem(node->ops->ProtocolTree_branch(node, "ether_source", source));

	struct Value ethertype;
	ethertype.length = 16;
	ethertype.val.uint16 = htobe16(buf->p->eh->ether_type);
	ethertype.type = is_uint16;

	ProtocolItem_t *child = node->ops->ProtocolTree_branch(node, "ether_type", ethertype);
    check_mem(child);

    Dissector_t *nextDissector;

    nextDissector = this->ops->Dissector_getSub(this, ethertype.val.uint16);
    check(nextDissector != NULL, "there has to be a next dissector");
  //  }

    // ProtocolItem_t *child = node->ops->ProtocolTree_branch(node, "pnrt", NONE);
    // check_mem(child);

	Buffy_t *virtual = buf->ops->Buffy_createVirtual(buf, 14 * 8);

    nextDissector->ops->Dissector_dissect(nextDissector, virtual, child);


    // memcpy(&truffle.eh.sourceMacAddress, p->eh->ether_src, 6);
    // memcpy(&truffle.eh.destMacAddress, p->eh->ether_dst, 6);
    //
    // truffle.eh.sourceMacAddress = (htobe64(truffle.eh.sourceMacAddress)) >> 4*4;
    // truffle.eh.destMacAddress = (htobe64(truffle.eh.destMacAddress)) >> 4*4;
    //
    //
    // //		printf("get: %016llX\n", data);
    //
    // // TODO put this in debug wrap if at all
    // printf("--------- %lld ---------\n", n);
    //
    // printf("src: %016lX\n", truffle.eh.sourceMacAddress);
    // printf("dest: %016lX\n", truffle.eh.destMacAddress);
    //
    // Dissector_t *nextDissector = this->ops->Dissector_getSub(this, buf->p->eh.ether_type);
    // ProtocolItem_t *child = tree->ops->ProtocolTree_branch(tree);
    //
    // return nextDissector->ops->Dissector_dissect(nextDissector, buf, child);

	//printf("dissecting very bigtime\n");
	// TODO implement

    return 0;
error:
    return -1;
	//return 0;
}
