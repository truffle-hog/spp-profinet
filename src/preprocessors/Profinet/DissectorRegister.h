/**
 * @file
 * @brief The interface for dissector registers.
 *
 * The dissector register is used to register dissectors to intervals. Thereby making
 * it possible to dissect a package while using certain data ranges for calling a next
 * dissector that is mapped to the given data.
 */

#ifndef __DISSECTOR_REGISTER_H__
#define __DISSECTOR_REGISTER_H__

#include "Dissector.h"

struct DissectorRegister;
typedef struct DissectorRegister DissectorRegister_t;



/**
 * @brief Frees the given DissectorRegister.
 */
void DissectorRegister_free(DissectorRegister_t *this);


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
Dissector_t * DissectorRegister_get(DissectorRegister_t *this, uint64_t data);

#endif
