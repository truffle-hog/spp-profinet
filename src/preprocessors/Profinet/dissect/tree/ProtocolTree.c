/**
 * @file
 * @brief // TODO corrent description
 */



#include <stdint.h>
#include <stdlib.h>

#include "dbg.h"

#include "ProtocolTree.h"
#include "ProtocolTree-int.h"

/**
 * @see ProtocolTree ops
 */
static const struct ProtocolTree_ops ProtocolTreeOverride_ops = {

	ProtocolTree_free,
	ProtocolTree_branch,
	ProtocolTree_setValue,
	ProtocolTree_getValue,
	ProtocolTree_getRoot,
	ProtocolTree_getParent,
	ProtocolTree_getChild,
	ProtocolTree_getNode
};

/**
 * @see ProtocolTree_free
 */
void ProtocolTree_free(struct ProtocolNode *proto) {

	if (proto->parent == NULL) {
		free(proto->treeData);
	}

	int i = 0;
	for (i = 0; i < proto->childCount; i++) {

		ProtocolTree_free(proto->children[i]);
	}

	// TODO maybe have to free key for avoiding memory leak ?
	free(proto);
}

/**
 * @see ProtocolTree_new
 */
struct ProtocolNode *ProtocolTree_new() {

	struct ProtocolNode *node = malloc(sizeof(struct ProtocolNode));
	check_mem(node);

	struct TreeData *treeData = malloc(sizeof(struct TreeData));
	check_mem(treeData);

	treeData->size = 0;
	treeData->root = node;
	treeData->keys = NULL;
	treeData->mappedNodePointers = NULL;

	node->ops = &ProtocolTreeOverride_ops;
	node->childCount = 0;
	node->treeData = treeData;
	node->parent = NULL;
	node->children = NULL;

	return node;

error:

	return NULL;
}

/**
 * @see ProtocolNode_branch
 */
struct ProtocolNode *ProtocolTree_branch(struct ProtocolNode *this, char *key, struct Value value) {

	struct ProtocolNode *child = malloc(sizeof(struct ProtocolNode));
	check_mem(child);

	child->treeData = this->treeData;

	child->key = key;
	child->value = value;
	child->ops = this->ops;
	child->childCount = 0;
	child->parent = this;
	child->children = NULL;

	// TODO implement dynamic growing
	this->children = realloc(this->children, (this->childCount + 1) * sizeof(void*));
	check_mem(this->children);

	this->treeData->keys = realloc(this->treeData->keys, (this->treeData->size + 1) * sizeof(void*));
	check_mem(this->treeData->keys);

	this->treeData->mappedNodePointers = realloc(this->treeData->mappedNodePointers, (this->treeData->size + 1) * sizeof(void*));
	check_mem(this->treeData->mappedNodePointers);

	this->treeData->mappedNodePointers[this->treeData->size] = child;
	this->treeData->keys[this->treeData->size] = child->key;

	this->children[this->childCount] = child;
	this->childCount++;
	this->treeData->size++;

	return child;

error:
	return NULL;
}

/**
 * @see ProtocolTree_setValue
 */
struct Value ProtocolTree_setValue(struct ProtocolNode *this, struct Value value) {

	this->value = value;
	// TODO maybe check and return old value
	return value;
}

/**
 * @see ProtocolTree_setValue
 */
struct Value ProtocolTree_getValue(const struct ProtocolNode *this) {

	return this->value;
}



/**
 * @see ProtocolTree_getRoot
 */
struct ProtocolNode *ProtocolTree_getRoot(const struct ProtocolNode *this) {

	return this->treeData->root;

}

/**
 * @see ProtocolTree_getParent
 */
struct ProtocolNode *ProtocolTree_getParent(const struct ProtocolNode *this) {

	return this->parent;
}

/**
 * @see ProtocolTree_getChild
 */
struct ProtocolNode *ProtocolTree_getChild(const struct ProtocolNode *this, int index) {

	return this->children[index];
}

/**
 * @see ProtocolTree_getNode
 */
struct ProtocolNode *ProtocolTree_getNode(const struct ProtocolNode *this, char *key) {

	int i = 0;
	// TODO change linear search to real hasing ;)
	for (i = 0; i < this->treeData->size; i++) {

//		printf("%s\n", this->treeData->keys[i]);

		if (!strcmp(this->treeData->keys[i], key)) {
			return this->treeData->mappedNodePointers[i];
		}

	}
	return NULL;
}
