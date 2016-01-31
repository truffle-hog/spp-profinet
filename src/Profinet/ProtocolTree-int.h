/**
 * @file
 * @brief The internal sturcture of ProtocolTree.
 */

#ifndef __PROTOCOL_TREE_INT_H__
#define __PROTOCOL_TREE_INT_H__

struct ProtocolTree;

/**
 * @brief The operations that can be called by a ProtocolTree.
 *
 */
struct ProtocolTree_ops {

  /**
   * @brief Frees the given buffer from memory.
   *
   * @param buffy the buffer to be freed
   */
  void (*ProtocolTree_free)(ProtocolTree_t *buffy);

};

/**
 * @brief Buffer for dissecting packages in the profinet plugin.
 *
 */
struct ProtocolTree {

  /** Whether this protocol tree was initialized. **/
  bool initialized;

  /** The operations that can be called by a ProtocolTree**/
  const struct ProtocolTree_ops *ops;
};

ProtocolTree_t *ProtocolTree_new(Packet *p);

#endif
