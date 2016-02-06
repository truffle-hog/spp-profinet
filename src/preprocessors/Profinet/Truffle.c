
#include <stdlib.h>

#include "Truffle.h"

#include "dbg.h"

Truffle_t *Truffle_new(const ProtocolTree_t *protoTree) {

	Truffle_t *truffle = malloc(sizeof(Truffle_t));

	truffle->eh.sourceMacAddress = protoTree->ops->findData("ether_source");
	truffle->eh.destMacAddress = protoTree->ops->findData("ether_dest");
	truffle->eh.etherType = protoTree->ops->findData("ether_type");

	struct FieldInfo *fieldInfo = protoTree->ops->getField(ETHER_TYPE);
	
	// now i know : fieldInfo->len == 16;
	// and		  : fieldInfo->type == RAW;


	truffle->frame.cycleCounter



	return truffle;
}
