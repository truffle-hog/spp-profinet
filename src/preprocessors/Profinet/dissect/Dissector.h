/**
 * @file Dissector.h
 *
 * @brief The Basic Dissector abstraction (Interface).
 *
 * The Base Dissector abstraction. Every implementation of a Dissector will use
 * and implement the operations described in this interface.
 * Dissector are used to dissect certain ranges of data in a network package,
 * while having the possibility to link to further dissectors when the dissection
 * of the desired range is complete.
 *
 * -> It is possible to link several Dissectors together building a tree of dissectors
 * and subdissectors that call each other when their dissection part is completed.
 */

#ifndef __DISSECTOR_H__
#define __DISSECTOR_H__

#include <stdint.h>

#include "tree/ProtocolTree.h"
#include "Buffy.h"

struct Dissector;
typedef struct Dissector Dissector_t;


/**
 * @brief Creates a new Dissector with the given operations.
 *
 * This Function is the interface constructor for every Dissector implementation.
 * Calling this function will initialize the dissector correctly and fill the needed
 * data within the Dissector structure.
 *
 * @param ops the pointer to the operations used for this dissector
 * @return a pointer to the created dissector
 */
//Dissector_t * Dissector_new(const struct Dissector_ops *ops);


/**
 * Frees the given dissector.
 */
void Dissector_free(Dissector_t *dissector);

// /**
//  * @brief Function that is called internally on initializations of the subdissectors.
//  * This Function should never be called from outside the dissector itself
//  */
// void Dissector_initializeSubDissectors(Dissector_t *this);

/**
 * @brief Registers a given sub dissector on this dissector.
 *
 * @param this the dissector to register the subDissector on
 * @param subDissector the dissector to be registered as sub
 *
 * @return NULL if there was no other dissector registered for the given interval
 *         otherwise the existing Dissector will be overwritten and returned.
 */
Dissector_t * Dissector_registerSub(Dissector_t *this,
                           Dissector_t *subDissector);

/**
 * @brief Returns the sub dissector that is register for the given unsigned long.
 *
 * @param this the dissector calling Dissector_getSub
 * @param data the value for looking up in the dissector register
 *
 * @return the registered sub dissector if any, NULL otherwise
 */
Dissector_t * Dissector_getSub(Dissector_t *this, uint64_t data);

/**
 * @brief Dissects the package the given buffer is pointing to.
 *
 * @param this the calling Dissector
 * @param buf the buffer pointing to the package data currently being processed
 * @param tree the tree strcture to save the package data in
 *
 * @return 0 if the dissection was successful wihtout any failures,
 *         -1 if it was a faulty package. The fault flag will be set in the
 *         ProtocolTree accordingly
 */
int Dissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

#endif
