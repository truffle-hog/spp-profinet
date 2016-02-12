/**
 * @file
 * @brief The interface for Buffy.
 */


#include <stdint.h>

#include "Buffy.h"
#include "Buffy-int.h"

#include "plugbase.h"

#include "dbg.h"

/**
 * @see Buffy_new
 */
Buffy_t *Buffy_new(Packet *p) {

	Buffy_t *buffy = malloc(sizeof(Buffy_t));
	check_mem(buffy);

	buffy->p = p;

	buffy->data = buffy->p->data;



	return buffy;
error:
	return NULL;

}

/**
 * @see Buffy_free
 */
void Buffy_free(Buffy_t *buffy) {

	free(buffy);

}

/**
 * @see Buffy_getBits8
 */
uint8_t Buffy_getBits8(Buffy_t *this, unsigned int bitOffset,
    const int noOfBits) {

	check(noOfBits <= 8, "number of bits has to be between 1 and 8 bits.");
	check(noOfBits > 0, "number of bits has to be between 1 and 8 bits.");


	if (noOfBits > 8 || noOfBits <= 0) {

		goto error;
	}

	uint8_t bitmask = (1 << noOfBits) - 1;


	return ((uint8_t)this->data[bitOffset]) & bitmask;

error:
	return -1;
}

/**
 * @see Buffy_getBits16
 */
uint16_t Buffy_getBits16(Buffy_t *this, unsigned int bitOffset,
    const int noOfBits, const enum Encoding encoding);

/**
 * @see Buffy_getBits32
 */
uint32_t Buffy_getBits32(Buffy_t *this, unsigned int bitOffset,
    const int noOfBits, const enum Encoding encoding);

/**
 * @see Buffy_getBits64
 */
uint64_t Buffy_getBits64(Buffy_t *this, unsigned int bitOffset,
    const int noOfBits, const enum Encoding encoding);

