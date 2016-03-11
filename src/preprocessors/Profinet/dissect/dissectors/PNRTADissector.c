/**
 * @file
 * @brief PNRTADissector implementation.
 *
 * This Dissector is the 0x8892 toplevel dissector, which will be followed by frame
 * and block dissectors.
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"
#include "dissect/dissectors/PNRTADissector.h"
#include "dissect/dissectors/DCPDissector.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"
#include "dissect/buffer/Buffy.h"
#include "dissect/buffer/Buffy-int.h"

#include "dbg.h"

#define FRAME_ID_DCP_HELLO      0xfefc
#define FRAME_ID_DCP_GETORSET   0xfefd
#define FRAME_ID_DCP_IDENT_REQ  0xfefe
#define FRAME_ID_DCP_IDENT_RES  0xfeff

/**
 * @brief The Dissector for Profi Real Time IO 0x8892.
 *
 */
struct PNRTADissector {
  /** @brief Houses a Dissector internally for safe type casting. **/
  struct Dissector dissector;

};

void PNRTADissector_free(Dissector_t *dissector);

int PNRTADissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

/**
 * @see Dissector_ops
 */
static const struct Dissector_ops PNRTADissectorOverride_ops = {

  sizeof(struct PNRTADissector), /* size */
  (uint64_t) 0xFE00,
  (uint64_t) 0xFEFF,
  PNRTADissector_free,
  Dissector_registerSub,
  Dissector_getSub,
  PNRTADissector_dissect
};

void PNRTADissector_initializeSubDissectors(Dissector_t *this) {

    Dissector_t *dcpDissector = DCPDissector_new();

    this->ops->Dissector_registerSub(this, dcpDissector);
}

/**
 * @see Dissector_new
 */
Dissector_t *
PNRTADissector_new() {

    struct PNRTADissector *pnrtaDissector = (struct PNRTADissector *) Dissector_new(&PNRTADissectorOverride_ops);
    check_mem(pnrtaDissector);

    PNRTADissector_initializeSubDissectors(&pnrtaDissector->dissector);

    return (Dissector_t*) pnrtaDissector;

error:
    return NULL;

}

/**
 * @see Dissector_free
 */
void PNRTADissector_free(Dissector_t *dissector) {

	struct PNRTADissector *this = (struct PNRTADissector*) dissector;

	free(this);
//	this->dissector.ops->Dissector_free(&this->dissector);

	(void) dissector;
	// TODO implement
}

/**
 * @see Dissector_dissect
 */
int PNRTADissector_dissect(Dissector_t *this, Buffy_t *buf, struct ProtocolNode *node) {

	(void) this;
	(void) buf;
	(void) node;

    struct Value pnrta;
	pnrta.type = is_string;
	pnrta.val.string = "Acyclic Realtime";
	pnrta.length = strlen(pnrta.val.string);

    ProtocolItem_t *pnrtaItem = node->ops->ProtocolTree_branch(node, "frame", pnrta);
    check_mem(pnrtaItem);

    uint16_t u16FrameID = buf->ops->Buffy_getBits16(buf, 0);



    Dissector_t *nextDissector = this->ops->Dissector_getSub(this, u16FrameID);
    check(
			nextDissector != NULL, "there is no acyclic dissector registered for frameID: 0x%04X",
			u16FrameID);

    return nextDissector->ops->Dissector_dissect(nextDissector, buf, pnrtaItem);

error:
	return -1;
}
