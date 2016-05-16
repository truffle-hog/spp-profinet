#include <stdlib.h>
#include <stdint.h>

#include "dbg.h"

#include "util/ScopeAlloc.h"

#include "util/DArray.h"

struct DArray *dArrayAppScope;

struct DArray *dArrayPacketScope;

struct DArray *dArrayClientScope;

static void *
_scopeMalloc(struct DArray *scope, size_t size) {

    void *newMem = malloc(size);
    check_mem(newMem);

    DArray_pushBack(scope, newMem);

    return newMem;
error:
    return NULL;
}

void *appScopeMalloc(size_t size) {

    if (!dArrayAppScope) {
        dArrayAppScope = DArray_new(1);
        check_mem(dArrayAppScope);
    }

    return _scopeMalloc(dArrayAppScope, size);

error:
    return NULL;

}

void *packetScopeMalloc(size_t size) {

    if (!dArrayPacketScope) {
        dArrayPacketScope = DArray_new(1);
        check_mem(dArrayPacketScope);
    }

    return _scopeMalloc(dArrayPacketScope, size);

error:
    return NULL;

}

void *clientScopeMalloc(size_t size) {

    if (!dArrayClientScope) {
        dArrayClientScope = DArray_new(1);
        check_mem(dArrayClientScope);
    }

    return _scopeMalloc(dArrayClientScope, size);

error:
    return NULL;

}

int appScopeFree() {

    check(dArrayAppScope, "no app scope yet initialized");

    check(DArray_clear(dArrayAppScope) != -1, "clear failed");

    return 0;

error:
    return -1;
}

int packetScopeFree() {

    check(dArrayPacketScope, "no packet scope yet initialized");

    check(DArray_clear(dArrayPacketScope) != -1, "clear failed");

    return 0;

error:
    return -1;
}

int clientScopeFree() {

    check(dArrayClientScope, "no client scope yet initialized");

    check(DArray_clear(dArrayClientScope) != -1, "clear failed");

    return 0;

error:
    return -1;
}

void cleanUpScopes() {

	DArray_free(dArrayAppScope);
	DArray_free(dArrayClientScope);
	DArray_free(dArrayPacketScope);

}

