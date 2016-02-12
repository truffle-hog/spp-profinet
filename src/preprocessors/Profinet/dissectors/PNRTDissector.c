/**
 * @file
 * @brief PNRTDissector implementation.
 *
 * This Dissector is the 0x8892 toplevel dissector, which will be followed by frame
 * and block dissectors.
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include "Dissector-int.h"
#include "Dissector.h"
//#include "PNRTADissector.h"

#include "ProtocolTree.h"
#include "Buffy.h"
#include "Buffy-int.h"


#include "dbg.h"

/**
 * @brief The Dissector for Profi Real Time IO 0x8892.
 *
 */
struct PNRTDissector {
  /** @brief Houses a Dissector internally for safe type casting. **/
  struct Dissector dissector;

};

void PNRTDissector_free(Dissector_t *dissector);

int PNRTDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

/**
 * @see Dissector_ops
 */
static const struct Dissector_ops PNRTDissectorOverride_ops = {

  sizeof(struct PNRTDissector), /* size */
  0x8892,
  0x8892,
  PNRTDissector_free,
  Dissector_registerSub,
  Dissector_getSub,
  PNRTDissector_dissect
};

void PNRTDissector_initializeSubDissectors(Dissector_t *this) {

    //Dissector_t *pnrtaDissector = PNRTADissector_new();

    //this->ops->Dissector_registerSub(this, pnrtaDissector);

}

/**
 * @see Dissector_new
 */
Dissector_t *
PNRTDissector_new() {


    struct PNRTDissector *pnrtDissector;

    pnrtDissector = malloc(sizeof(struct PNRTDissector));
    check_mem(pnrtDissector);

    Dissector_t *disser = Dissector_new(&PNRTDissectorOverride_ops);
    check_mem(disser);

    pnrtDissector->dissector = *disser;

    PNRTDissector_initializeSubDissectors((Dissector_t*) pnrtDissector);

    return (Dissector_t*) pnrtDissector;

error:
    return NULL;

}

/**
 * @see Dissector_free
 */
void PNRTDissector_free(Dissector_t *dissector) {

	struct PNRTDissector *this = (struct PNRTDissector*) dissector;

	free(this);
//	this->dissector.ops->Dissector_free(&this->dissector);

	(void) dissector;
	// TODO implement
}

/**
 * @see Dissector_dissect
 */
int PNRTDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree) {

    //uint16_t frameID = buf->ops->Buffy_getBits16(buf, 0, 16, 0);

	debug("dissecting very bigtime");
	// TODO implement

	return 0;
}
