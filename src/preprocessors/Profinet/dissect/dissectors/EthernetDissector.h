/**
 * @file
 * @brief The interface for the PacketDissector.
 */

#ifndef __ETHERNET_DISSECTOR_H__
#define __ETHERNET_DISSECTOR_H__

#include <stdlib.h>
#include <stdio.h>

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"
#include "dissect/tree/ProtocolTree.h"
#include "dissect/buffer/Buffy.h"

/**
 * @see Dissector_new
 */
Dissector_t *EthernetDissector_new();

/**
 * @see Dissector_free
 */
void EthernetDissector_free(Dissector_t *dissector);

/**
 * @see Dissector_dissect
 */
int EthernetDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);


#endif
