/**
 * @file Dissector.h
 *
 * @brief The Basic Dissector abstraction (Interface).
 *
 * The Base Dissector abstraction. Every implementation of a Dissector will use
 * and implement the operations described in this interface.
 * Dissector are used to dissect certain ranges of data in a network package,
 * while having the possibility to link to further dissectors when the dissection
 * of the desired range is complete.
 *
 * -> It is possible to link several Dissectors together building a tree of dissectors
 * and subdissectors that call each other when their dissection part is completed.
 */

#ifndef __PROTOCOL_TYPES_H__
#define __PROTOCOL_TYPES_H__

enum ProtocolType { ETHERNET = 0 };

#endif
