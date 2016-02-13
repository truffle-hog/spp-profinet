/**
 * @file
 * @brief The interface for Buffy.
 */


#include <stdint.h>

#include "Buffy.h"
#include "Buffy-int.h"

#include "plugbase.h"

#include "dbg.h"

static const struct Buffy_ops BuffyOverride_ops = {

	Buffy_free,
	Buffy_createVirtual,
	Buffy_getBits8,
	Buffy_getBits16,
	Buffy_getBits32,
	Buffy_getBits64,
	Buffy_getNBits8,
	Buffy_getNBits16,
	Buffy_getNBits32,
	Buffy_getNBits64
};

/**
 * @see Buffy_new
 */
Buffy_t *Buffy_new(Packet *p) {

	Buffy_t *buffy = malloc(sizeof(Buffy_t));
	check_mem(buffy);

	buffy->p = p;
	buffy->ops = &BuffyOverride_ops;

	buffy->data = p->pkt;
	buffy->position = 0;
	buffy->parent = NULL;
	buffy->next = NULL;



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

Buffy_t *Buffy_createVirtual(Buffy_t *this, unsigned int bitOffset) {

	check(this != NULL, "The calling buffer must not be null");
	check(bitOffset % 8 == 0, "a bitoffset that is not 8 bit alligned is not supported yet");

	unsigned int byteOffset = bitOffset / 8;

	Buffy_t *buffy = malloc(sizeof(Buffy_t));
	check_mem(buffy);

	buffy->p = this->p;
	buffy->ops = this->ops;
	buffy->data = this->data + byteOffset;
	buffy->parent = this;
	buffy->raw = this->raw;
	buffy->position = bitOffset;

	this->next = buffy;

	return buffy;

error:
	return NULL;
}

uint8_t Buffy_getBits8(Buffy_t *this, unsigned int byteOffset) {

	check(this != NULL, "calling buffer must not be null");

	return ((uint8_t)this->data[byteOffset]);
error:
	return -1;
}

uint16_t Buffy_getBits16(Buffy_t *this, unsigned int byteOffset) {

	check(this != NULL, "calling buffer must not be null");

	uint16_t value = *((uint16_t*) (this->data + byteOffset));
	return htobe16(value);
error:
	return -1;
}

uint32_t Buffy_getBits32(Buffy_t *this, unsigned int byteOffset) {

	check(this != NULL, "The calling buffer must not be null.");

	uint32_t value = *((uint32_t*) (this->data + byteOffset));
	return htobe32(value);
error:
	return -1;
}

uint64_t Buffy_getBits64(Buffy_t *this, unsigned int byteOffset) {

	check(this != NULL, "The calling buffer must not be null.");

	uint64_t value = *((uint64_t*) (this->data + byteOffset));
	return htobe64(value);
error:
	return -1;
}

/**
 * @see Buffy_getBits8
 */
uint8_t Buffy_getNBits8(Buffy_t *this, unsigned int bitOffset,
    const int noOfBits) {

	check(this != NULL, "The calling buffer must not be null.");
	check(noOfBits <= 8, "number of bits has to be between 1 and 8 bits.");
	check(noOfBits > 0, "number of bits has to be between 1 and 8 bits.");

	// the byte offset from bit offset
	unsigned int byteOffset = bitOffset / 8;
	// the bitoffset from the byte
	unsigned int bitOffsetFromByte = bitOffset % 8;

	// create bitmask
	unsigned char bitmask = -1;

	// e.g. noofBits = 3 and bitcount = 2
	// ~(1111 1111 >> 3) = ~(0001 1111) = 1110 0000
	bitmask = (~(bitmask >> noOfBits));
	// 1110 0000 >> 2 = 0011 1000
	bitmask = bitmask >> bitOffsetFromByte;

	unsigned char value = (unsigned char) this->data[byteOffset] & bitmask;

	// shift the extracted bits to the correct position
	value = value >> (8 - noOfBits - bitOffsetFromByte);

	return value;

	//return (((unsigned char)this->data[byteOffset]) & bitmask) >> (8 - noOfBits - bitOffsetFromByte);

error:
	return -1;
}

/**
 * @see Buffy_getBits16
 */
uint16_t Buffy_getNBits16(Buffy_t *this, unsigned int bitOffset,
    const int noOfBits, const enum Encoding encoding) {

	check(noOfBits <= 16, "number of bits has to be smaller or equally to 16 bits.");
	check(noOfBits > 0, "number of bits cannot be zero. needs to be positive");

	check(bitOffset % 8 == 0, "a bitoffset that is not 8 bit alligned is not supported yet");
	//TODO Check this function for errors!!!!



	unsigned int byteOffset = bitOffset / 8;
	unsigned int bitCount = bitOffset % 8;

	// cast the datapointer to 16 bit unsigned integer Pointer and dereference it
	uint16_t value = *((uint16_t*) (this->data + byteOffset));

	// then care about the encoding
	if (encoding == ENC_LITTLE_ENDIAN) {
		value = htobe16(value);
	}

	// the care about getting the right number of bits
	uint16_t bitmask = ~(~0 >> noOfBits) >> bitCount;

	return value & bitmask;

	//TODO implement
	return 0;
error:
	return -1;

}

/**
 * @see Buffy_getBits32
 */
uint32_t Buffy_getNBits32(Buffy_t *this, unsigned int bitOffset,
    const int noOfBits, const enum Encoding encoding) {

	check(bitOffset % 8 == 0, "a bitoffset that is not 8 bit alligned is not supported yet");

	check(noOfBits <= 16, "number of bits has to be smaller or equally to 16 bits.");
	check(noOfBits > 0, "number of bits cannot be zero. needs to be positive");

	//TODO implement
	return 0;
error:
	return -1;
}

/**
 * @see Buffy_getBits64
 */
uint64_t Buffy_getNBits64(Buffy_t *this, unsigned int bitOffset,
    const int noOfBits, const enum Encoding encoding) {

	check(bitOffset % 8 == 0, "a bitoffset that is not 8 bit alligned is not supported yet");

	check(noOfBits <= 16, "number of bits has to be smaller or equally to 16 bits.");
	check(noOfBits > 0, "number of bits cannot be zero. needs to be positive");

	//TODO implement
	return 0;
error:
	return -1;
}
