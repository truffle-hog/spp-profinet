#include <stdlib.h>
#include <stdint.h>

#include "dbg.h"

#include "util/DArray.h"

static void
*_reallocate(struct DArray *this, size_t newSize) {

    this->allocated = newSize;
    this->ptrArray = realloc(this->ptrArray, this->allocated * sizeof(void *));
    check_mem (this->ptrArray);

    return this->ptrArray;

error:
    return NULL;
}

struct DArray *DArray_new(size_t initial) {

    struct DArray *dArray = malloc(sizeof(struct DArray));
    check_mem(dArray);

    dArray->allocated = initial;
    dArray->size = 0;
    dArray->ptrArray = malloc(initial * sizeof(void*));

    return dArray;

error:
    return NULL;

}

int DArray_clear(struct DArray *this) {

    this->size = 0;
    this->allocated = 1;
    this->ptrArray = realloc(this->ptrArray, this->allocated * sizeof(void*));
    check_mem(this->ptrArray);

    return 0;

error:
    return -1;
}

size_t DArray_size(struct DArray *this) {
    return this->size;
}

int DArray_pushBack(struct DArray *this, void *elementPtr) {

    if (this->size == this->allocated) {

        check_mem(_reallocate(this, 2 * this->size));
    }
    this->ptrArray[this->size] = elementPtr;
    this->size++;
    return 0;
error:
    return -1;
}

void *DArray_popBack(struct DArray *this) {
    check(this->size > 0, "there is no element left");

    this->size--;

    if ( (4 * this->size) <= this->allocated && this->size > 0) {

        _reallocate(this, 2 * this->size);
    }

error:
    return NULL;
}

void *DArray_get(struct DArray *this, unsigned long index) {

    check (0 <= index && index < this->size, "out of bounds: %ld, allowed: [0,%ld]",
            index, this->size - 1);

    return this->ptrArray[index];

error:
    return NULL;
}

int DArray_forEach(struct DArray *this, void (doThis)(void *elementPtr)) {

	(void) this;
	(void) doThis;

    sentinel("Not implemented Exception");

    return 0;

error:
    return -1;

}

void DArray_free(struct DArray *dArray) {

    free(dArray->ptrArray);
    free(dArray);
}
