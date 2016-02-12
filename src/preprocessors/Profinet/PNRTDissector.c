/**
 * @file
 * @brief PNRTDissector implementation.
 *
 * This Dissector is the 0x8892 toplevel dissector, which will be followed by frame
 * and block dissectors.
 *
 */
#include <stdlib.h>

#include "Dissector-int.h"
#include "Dissector.h"
#include "ProtocolTree.h"
#include "Buffy.h"

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
  (uint64_t) 0x8892,
  (uint64_t) 0x8892,
  PNRTDissector_free,
  NULL,
  NULL,
  PNRTDissector_dissect
};

/**
 * @see Dissector_new
 */
Dissector_t *
PNRTDissector_new() {

  Dissector_t *dissector;

  dissector = Dissector_new(&PNRTDissectorOverride_ops);

  return dissector;
}

/**
 * @see Dissector_free
 */
void PNRTDissector_free(Dissector_t *dissector) {

	(void) dissector;
	// TODO implement
}

/**
 * @see Dissector_dissect
 */
int PNRTDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree) {

	(void) this;
	(void) buf;
	(void) tree;
	// TODO implement

	return 0;
}
