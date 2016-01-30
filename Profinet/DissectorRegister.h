
#ifndef __DISSECTOR_REGISTER_H__
#define __DISSECTOR_REGISTER_H__

/**
* The dissector register is used to register dissectors to intervals. Thereby making
* it possible to dissect a package while using certain data ranges for calling a next
* dissector that is mapped to the given data.
*
*/
struct DissectorRegister;
typedef struct DissectorRegister DissectorRegister_t;


/**
* Creates a new DissectorRegister with the given operations. This Function is the
* interface constructor for every DissectorRegister implementation.
*
* @param ops the pointer to the operations used for this DissectorRegister
* \return a pointer to the created DissectorRegister
*/
DissectorRegister_t * DissectorRegister_new(const struct DissectorRegister_ops *ops);


/**
* Frees the given DissectorRegister.
*/
void DissectorRegister_free(DissectorRegister_t *this);


/**
* Inserts a new Dissector for the given interval. If a dissector is allready mapped
*
* @param this the calling register
* @param the interval the given dissector will be mapped to
* @param dissector the dissector to be inserted
*
* \return NULL if there is no previous dissector registered within the given interval,
*         otherwise overwrites the old dissector and returns it
*/
Dissector_t * DissectorRegister_insert(DissectorRegister_t *this,
                           DissectorRegister_t *dissector);

/**
* Returns the sub DissectorRegister that is register for the given unsigned long.
*
* @param this the DissectorRegister calling
* @param data the value for looking up in the DissectorRegister
*
* \return the registered Dissector if any, NULL otherwise
*/
Dissector_t * DissectorRegister_get(DissectorRegister_t *this, uint64_t data);

#endif
