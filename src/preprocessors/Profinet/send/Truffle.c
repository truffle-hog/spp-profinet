
#include <stdlib.h>

#include "send/Truffle.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"

#include "util/ScopeAlloc.h"

#include "dbg.h"

Truffle_t *Truffle_new(const struct ProtocolNode *protoTree) {

//	Truffle_t *truffle = calloc(1, sizeof(Truffle_t));

	Truffle_t *truffle = packetScopeMalloc(sizeof(Truffle_t));

	struct Value *etherDest = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "ether_dest");
	struct Value *etherSource = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "ether_source");
	struct Value *etherType = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "ether_type");

	struct Value *nameOfStation = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "name_of_station");

	struct Value *serviceID = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "dcp_service_id");
	struct Value *serviceType = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "dcp_service_type");
	struct Value *serviceIDName = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "service_id_name");
	struct Value *serviceTypeName = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "service_type_name");
	struct Value *xID = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "dcp_xid");
	struct Value *responseDelay = protoTree->ops->ProtocolTree_getImportantValue(protoTree, "dcp_response_delay");


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

	// TODO check for real dcp -- this works for now though
	if (serviceType != NULL) {

		if (serviceType->val.uint8 == 0x00) {

			ssize_t nameOfStationLength = strlen(nameOfStation->val.string);
			nameOfStationLength = nameOfStationLength <= MAX_STRING_LEN ? nameOfStationLength : MAX_STRING_LEN;
			memcpy(&truffle->frame.val.dcp.blocks[0].val.deviceBlock.nameOfStation, nameOfStation->val.string, nameOfStationLength);
			truffle->frame.val.dcp.blocks[0].val.deviceBlock.nameOfStation[MAX_STRING_LEN - 1] = '\0';


			truffle->frame.val.dcp.responseDelay = responseDelay->val.uint16;
		}

		ssize_t idNameLength = strlen(serviceIDName->val.string);
		idNameLength = idNameLength <= MAX_STRING_LEN ? idNameLength : MAX_STRING_LEN;

		ssize_t typeNameLength = strlen(serviceTypeName->val.string);

		typeNameLength = typeNameLength <= MAX_STRING_LEN ? typeNameLength : MAX_STRING_LEN;



		truffle->frame.type = IS_DCP;

		truffle->frame.val.dcp.xID = xID->val.uint32;

		truffle->frame.val.dcp.serviceID = serviceID->val.uint8;
		memcpy(&truffle->frame.val.dcp.serviceIDName, serviceIDName->val.string, idNameLength);
		truffle->frame.val.dcp.serviceIDName[MAX_STRING_LEN - 1] = '\0';

		memcpy(&truffle->frame.val.dcp.serviceTypeName, serviceTypeName->val.string, typeNameLength);
		truffle->frame.val.dcp.serviceTypeName[MAX_STRING_LEN - 1] = '\0';
		truffle->frame.val.dcp.serviceType = serviceType->val.uint8;



		truffle->frame.val.dcp.blocks[0].type = IS_DEVICE;



	}


	return truffle;
}
