/**
 * @file
 * @brief The internal structure of Sender.
 */

#ifndef __SENDER_INT_H__
#define __SENDER_INT_H__

#include <stdbool.h>

#include "Sender.h"

#include "Truffle.h"

struct Sender;

/**
 * @brief The operations that can be called by a Sender.
 *
 */
struct Sender_ops {

  /**
   * @brief Frees the given sender.
   *
   * @param sender the sender to be freed
   *
   * @return 0 if the freeing was successful,
   *         -1 otherwise
   */
  int (*Sender_free)(Sender_t *sender);

  /**
   * Sends the given truffle to the specified ipc
   *
   * @param this the calling sender
   * @param truffle the truffle to be send
   *
   * @return 0 if the sending was successful, -1 if no client is detected for receiving, or on other errors.
   */
  int (*Sender_send)(Sender_t *this, Truffle_t *truffle);

};

/**
 * @brief Sender for sending Truffles to a specified port/socket/mq/sma.
 */
struct Sender {

  /** Whether this sender was initialized. **/
  bool initialized;

  /** The sender operations. **/
  const struct Sender_ops *ops;
};


/**
 * Creates a new Dissector with the given operations. This Function is the
 * interface constructor for every Dissector implementation.
 *
 * @param ops the pointer to the operations used for this dissector
 * @return a pointer to the created dissector
 */

Sender_t * Sender_new(const struct Sender_ops *ops);

#endif
