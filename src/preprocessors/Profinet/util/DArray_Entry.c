#include <stdlib.h>
#include <stdint.h>

#include "dbg.h"

#include "util/DArray_Entry.h"
#include "util/HashMap.h"

static void
*_reallocate(struct DArray_Entry *this, size_t newSize) {

    this->allocated = newSize;
    this->values = realloc(this->values, this->allocated * sizeof(void*));
    check_mem (this->values);

    return this->values;

error:
    return NULL;
}

struct DArray_Entry *DArray_Entry_new(size_t initial) {

    struct DArray_Entry *dArrayEntry = malloc(sizeof(struct DArray_Entry));
    check_mem(dArrayEntry);

    dArrayEntry->allocated = initial;
    dArrayEntry->size = 0;
    dArrayEntry->values = malloc(initial * sizeof(void*));
    check_mem(dArrayEntry->values);

    return dArrayEntry;

error:
    return NULL;

}

int DArray_Entry_clear(struct DArray_Entry *this) {

    this->size = 0;
    this->allocated = 1;
    this->values = realloc(this->values, this->allocated * sizeof(void*));
    check_mem(this->values);

    return 0;

error:
    return -1;
}

size_t DArray_Entry_size(struct DArray_Entry *this) {
    return this->size;
}

int DArray_Entry_pushBack(struct DArray_Entry *this, struct Entry *element) {

    if (this->size == this->allocated) {

        check_mem(_reallocate(this, 2 * this->size));
    }
    this->values[this->size] = element;
    this->size++;
    return 0;
error:
    return -1;
}

int
DArray_Entry_popBack(struct DArray_Entry *this, struct Entry *popped) {
    check(this->size > 0, "there is no element left");

    this->size--;

    if ( (4 * this->size) <= this->allocated && this->size > 0) {

        _reallocate(this, 2 * this->size);
    }

//	popped* = this->values[this->size];


	if (popped) {
		memcpy(popped, this->values[this->size], sizeof(struct Entry));
	}

	return 0;

error:
    return -1;
}

struct Entry
*DArray_Entry_get(struct DArray_Entry *this, unsigned long index) {

    check (0 <= index && index < this->size, "out of bounds: %ld, allowed: [0,%ld]",
            index, this->size - 1);

    return this->values[index];

error:
    return NULL;
}

int DArray_Entry_forEach(struct DArray_Entry *this, int (*doThis)(struct Entry *entry, void *args, void *ret),
                        void *args, void *ret) {

    unsigned long i = 0;

    for (i = 0; i < this->size; i++) {

		int checkFunc = doThis(this->values[i], args, ret);
		check (checkFunc != -1, "error on foreach function");

        if (checkFunc == 0) {
            break;
        }
    }

    return 0;

error:
    return -1;

}

void DArray_Entry_free(struct DArray_Entry *DArray_Entry) {

    free(DArray_Entry->values);
    free(DArray_Entry);

	DArray_Entry = NULL;
}
