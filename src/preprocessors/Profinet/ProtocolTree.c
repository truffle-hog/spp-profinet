/**
 * @file
 * @brief // TODO corrent description
 */

#include "plugbase.h"

#include <stdint.h>
#include <stdlib.h>

#include "dbg.h"

#include "ProtocolTree.h"
#include "ProtocolTree-int.h"

/**
 * @see ProtocolTree_new
 */
ProtocolTree_t *ProtocolTree_new() {

	ProtocolTree_t *tree = malloc(sizeof(ProtocolTree_t));
	check_mem(tree);

	tree->hInfo = malloc(sizeof(struct HeaderInfo));

	return tree;

error:
	return NULL;
}


/**
 * @see ProtocolTree_free
 */
void ProtocolTree_free(ProtocolTree_t *proto);

/**
 * @see ProtocolTree_branch
 */
ProtocolTree_t *ProtocolTree_branch(ProtocolTree_t *this, struct HeaderInfo *info);

/**
 * @see ProtocolTree_findBranch
 */
ProtocolTree_t *ProtocolTree_findBranch(ProtocolTree_t *this, char *caption);

