
#include <stdlib.h>

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"

#include "dissect/DissectorRegister-int.h"
#include "dissect/DissectorRegister.h"

#include "dbg.h"

/**
 * @see Sender_ops
 */
static const struct DissectorRegister_ops DissectorRegisterOverride_ops = {
	0,
	DissectorRegister_free,
	DissectorRegister_insert,
	DissectorRegister_get
};

/**
 * @see DissectorRegister_new
 */
DissectorRegister_t * DissectorRegister_new() {

	// TODO delete debuing messges

	DissectorRegister_t *dRegister = malloc(sizeof(DissectorRegister_t));
	check_mem(dRegister);

	dRegister->ops = &DissectorRegisterOverride_ops;

	dRegister->size = 0;

	return dRegister;

error:

	return NULL;

}


/**
 * @see DissectorRegister_free
 */
void DissectorRegister_free(DissectorRegister_t *this) {

	unsigned long i = 0;

	for (i = 0; i < this->size; i++) {

		this->dissectorList[i]->ops->Dissector_free(this->dissectorList[i]);
	}

	this->size = 0;
	free(this);
}


/**
 */
Dissector_t * DissectorRegister_insert(DissectorRegister_t *this,
                           Dissector_t *dissector) {

	printf("low insert: %016lX\n", dissector->ops->Dissector_lower);
	printf("high insert: %016lX\n", dissector->ops->Dissector_upper);

	this->lowList[this->size] = dissector->ops->Dissector_lower;
	this->highList[this->size] = dissector->ops->Dissector_upper;
	this->dissectorList[this->size] = dissector;

	this->size++;

	return NULL;
}

/**
 *
 */
Dissector_t * DissectorRegister_get(DissectorRegister_t *this, uint64_t data) {

	unsigned long i = 0;

	// printf("get: %016llX\n", data);
	//
	// printf("in register of size: %ld", this->size);

	for (i = 0; i < this->size; i++) {

		if ((this->lowList[i] <= data) && (data <= this->highList[i])) {


			return this->dissectorList[i];

		}

	}

	return NULL;
}
