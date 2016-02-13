/**
 * @file
 * @brief The interface for the PacketDissector.
 */

#ifndef __PACKET_DISSECTOR_H__
#define __PACKET_DISSECTOR_H__


#include <stdlib.h>
#include <stdio.h>

#include "Dissector.h"
#include "tree/ProtocolTree.h"
#include "Buffy.h"

/**
 * @see Dissector_new
 */
Dissector_t *PacketDissector_new();

/**
 * @see Dissector_free
 */
void PacketDissector_free(Dissector_t *dissector);

/**
 * @see Dissector_dissect
 */
int PacketDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

#endif
