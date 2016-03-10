/**
 * @file
 * @brief The internal sturcture of ProtocolTree.
 */

#ifndef __PROTOCOL_TREE_INT_H__
#define __PROTOCOL_TREE_INT_H__

#include <stdbool.h>

#include "dissect/tree/ProtocolTree.h"

/**
 * @brief The operations that can be called by a ProtocolTree.
 *
 */
struct ProtocolTree_ops {

    /**
     * @brief Frees the given ProtocolNode and all of its ancestors in memory.
     *
     * @param proto the ProtocolNode to be freed
     */
    void (*ProtocolTree_free)(struct ProtocolNode *proto);

    // char *(*ProtocolItem_toString)(struct ProtocolNode *node);
    //
    // char *(*ProtocolTree_toString)(struct ProtocolNode *tree);

    /**
     * @brief Creates a new branch from the given ProtocolNode and returns a pointer to the newly created ProtocolNode.
     *
     * @param this the calling ProtocolNode
     *
     * @return A pointer to the newly created Node.
     */
    struct ProtocolNode *(*ProtocolTree_branch)(struct ProtocolNode *this, char *key, struct Value value);

    /**
     * @brief Sets the given field information for this protocol item.
     *
     * @param this the calling ProtocolNode
     * @param value the value to be inserted for this Node
     *
     * @return the old value of this Node if there is one, NULL otherwise.
     */
    struct Value (*ProtocolTree_setValue)(struct ProtocolNode *this, struct Value value);

    /**
     * @brief Returns the value of this Node.
     *
     * @param this the calling ProtocolNode
     *
     * @return the value of this Node
     */
    struct Value (*ProtocolTree_getValue)(const struct ProtocolNode *this);

    /**
     * @brief Returns this Tree's root Node that this ProtocolNode belongs to.
     * @param this the calling ProtocolNode
     *
     */
    struct ProtocolNode *(*ProtocolTree_getRoot)(const struct ProtocolNode *this);

    /**
     * @brief Returns this node's parent.
     * @return the parent of this node
     */
    struct ProtocolNode *(*ProtocolTree_getParent)(const struct ProtocolNode *this);

    /**
     * @brief Returns the child with the specified index of this ProtocolNode.
     *
     * @param this the calling ProtocolTree_getValue
     * @param index the index of the child to be returned
     *
     * @return the child at the specified index
     */
    struct ProtocolNode *(*ProtocolTree_getChild)(const struct ProtocolNode *this, int index);

    /**
     * @brief Retrieves the Node with the specified key in this node's tree.
     *
     * @param this the calling ProtocolNode
     * @param key the key to be searched for
     *
     * @return the ProtocolNode with the given key in this node's tree
     */
    struct ProtocolNode *(*ProtocolTree_getNode)(const struct ProtocolNode *this, char *key);

};

struct Value {

    /** The length of this value. **/
    int length;
    /** The different types this value can be. **/
    enum {  is_int8, is_uint8,
            is_int16, is_uint16,
            is_int32, is_uint32,
            is_int64, is_uint64,
            is_float, is_double,
            is_char, is_string
        } type;

    /** Union of all data types that can be inserted for a Value. **/
    union {
        int8_t int8;
        uint8_t uint8;
        int16_t int16;
        uint16_t uint16;
        int32_t int32;
        uint32_t uint32;
        int64_t int64;
        uint64_t uint64;
        float float32;
        double double64;
        char character;
        const char *string;
    } val;
};

struct TreeData {

    /** THE FAKE HASHMAP  ;) **/
    char **keys;
    struct ProtocolNode **mappedNodePointers;
    /** -------------------  **/

    /** The root Node of this ProtocolTree. **/
    struct ProtocolNode *root;

	/** The number of Nodes in this ProtocolTree. **/
	int         size;
};

/** Each proto_tree, proto_item is one of these. */
struct ProtocolNode {

	const struct ProtocolTree_ops *ops;

	/** The number of children from this ProtocolNode. **/
	int childCount;

	/** The unique key identifying this ProtocolNode. **/
	char *key;

    /** The value this ProtocolNode has. **/
    struct Value value;

	/** The children of this ProtocolNode. **/
	struct ProtocolNode **children;

	/** The next sister Node of this Node. **/
	struct ProtocolNode *next;

	/** The previous sister Node of this Node. **/
	struct ProtocolNode *prev;

	/** The parent of this ProtocolNode. **/
	struct ProtocolNode *parent;

	/** The treeData pointer common for every Node in this tree. **/
	struct TreeData *treeData;

};


/**
 * @brief Creates a new ProtocolTree.
 *
 * @return the instantiated Node
 */
struct ProtocolNode *ProtocolTree_new(char *rootKey);


#endif
