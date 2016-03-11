
#include <stdlib.h>

#include "send/Truffle.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"

#include "dbg.h"

Truffle_t *Truffle_new(const struct ProtocolNode *protoTree) {

	Truffle_t *truffle = malloc(sizeof(Truffle_t));

	struct Value *etherDest = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "ether_dest");
	struct Value *etherSource = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "ether_source");
	struct Value *etherType = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "ether_type");

	debug("dest: %016lX", etherDest->val.uint64);
	debug("src: %016lX", etherSource->val.uint64);
	debug("type: %016X", etherType->val.uint16);
	//truffle->eh.sourceMacAddress = protoTree->ops->ProtocolTree_getNode(protoTree, "ether_source")->value.val.lval;
	// truffle->eh.destMacAddress = protoTree->ops->ProtocolTree_getNode(protoTree,"ether_dest")->value.val.lval;
	//truffle->eh.etherType = protoTree->ops->ProtocolTree_getNode(protoTree,"ether_type")->value.val.ival;

	//struct FieldInfo *fieldInfo = protoTree->ops->getField(ETHER_TYPE);

	// now i know : fieldInfo->len == 16;
	// and		  : fieldInfo->type == RAW;


	//truffle->frame.cycleCounter

	truffle->eh.destMacAddress = etherDest->val.uint64;
	truffle->eh.sourceMacAddress = etherSource->val.uint64;

	return truffle;
}
