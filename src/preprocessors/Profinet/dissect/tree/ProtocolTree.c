/**
 * @file
 * @brief // TODO corrent description
 */



#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dbg.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"

/**
 * @see ProtocolTree ops
 */
static const struct ProtocolTree_ops ProtocolTreeOverride_ops = {

	ProtocolTree_free,
	ProtocolTree_branch,
	ProtocolTree_branchImportant,
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
struct ProtocolNode *ProtocolTree_new(char *rootName, Dissector_t *dissectedBy) {

	struct ProtocolNode *node = malloc(sizeof(struct ProtocolNode));
	check_mem(node);

	struct TreeData *treeData = malloc(sizeof(struct TreeData));
	check_mem(treeData);

	treeData->size = 1;
	treeData->root = node;
	treeData->nodePointers = malloc(sizeof(void*));
	treeData->nextID = 0;


	treeData->nodePointers[0] = node;

	treeData->map = HashMap_new(1000);
	check_mem(treeData->map);

	node->isImportant = false;
	node->importantKey = "";

	node->name = rootName;
	node->id = treeData->nextID++;
	node->ops = &ProtocolTreeOverride_ops;
	node->childCount = 0;
	node->treeData = treeData;
	node->parent = NULL;
	node->children = NULL;
	node->visited = false;

	node->dissectedBy = dissectedBy;

	return node;

error:

	return NULL;
}

/**
 * @see ProtocolNode_branch
 */
struct ProtocolNode *ProtocolTree_branch(struct ProtocolNode *this, char *name, struct Value value, Dissector_t *dissectedBy) {

	struct ProtocolNode *child = malloc(sizeof(struct ProtocolNode));
	check_mem(child);

	child->treeData = this->treeData;

	child->dissectedBy = dissectedBy;

	child->name = name;
	child->value = value;
	child->isImportant = false;
	child->importantKey = "";
	child->ops = this->ops;
	child->childCount = 0;
	child->parent = this;
	child->children = NULL;
	child->id = child->treeData->nextID++;
	child->visited = false;

	// TODO implement dynamic growing
	this->children = realloc(this->children, (this->childCount + 1) * sizeof(void*));
	check_mem(this->children);

	this->treeData->nodePointers = realloc(this->treeData->nodePointers, (this->treeData->size + 1) * sizeof(void*));
	check_mem(this->treeData->nodePointers);

	this->treeData->nodePointers[this->treeData->size] = child;

	this->children[this->childCount] = child;
	this->childCount++;
	this->treeData->size++;

	return child;

error:
	return NULL;
}

struct ProtocolNode *ProtocolTree_branchImportant(struct ProtocolNode *this, char *name, char *importantKey, struct Value value, Dissector_t *dissectedBy) {

	struct ProtocolNode *child = malloc(sizeof(struct ProtocolNode));
	check_mem(child);

	child->treeData = this->treeData;

	child->dissectedBy = dissectedBy;

	child->name = name;
	child->value = value;
	child->isImportant = true;
	child->importantKey = importantKey;
	child->ops = this->ops;
	child->childCount = 0;
	child->parent = this;
	child->children = NULL;
	child->id = child->treeData->nextID++;
	child->visited = false;

	ProtocolTree_insertImportantValue(this, importantKey, value);

	// TODO implement dynamic growing
	this->children = realloc(this->children, (this->childCount + 1) * sizeof(void*));
	check_mem(this->children);

	this->treeData->nodePointers = realloc(this->treeData->nodePointers, (this->treeData->size + 1) * sizeof(void*));
	check_mem(this->treeData->nodePointers);

	this->treeData->nodePointers[this->treeData->size] = child;

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
struct ProtocolNode *ProtocolTree_getNode(const struct ProtocolNode *this, char *name) {

	sentinel("not implemented exception");

error:
	return NULL;
}

int
ProtocolTree_forEach(struct ProtocolNode *this,
	int (*doThis)(struct ProtocolNode node, void *args, void *ret),
	void *args, void *ret) {




}
