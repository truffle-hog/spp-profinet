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
   * @brief Creates a new ProtocolTree.
   *
   * @return the instantiated Tree
   */
  ProtocolTree_t *(*ProtocolTree_new)();

  /**
   * @brief Frees the given ProtocolTree from memory.
   *
   * @param proto the ProtocolTree to be freed
   */
  void (*ProtocolTree_free)(ProtocolTree_t *proto);

  /**
   * @brief Creates a new branch with the given info field from the current root pointer of this ProtocolTree.
   *
   * @param this the calling ProtocolTree
   * @param info the header info to be inserted for the new subtree
   *
   * @return A pointer to a Subtree with the newly created branch as its root pointer.
   */
  ProtocolTree_t *(*ProtocolTree_branch)(ProtocolTree_t *this, struct HeaderInfo *info);

  /**
   * @brief Searches and returns the branch with the given caption.
   *
   * @param this the calling ProtocolTree
   * @param the caption to be searched for
   *
   * @return the ProtocolTree starting at the found branch, NULL if there is no such
   *          branch.
   */
  ProtocolTree_t *(*ProtocolTree_findBranch)(ProtocolTree_t *this, char *caption);

};

/**
 * @brief Buffer for dissecting packages in the profinet plugin.
 *
 */
struct ProtocolTree {

  /** Whether this protocol Subtree was initialized. **/
  bool initialized;

  /** The Info field of this Subtree **/
  struct HeaderInfo *hInfo;

  /** Pointer to the parent subtree **/
  ProtocolTree_t *parent;

  /** Pointing to the branching protocol trees of this root node **/
  ProtocolTree_t **branches;

  /** The operations that can be called by a ProtocolTree**/
  const struct ProtocolTree_ops *ops;
};

ProtocolTree_t *ProtocolTree_new(Packet *p);

#endif
