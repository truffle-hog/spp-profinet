#ifndef __PNRT_DISSECTOR_H__
#define __PNRT_DISSECTOR_H__

/**
 * @file
 * @brief PNRTDissector header.
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

/**
 * @see Dissector_new
 */
Dissector_t *PNRTDissector_new();

/**
 * @see Dissector_free
 */
void PNRTDissector_free(Dissector_t *dissector);

/**
 * @see Dissector_dissect
 */
int PNRTDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

#endif
