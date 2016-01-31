/**
 * @file
 * @brief The interface for Buffy.
 */

#ifndef __BUFFY_INT_H__
#define __BUFFY_INT_H__

struct Buffy;

/**
 * @brief The operations that can be called by a Buffy buffer.
 *
 */
struct Buffy_ops {

  /**
   * @brief Frees the given buffer from memory.
   *
   * @param buffy the buffer to be freed
   */
  void (*Buffy_free)(Buffy_t *buffy);

  /**
   * @brief Get 1 - 8 bits returned in a uint8.
   *
   * @param this the calling buffer
   * @param bit_offset the offset for from the currenty buffer position
   * @param the number of bits to be read
   *
   * @return unsigned 8 bit value representing the specified bit range
   */
  uint8_t (*Buffy_get_bits8)(Buffy_t \*this, unsigned int bit_offset,
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
  uint16_t (*Buffy_get_bits16)(Buffy_t \*this, unsigned int bit_offset,
      const int no_of_bits, const unsigned int encoding);

  /**
   * @brief Get 1 - 32 bits returned in a uint32.
   *
   * @param this the calling buffer
   * @param bit_offset the offset for from the currenty buffer position
   * @param the number of bits to be read
   Gu
   * @return unsigned 32 bit value representing the specified bit range
   */
  uint32_t (*Buffy_get_bits32)(Buffy_t \*this, unsigned int bit_offset,
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
  uint64_t (*Buffy_get_bits64)(Buffy_t \*this, unsigned int bit_offset,
      const int no_of_bits, const unsigned int encoding);

};

/**
 * @brief Buffer for dissecting packages in the profinet plugin.
 *
 */
struct Buffy {

  /** Whether this buffer was initialized. **/
  bool initialized;

  /** Pointer to the snort package this buffer was created from **/
  Packet *p;

  /** The buffer operations. **/
  const struct Buffy_ops *ops;
};

Buffy_t *Buffy_new(Packet *p);

#endif
