/**
 * @file
 * @brief // TODO corrent description
 */



#include <stdint.h>
#include <stdlib.h>

#include "dbg.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"

/**
 * @see ProtocolTree ops
 */
static const struct ProtocolTree_ops ProtocolTreeOverride_ops = {

	ProtocolTree_free,
	ProtocolTree_branch,
	ProtocolTree_setValue,
	ProtocolTree_getValue,
	ProtocolTree_insertImportantValue,
	ProtocolTree_getImportantValue,
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

// char *ProtocolItem_toString(struct ProtocolNode *node) {
//
// 	char *str = "";
// 	//"%s [label=\"{%s|%s]"
// 	int i = 0;
// 	for (i = 0; i < node->childCount; i++) {
// 		//strcat(str, "%s -> %s;\n", node->key, node->children[i]->key);
// 	}
// 	return str;
// }
//


/**
 * @see ProtocolTree_new
 */
struct ProtocolNode *ProtocolTree_new(char *rootKey) {

	struct ProtocolNode *node = malloc(sizeof(struct ProtocolNode));
	check_mem(node);

	struct TreeData *treeData = malloc(sizeof(struct TreeData));
	check_mem(treeData);

	treeData->size = 1;
	treeData->root = node;
	treeData->keys = malloc(sizeof(void*));
	treeData->mappedNodePointers = malloc(sizeof(void*));

	treeData->keys[0] = rootKey;
	treeData->mappedNodePointers[0] = node;

	treeData->map = HashMap_new(10);
	check_mem(treeData->map);

	node->key = rootKey;

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
struct ProtocolNode *ProtocolTree_branch(struct ProtocolNode *this, char *name, struct Value value) {

	struct ProtocolNode *child = malloc(sizeof(struct ProtocolNode));
	check_mem(child);

	child->treeData = this->treeData;

	child->name = name;
	child->value = value;
	child->ops = this->ops;
	child->childCount = 0;
	child->parent = this;
	child->children = NULL;
	child->id = child->treeData->nextID++;

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

int ProtocolTree_insertImportantValue(struct ProtocolNode *this, char *key, struct Value value) {

	return HashMap_insert(this->treeData->map, key, value, NULL);
}

struct Value *ProtocolTree_getImportantValue(struct ProtocolNode *this, char *key) {

	return HashMap_find(this->treeData->map, key);
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
