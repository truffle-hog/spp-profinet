/**
 * @file
 * @brief This file houses the operations that are specific for a UnixSocketSender
 *
 * UnixSocketSender uses Unix sockets for sending a Truffle to a listening client.
 */

struct PNRTDissector {
  struct Dissector dissector;

};

/**
 * @see Dissector_ops
 */
static const struct Dissector_ops PNRTDissectorOverride_ops = {

  sizeof(struct PNRTDissector), /* size */
  (uint64_t) 0x8892,
  (uint64_t) 0x8892,
  PNRTDissector_free,
  NULL,
  NULL,
  PNRTDissector_dissect
};

/**
 * @see Dissector_new
 */
Dissector_t *
PNRTDissector_new() {

  Dissector_t *dissector;

  dissector = Dissector_new(&PNRTDissectorOverride_ops);

  return dissector;
}

/**
 * @see Dissector_free
 */
void PNRTDissector_free(Dissector_t *dissector);

/**
 * @see Dissector_dissect
 */
int PNRTDissector_dissect(Dissector_t *this, Buffer_t *buf, ProtocolTree_t *tree);
