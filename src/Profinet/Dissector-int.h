
#ifndef __DISSECTOR_INT_H__
#define __DISSECTOR_INT_H__

/** @file Dissector-int.h
 *
 * This Header discribes the internal structure of the Dissector type. It defines
 * the basic interface for operations.
 */
struct Dissector;

struct Dissector_ops {

  /**
  * @brief Returns the number of subdissectors in this dissector.
  *
  * @return the number of sub-dissectors in this dissector
  */
  size_t Dissector_size;
  unit64_t Dissector_lower;
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
  Dissector_t * (*Dissector_registerSub)(Dissector_t *this, Dissector_t *subDissector, Interval interval);
  Dissector_t * (*Dissector_getSub)(Dissector_t *this, uint64_t data);
  int (*Dissector_dissect)(Dissector_t *this, Buffer_t *buf, ProtocolTree_t *tree);
};

struct Dissector {

  /** Whether this dissector was initialized. **/
  bool initialized;

  /** The dissectors operations. **/
  const struct Dissector_ops *ops;
  /** The dissector this dissector has been called from. **/
  Dissector_t *calling;
};

Dissector_t *Dissector_new(const struct Dissector_ops *ops);

#endif
