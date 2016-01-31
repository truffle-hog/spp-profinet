/**
 * @file
 * @brief The internal functionality of Sender.
 */

#ifndef __SENDER_INT_H__
#define __SENDER_INT_H__

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

Sender_t * Sender_new(const struct sender_ops *ops);

#endif
