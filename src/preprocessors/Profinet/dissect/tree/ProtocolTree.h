/**
 * @file
 * @brief The interface for ProtocolTree.
 */

#ifndef __PROTOCOL_TREE_H__
#define __PROTOCOL_TREE_H__

#include <stdint.h>
#include "dissect/Dissector.h"
#include "dissect/Dissector-int.h"

/** The value to be inserted for a ProtocolNode. **/
struct Value;
/** A protocol tree element. */
typedef struct ProtocolNode ProtocolTree_t;
/** A protocol item element. */
typedef struct ProtocolNode ProtocolItem_t;

/**
 * @brief Frees the given ProtocolNode and all of its ancestors in memory.
 *
 * @param proto the ProtocolNode to be freed
 */
void ProtocolTree_free(struct ProtocolNode *proto);

// char *ProtocolItem_toString(struct ProtocolNode *node);
//
// char *ProtocolTree_toString(struct ProtocolNode *tree);

/**
 * @brief Creates a new branch from the given ProtocolNode and returns a pointer to the newly created ProtocolNode.
 *
 * @param this the calling ProtocolNode
 *
 * @return A pointer to the newly created Node.
 */
struct ProtocolNode *ProtocolTree_branch(struct ProtocolNode *this, char *name, struct Value value, struct Dissector *dissectedBy);

struct ProtocolNode *ProtocolTree_branchImportant(struct ProtocolNode *this, char *name, char *importantKey, struct Value value, struct Dissector *dissectedBy);

int ProtocolTree_insertImportantValue(struct ProtocolNode *this, char *key, struct Value value);

struct Value *ProtocolTree_getImportantValue(struct ProtocolNode *this, char *key);

/**
 * @brief Sets the given field information for this protocol item.
 *
 * @param this the calling ProtocolNode
 * @param value the value to be inserted for this Node
 *
 * @return the old value of this Node if there is one, NULL otherwise.
 */
struct Value ProtocolTree_setValue(struct ProtocolNode *this, struct Value value);

/**
 * @brief Returns the value of this Node.
 *
 * @param this the calling ProtocolNode
 *
 * @return the value of this Node
 */
struct Value ProtocolTree_getValue(const struct ProtocolNode *this);

/**
 * @brief Returns this Tree's root Node that this ProtocolNode belongs to.
 * @param this the calling ProtocolNode
 *
 */
struct ProtocolNode *ProtocolTree_getRoot(const struct ProtocolNode *this);

/**
 * @brief Returns this node's parent.
 * @return the parent of this node
 */
struct ProtocolNode *ProtocolTree_getParent(const struct ProtocolNode *this);

/**
 * @brief Returns the child with the specified index of this ProtocolNode.
 *
 * @param this the calling ProtocolTree_getValue
 * @param index the index of the child to be returned
 *
 * @return the child at the specified index
 */
struct ProtocolNode *ProtocolTree_getChild(const struct ProtocolNode *this, int index);

/**
 * @brief Retrieves the Node with the specified key in this node's tree.
 *
 * @param this the calling ProtocolNode
 * @param key the key to be searched for
 *
 * @return the ProtocolNode with the given key in this node's tree
 */
struct ProtocolNode *ProtocolTree_getNode(const struct ProtocolNode *this, char *key);


#endif
