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
#include "dissect/tree/ProtocolTree.h"
#include "dissect/buffer/Buffy.h"

#include "dbg.h"

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

/**
 * @see Dissector_new
 */
Dissector_t *
PNRTADissector_new() {


    struct PNRTADissector *PNRTADissector;

    PNRTADissector = malloc(sizeof(struct PNRTADissector));
    check_mem(PNRTADissector);

    Dissector_t *disser = Dissector_new(&PNRTADissectorOverride_ops);
    check_mem(disser);

    PNRTADissector->dissector = *disser;

    return (Dissector_t*) PNRTADissector;

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
int PNRTADissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree) {

	(void) this;
	(void) buf;
	(void) tree;

    uint16_t frameID = buf->ops->Buffy_get_bits16(buf, 0, 16, 0);

    FieldInfo *frameInfo = FrameInfo_new(buf->packet);
    tree->ops->ProcolTree_setFieldInfo(tree, framInfo);

    Dissector_t *nextDissector = this->ops->Dissector_getSub(this, frameInfo->headerType);

    ProtocolItem_t *child = tree->ops->ProtocolTree_branch(tree);

    nextDissector->ops->Dissector_dissect(nextDissector, buf, child);

	printf("dissecting very bigtime\n");
	// TODO implement

	return 0;
}
