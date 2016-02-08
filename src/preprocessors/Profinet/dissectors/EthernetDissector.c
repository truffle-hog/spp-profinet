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

#include "Dissector-int.h"
#include "Dissector.h"
#include "ProtocolTree.h"
#include "Buffy.h"

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
    (uint64_t) 0x8892,
    (uint64_t) 0x8892,
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

    struct FieldInfo *etherInfo = malloc(sizeof(struct FieldInfo));
    etherInfo->name = "ethernet_header";

    tree->ops->ProtocolTree_setFieldInfo(tree, etherInfo);

    ProtocolItem_t *destBranch = tree->ops->ProtocolTree_branch(tree);
    struct FieldInfo *destInfo = malloc(sizeof(struct FieldInfo));
    destInfo->headerType = ETHERNET;
    destInfo->key = "destination_mac_address";
    destInfo->name = "";
    destInfo->data = (void*) malloc(sizeof(uint64_t));
    destInfo->dataType = UINT;
    destInfo->length = 64;

    memcpy(destInfo->data, buf->p->eh->ether_dst, 6);
    destInfo->data = (htobe64(destInfo->data)) >> 16;

    destBranch->ops->ProtocolTree_setFieldInfo(destBranch);

    printf("dest: %016lX\n", (uint64_t) *destInfo->data);


    ProtocolItem_t *source = tree->ops->ProtocolTree_branch(tree);
    struct FieldInfo *srcInfo = malloc(sizeof(struct FieldInfo));




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
