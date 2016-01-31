/**
 * @file
 * @brief The interface for ProtocolTree.
 */

#ifndef __PROTOCOL_TREE_H__
#define __PROTOCOL_TREE_H__

/**
 * @brief Creates a new buffer from the given snort package.
 *
 * @param p the packet as defined by snort
 * @return the instantiated Buffer
 */
ProtocolTree_t *ProtocolTree_new(Packet *p);

/**
 * @brief Frees the given buffer from memory.
 *
 * @param buffy the buffer to be freed
 */
void ProtocolTree_free(ProtocolTree_t *buffy);

/**
 * @brief Get 1 - 8 bits returned in a uint8.
 *
 * @param this the calling buffer
 * @param bit_offset the offset for from the currenty buffer position
 * @param the number of bits to be read
 *
 * @return unsigned 8 bit value representing the specified bit range
 */
uint8_t ProtocolTree_get_bits8(ProtocolTree_t *this, unsigned int bit_offset,
    const int no_of_bits);

/**
 * @brief Get 1 - 16 bits returned in a uint16.
 *
 * @param this the calling buffer
 * @param bit_offset the offset for from the currenty buffer position
 * @param the number of bits to be read
 *
 * @return unsigned 16 bit value representing the specified bit range
 */
uint16_t ProtocolTree_get_bits16(ProtocolTree_t *this, unsigned int bit_offset,
    const int no_of_bits, const unsigned int encoding);

/**
 * @brief Get 1 - 32 bits returned in a uint32.
 *
 * @param this the calling buffer
 * @param bit_offset the offset for from the currenty buffer position
 * @param the number of bits to be read
 *
 * @return unsigned 32 bit value representing the specified bit range
 */
uint32_t ProtocolTree_get_bits32(ProtocolTree_t *this, unsigned int bit_offset,
    const int no_of_bits, const unsigned int encoding);

/**
 * @brief Get 1 - 64 bits returned in a uint64.
 *
 * @param this the calling buffer
 * @param bit_offset the offset for from the currenty buffer position
 * @param the number of bits to be read
 *
 * @return unsigned 64 bit value representing the specified bit range
 */
uint64_t ProtocolTree_get_bits64(ProtocolTree_t *this, unsigned int bit_offset,
    const int no_of_bits, const unsigned int encoding);

#endif
