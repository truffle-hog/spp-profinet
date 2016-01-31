/**
 * @file
 * @brief This file houses the operations that are specific for a UnixSocketSender
 *
 * UnixSocketSender uses Unix sockets for sending a Truffle to a listening client.
 */

/**
 * @brief Sends Truffles to a unix socket a client is reading from.
 *
 */
struct UnixSocketSender {
   /** The encapsulated sender type for save casting. */
   struct Sender sender;
   // TODO implement specific fields / functionality
};

/**
 * @see Sender_ops
 */
static const struct Sender_ops UnixSocketSenderOverride_ops = {
  UnixSocketSender_free,
  UnixSocketSender_send
};

/**
 * @see Sender_new
 */
Sender_t *
UnixSocketSender_new() {

  Sender_t *dissector;

  dissector = Dissector_new(&UnixSocketSenderOverride_ops);

  return dissector;
}

/**
 * @see Sender_free
 */
int UnixSocketSender_free(Sender_t *sender) {

  //TODO implement
  return 0;
}

/**
 * @see Sender_send
 */
int UnixSocketSender_send(Sender_t *this, Truffle_t *truffle) {

  //TODO implement
  return 0;
}
