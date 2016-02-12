#ifndef __TRUFFLE_H__
#define __TRUFFLE_H__

/**
 * @file
 * @brief The structure of a Truffle that is send via ipc.
 */

// TODO define the flags that are used within the truffle

#include <stdint.h>

#include "ProtocolTree.h"
/**
 * @brief Houses specific information about the ether header.
 *
 */
struct EtherHeader {
  uint64_t sourceMacAddress;
  uint64_t destMacAddress;
  uint16_t etherType;
};

/**
 * @brief Houses specific information about the frame.
 */
struct Frame {
  uint16_t frameID;
  char destName[30];
  char srcName[30];
  long long cycleCounter;
};


/**
 * @brief The datastructure for sending relevant information to another process.
 *
 * The Truffle is the datastructure that encapsulates all necessary and important
 * information about a processed Network Packet. The structure of the Truffle is also
 * known by the clients that want to receive information about the network package.
 *
 * Like this clients are able to cast incoming data to this data type and imediately
 * read out the relevant data.
 *
 */
struct Truffle {

  /** @brief Flags are used for specific boolean states that are relevant for the whole package. **/
  uint64_t flags;
  /** @brief The Etherheader holds information from the etherheader of the network package. **/
  struct EtherHeader eh;
  /** @brief The Frame structure encapsulates information about the Frame within the network package. **/
  struct Frame frame;

};
typedef struct Truffle Truffle_t;

Truffle_t *Truffle_new(const struct ProtocolNode *protoTree);

#endif
