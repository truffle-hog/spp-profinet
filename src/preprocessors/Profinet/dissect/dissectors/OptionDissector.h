#ifndef __OPTION_DISSECTOR_H__
#define __OPTION_DISSECTOR_H__

/**
 * @file
 * @brief OptionDissector header.
 *
 * This Dissector ist the dissector for asynchronous real time
 *
 */


#include <stdlib.h>
#include <stdio.h>

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"
#include "dissect/tree/ProtocolTree.h"
#include "dissect/buffer/Buffy.h"

#define OPTION_DISSECTOR_ID      0xfffffffffffffffe
/**
 * @see Dissector_new
 */
Dissector_t *OptionDissector_new();

/**
 * @see Dissector_free
 */
void OptionDissector_free(Dissector_t *dissector);

/**
 * @see Dissector_dissect
 */
int OptionDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

#endif
