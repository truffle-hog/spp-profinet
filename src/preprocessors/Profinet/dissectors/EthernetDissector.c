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

#include "ProtocolTypes.h"

#include "Dissector-int.h"
#include "Dissector.h"
#include "ProtocolTree.h"
#include "ProtocolTree-int.h"

#include "Buffy.h"
#include "Buffy-int.h"

#include "dbg.h"

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
EthernetDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree) {

	struct Value dest;
	dest.length = 64;
	dest.val.lval = 0;
	dest.type = is_long;

    memcpy(&dest.val.lval, buf->p->eh->ether_dst, 6);
    dest.val.lval = (htobe64(dest.val.lval)) >> 16;

	tree->ops->ProtocolTree_branch(tree, "ether_dest", dest);

    printf("dest: %016lX\n", (uint64_t) dest.val.lval);

	struct Value source;
	source.length = 64;
	source.val.lval = 0;
	source.type = is_long;

	memcpy(&source.val.lval, buf->p->eh->ether_src, 6);
	source.val.lval = (htobe64(source.val.lval)) >> 16;	

    tree->ops->ProtocolTree_branch(tree, "ether_source", source);

	printf("src: %016lX\n", (uint64_t) dest.val.lval);

	struct Value ethertype;
	ethertype.length = 16;
	ethertype.val.ival = buf->p->eh->ether_type;
	ethertype.type = is_int;

	tree->ops->ProtocolTree_branch(tree, "ether_type", ethertype);

	printf("type: %016X\n", (uint16_t) ethertype.val.ival);
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

	//return 0;
}
