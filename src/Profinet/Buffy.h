/** @file Buffy.h
 *
 *  Snort Preprocessor Plugin Header
 *
 *  This file gets included in plugbase.h when it is integrated into the rest
 *  of the program.
 */

// uint8_t Buffy_get_uint8_t(Buffy_t *buffy, const int offset);
//
// uint16_t Buffy_get_ntohs(Buffy_t *buffy, const int offset);
// uint32_t Buffy_get_ntoh24(Buffy_t *buffy, const int offset);
// uint32_t Buffy_get_ntohl(Buffy_t *buffy, const int offset);
// uint64_t Buffy_get_ntoh40(Buffy_t *buffy, const int offset);
// int64_t Buffy_get_ntohi40(Buffy_t *buffy, const int offset);
// uint64_t Buffy_get_ntoh48(Buffy_t *buffy, const int offset);
// int64_t Buffy_get_ntohi48(Buffy_t *buffy, const int offset);
// uint64_t Buffy_get_ntoh56(Buffy_t *buffy, const int offset);
// int64_t Buffy_get_ntohi56(Buffy_t *buffy, const int offset);
// uint64_t Buffy_get_ntoh64(Buffy_t *buffy, const int offset);
// float Buffy_get_ntohieee_float(Buffy_t *buffy, const int offset);
// double Buffy_get_ntohieee_double(Buffy_t *buffy,
//     const int offset);
//
// uint16_t Buffy_get_letohs(Buffy_t *buffy, const int offset);
// uint32_t Buffy_get_letoh24(Buffy_t *buffy, const int offset);
// uint32_t Buffy_get_letohl(Buffy_t *buffy, const int offset);
// uint64_t Buffy_get_letoh40(Buffy_t *buffy, const int offset);
// int64_t Buffy_get_letohi40(Buffy_t *buffy, const int offset);
// uint64_t Buffy_get_letoh48(Buffy_t *buffy, const int offset);
// int64_t Buffy_get_letohi48(Buffy_t *buffy, const int offset);
// uint64_t Buffy_get_letoh56(Buffy_t *buffy, const int offset);
// int64_t Buffy_get_letohi56(Buffy_t *buffy, const int offset);
// uint64_t Buffy_get_letoh64(Buffy_t *buffy, const int offset);
// float Buffy_get_letohieee_float(Buffy_t *buffy, const int offset);
// double Buffy_get_letohieee_double(Buffy_t *buffy,
//     const int offset);
//
// uint16_t Buffy_get_uint16_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// uint32_t Buffy_get_uint24_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// uint32_t Buffy_get_uint32_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// uint64_t Buffy_get_uint40_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// int64_t Buffy_get_int40_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// uint64_t Buffy_get_uint48_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// int64_t Buffy_get_int48_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// uint64_t Buffy_get_uint56_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// int64_t Buffy_get_int56_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// uint64_t Buffy_get_uint64_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
// float Buffy_get_ieee_float(Buffy_t *buffy, const int offset, const unsigned int encoding);
// double Buffy_get_ieee_double(Buffy_t *buffy, const int offset, const unsigned int encoding);

/* Fetch a specified number of bits from bit offset in a tvb.  All of these
 * functions are equivalent, except for the type of the return value.  Note
 * that the parameter encoding (where supplied) is meaningless and ignored */

/** \headerfile Buffy.h "Buffy.h"
* Get 1 - 8 bits returned in a uint8.
*
* @param buffy the calling buffer
* @param bit_offset the offset for from the currenty buffer position
* @param the number of bits to be read
*
* \return unsigned 8 bit value representing the specified bit range
*/
uint8_t Buffy_get_bits8(Buffy_t *buffy, unsigned int bit_offset,
    const int no_of_bits);

/**
* Get 1 - 16 bits returned in a uint16.
*
* @param buffy the calling buffer
* @param bit_offset the offset for from the currenty buffer position
* @param the number of bits to be read
*
* \return unsigned 16 bit value representing the specified bit range
*/
uint16_t Buffy_get_bits16(Buffy_t *buffy, unsigned int bit_offset,
    const int no_of_bits, const unsigned int encoding);

/**
* Get 1 - 32 bits returned in a uint32.
*
* @param buffy the calling buffer
* @param bit_offset the offset for from the currenty buffer position
* @param the number of bits to be read
*
* \return unsigned 32 bit value representing the specified bit range
*/
uint32_t Buffy_get_bits32(Buffy_t *buffy, unsigned int bit_offset,
    const int no_of_bits, const unsigned int encoding);

/**
* Get 1 - 64 bits returned in a uint64.
*
* @param buffy the calling buffer
* @param bit_offset the offset for from the currenty buffer position
* @param the number of bits to be read
*
* \return unsigned 64 bit value representing the specified bit range
*/
uint64_t Buffy_get_bits64(Buffy_t *buffy, unsigned int bit_offset,
    const int no_of_bits, const unsigned int encoding);
