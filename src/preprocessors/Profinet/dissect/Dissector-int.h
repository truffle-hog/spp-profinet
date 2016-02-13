/**
 * @file
 *
 * @brief This Header discribes the internal structure of the Dissector type, it defines
 * the basic interface for operations.
 */

#ifndef __DISSECTOR_INT_H__
#define __DISSECTOR_INT_H__

#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>

#include "dissect/DissectorRegister.h"
#include "dissect/Dissector.h"

struct Dissector;

/**
 * @brief The operations that can be called by a Dissector.
 *
 */
struct Dissector_ops {

  /**
  * @brief Returns the number of subdissectors in this dissector.
  *
  * @return the number of sub-dissectors in this dissector
  */
  size_t Dissector_size;

  /**
  * @brief Returns the lower bound this subdissector is being called upon.
  *
  * @return the lower bound this subdissector is being called upon
  */
  uint64_t Dissector_lower;

  /**
  * @brief Returns the upper bound this subdissector is being called upon.
  *
  * @return the upper bound this subdissector is being called upon
  */
  uint64_t Dissector_upper;
  /**
  * @brief Returns the number of subdissectors in this dissector.
  *
  * @return the number of sub-dissectors in this dissector
  */
  void (*Dissector_free)(Dissector_t *dissector);

  /**
  * @brief Registers a given sub dissector on this dissector.
  *
  * @param this the dissector to register the subDissector on
  * @param subDissector the dissector to be registered as sub
  *
  * @return NULL if there was no other dissector registered for the given interval
  *         otherwise the existing Dissector will be overwritten and returned.
  */
  Dissector_t * (*Dissector_registerSub)(Dissector_t *this, Dissector_t *subDissector);

  /**
  * @brief Returns the sub dissector that is register for the given unsigned long.
  *
  * @param this the dissector calling Dissector_getSub
  * @param data the value for looking up in the dissector register
  *
  * @return the registered sub dissector if any, NULL otherwise
  */
  Dissector_t * (*Dissector_getSub)(Dissector_t *this, uint64_t data);

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
  int (*Dissector_dissect)(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);
};

/**
 * @brief Used to dissect certain data ranges within a package.
 *
 * Dissector are used to dissect certain ranges of data in a network package,
 * while having the possibility to link to further dissectors when the dissection
 * of the desired range is complete. Further Dissectors are linked by using an
 * internal DissectorRegister.
 *
 * -> It is possible to link several Dissectors together building a tree of dissectors
 * and subdissectors that call each other when their dissection part is completed.
 */
struct Dissector {

  /** Whether this dissector was initialized. **/
  bool initialized;

  DissectorRegister_t *dissectorRegister;

  /** The dissectors operations. **/
  const struct Dissector_ops *ops;
  /** The dissector this dissector has been called from. **/
  struct Dissector *calling;
};

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
Dissector_t * Dissector_new(const struct Dissector_ops *ops);


#endif
