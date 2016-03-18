/**
 * @file
 * @brief The interface for Buffy.
 */

#ifndef __BUFFY_H__
#define __BUFFY_H__

#include <stdint.h>



enum Encoding;

struct Buffy;
typedef struct Buffy Buffy_t;


/**
 * @brief Frees the given buffer from memory.
 *
 * @param buffy the buffer to be freed
 */
void Buffy_free(Buffy_t *buffy);

/**
 * @brief Creates a new virtual buffer from the current buffer that starts at the given bit offset
 *
 * @param this the calling Buffer
 * @param bitOffset the bit offset the new buffer starts at
 */
Buffy_t *Buffy_createVirtual(Buffy_t *this, int bitOffset);

uint8_t *Buffy_copyNBytes(Buffy_t *this, uint8_t *dest, int byteCount, int byteOffset);

uint8_t Buffy_getBits8(Buffy_t *this, int byteOffset);

uint16_t Buffy_getBits16(Buffy_t *this, int byteOffset);

uint32_t Buffy_getBits32(Buffy_t *this, int byteOffset);

uint64_t Buffy_getBits64(Buffy_t *this, int byteOffset);

uint8_t Buffy_getBitsWalk8(Buffy_t *this, int *byteOffset);

uint16_t Buffy_getBitsWalk16(Buffy_t *this, int *byteOffset);

uint32_t Buffy_getBitsWalk32(Buffy_t *this, int *byteOffset);

uint64_t Buffy_getBitsWalk64(Buffy_t *this, int *byteOffset);

/**
 * @brief Get 1 - 8 bits returned in a uint8.
 *
 * @param this the calling buffer
 * @param bitOffset the offset for from the currenty buffer position
 * @param noOfBits the number of bits to be read
 *
 * @return unsigned 8 bit value representing the specified bit range
 */
uint8_t Buffy_getNBits8(Buffy_t *this, int bitOffset,
    const int noOfBits);

/**
 * @brief Get 1 - 16 bits returned in a uint16.
 *
 * @param this the calling buffer
 * @param bitOffset the offset for from the currenty buffer position
 * @param the number of bits to be read
 *
 * @return unsigned 16 bit value representing the specified bit range
 */
uint16_t Buffy_getNBits16(Buffy_t *this, int bitOffset,
    const int noOfBits, const enum Encoding encoding);

/**
 * @brief Get 1 - 32 bits returned in a uint32.
 *
 * @param this the calling buffer
 * @param bitOffset the offset for from the currenty buffer position
 * @param the number of bits to be read
 *
 * @return unsigned 32 bit value representing the specified bit range
 */
uint32_t Buffy_getNBits32(Buffy_t *this, int bitOffset,
    const int noOfBits, const enum Encoding encoding);

/**
 * @brief Get 1 - 64 bits returned in a uint64.
 *
 * @param this the calling buffer
 * @param bitOffset the offset for from the currenty buffer position
 * @param the number of bits to be read
 *
 * @return unsigned 64 bit value representing the specified bit range
 */
uint64_t Buffy_getNBits64(Buffy_t *this, int bitOffset,
    const int noOfBits, const enum Encoding encoding);

    /**
	* @brief returns a string representation of the whole package data
	*
	* @return string represenation of buffererd data
	*/
    char *Buffy_dump(Buffy_t *this);

#endif
