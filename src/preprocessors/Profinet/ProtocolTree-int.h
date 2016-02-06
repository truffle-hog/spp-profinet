/**
 * @file
 * @brief The internal sturcture of ProtocolTree.
 */

#ifndef __PROTOCOL_TREE_INT_H__
#define __PROTOCOL_TREE_INT_H__

#include <stdbool.h>

#include "ProtocolTree.h"

struct TreeData;

struct ProtocolNode;

struct FieldInfo;

/**
 * @brief The operations that can be called by a ProtocolTree.
 *
 */
struct ProtocolTree_ops {


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
 * @brief Tree structure for building protocol information.
 *
 */
struct ProtocolTest {

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

enum FieldType {

	STRING = 0, INT = 1, REAL = 2, BOOL = 3, RAW = 4

};

struct FieldInfo {

	char *name;
	void *data;

	/** The length of this data in Bits **/
	int length;
	enum FieldType type;
};


/** every protocol tree has one of these data. each node in the tree points to the same one **/
struct TreeData {
//	GHashTable  *interesting_hfids;
//	boolean     visible;
//	boolean     fake_protocols;

	/** The number of Nodes in this ProtocolTree. **/
	int         count;
//	struct _packet_info *pinfo;
};

/** Each proto_tree, proto_item is one of these. */
struct ProtocolNode {

	/** The number of sibblings on this ProtocolTree level. **/
	int sibblingCount;

	/** The number of children from this ProtocolNode. **/
	int childCount;

	/** The unique key identifying this ProtocolNode. **/
	char *key;

	/** The children of this ProtocolNode. **/
	struct ProtocolNode **children;

	/** The next sister Node of this Node. **/
	struct ProtocolNode *next;

	/** The previous sister Node of this Node. **/
	struct ProtocolNode *prev;

	/** The parent of this ProtocolNode. **/
	struct ProtocolNode *parent;

	/** The field info this ProtocolNode has. **/
	struct FieldInfo  *fieldInfo;

	/** The tree data common for every Node in this nodes tree. **/
	struct TreeData *treeData;

};


/**
 * @brief Creates a new ProtocolTree.
 *
 * @return the instantiated Tree
 */
ProtocolTree_t *ProtocolTree_new();


#endif
