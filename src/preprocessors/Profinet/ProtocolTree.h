/**
 * @file
 * @brief The interface for ProtocolTree.
 */

#ifndef __PROTOCOL_TREE_H__
#define __PROTOCOL_TREE_H__

#include <stdint.h>



/** A protocol tree element. */
typedef struct ProtocolNode ProtocolTree_t;
/** A protocol item element. */
typedef struct ProtocolNode ProtocolItem_t;

/** @brief Info that can be inserte into a protocol tree as new branch. **/
struct HeaderInfo {

  /** @brief The caption of this info field. **/
  char caption[256];
  /** @brief Interesting bits that can be set. **/
  uint64_t bitmask;
  /** @brief Infofield, can contain any information in char format for specific size. **/
  char infofield[256];
  /** @brief A value that can be put for information. **/
  long long value;
  /** @brief Specifies the type of information. **/
  int type;

};


/**
 * @brief Frees the given ProtocolTree from memory.
 *
 * @param proto the ProtocolTree to be freed
 */
void ProtocolTree_free(ProtocolTree_t *proto);

/**
 * @brief Creates a new branch with the given info field from the current root pointer of this ProtocolTree.
 *
 * @param this the calling ProtocolTree
 * @param info the header info to be inserted for the new subtree
 *
 * @return A pointer to a Subtree with the newly created branch as its root pointer.
 */
ProtocolTree_t *ProtocolTree_branch(ProtocolTree_t *this, struct HeaderInfo *info);

/**
 * @brief Searches and returns the branch with the given caption.
 *
 * @param this the calling ProtocolTree
 * @param the caption to be searched for
 *
 * @return the ProtocolTree starting at the found branch, NULL if there is no such
 *          branch.
 */
ProtocolTree_t *ProtocolTree_findBranch(ProtocolTree_t *this, char *caption);


#endif
