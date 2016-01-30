/**
 * @file
 *
 * @brief The internal structure of a dissector register. Including the operation structure
 * and fields.
 */

#ifndef __DISSECTOR_REGISTER_INT_H__
#define __DISSECTOR_REGISTER_INT_H__

struct DissectorRegister;

/**
 * @brief The operations that can be called by a DissectorRegister.
 *
 */
struct DissectorRegister_ops {

  /**
  * @brief Returns the number dissectors registered.
  *
  * @return the number of dissectors in this register
  */
  size_t DissectorRegister_size;

  /**
  * @brief Frees the given DissectorRegister.
  */
  void *(*DissectorRegister_free)(DissectorRegister_t *this);


  /**
  * @brief Inserts a new Dissector.
  *
  * The new dissector will be inserted into the DissectorRegister by obtaining its
  * lower and upper identifier bounds and mapping it accordingly.
  *
  * @param this the calling register
  * @param dissector the dissector to be inserted
  *
  * @return NULL if there is no previous dissector registered within its interval,
  *         otherwise overwrites the old dissector and returns it
  */
  Dissector_t * DissectorRegister_insert(DissectorRegister_t *this,
                             Dissector_t *dissector);

  /**
  * @brief Returns the Dissector that is registered for the given unsigned long.
  *
  * @param this the DissectorRegister calling
  * @param data the value for looking up in the DissectorRegister
  *
  * @return the registered Dissector if any, NULL otherwise
  */
  Dissector_t *(*DissectorRegister_get)(DissectorRegister_t *this, uint64_t data);

};

/**
 * @brief The datastructure for registering Dissectors on their specific intervals.
 *
 * The dissector register is used to register dissectors to intervals. Thereby making
 * it possible to dissect a package while using certain data ranges for calling a next
 * dissector that is mapped to the given data.
 */
struct DissectorRegister {

  /** Whether this dissector register is initialized. **/
  bool initialized;
  /** The dissector register operations. **/
  const struct DissectorRegister_ops *ops;
};

Dissector_t *DissectorRegister_new(const struct DissectorRegister_ops *ops);

#endif
