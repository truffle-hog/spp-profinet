
#include <stdlib.h>

#include "Truffle.h"

#include "ProtocolTree.h"
#include "ProtocolTree-int.h"

#include "dbg.h"

Truffle_t *Truffle_new(const struct ProtocolNode *protoTree) {

	Truffle_t *truffle = malloc(sizeof(Truffle_t));

	ProtocolItem_t *etherDest = protoTree->ops->ProtocolTree_getNode(protoTree, "ether_dest");
	ProtocolItem_t *etherSrc = protoTree->ops->ProtocolTree_getNode(protoTree, "ether_source");
	ProtocolItem_t *etherType = protoTree->ops->ProtocolTree_getNode(protoTree, "ether_type");

	debug("dest: %016lX", (uint64_t) etherDest->value.val.uint64);
	debug("src: %016lX", (uint64_t) etherSrc->value.val.uint64);
	debug("type: %016X", (uint16_t) etherType->value.val.uint16);
	//truffle->eh.sourceMacAddress = protoTree->ops->ProtocolTree_getNode(protoTree, "ether_source")->value.val.lval;
	// truffle->eh.destMacAddress = protoTree->ops->ProtocolTree_getNode(protoTree,"ether_dest")->value.val.lval;
	//truffle->eh.etherType = protoTree->ops->ProtocolTree_getNode(protoTree,"ether_type")->value.val.ival;

	//struct FieldInfo *fieldInfo = protoTree->ops->getField(ETHER_TYPE);

	// now i know : fieldInfo->len == 16;
	// and		  : fieldInfo->type == RAW;


	//truffle->frame.cycleCounter



	return truffle;
}
