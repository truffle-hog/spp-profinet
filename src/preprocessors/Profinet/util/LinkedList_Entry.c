#include <stdlib.h>
#include <stdint.h>

#include "dbg.h"

#include "util/DArray_Entry.h"
#include "util/HashMap.h"

static void
*_reallocate(struct DArray_Entry *this, size_t newSize) {

    this->allocated = newSize;
    this->values = realloc(this->ptrArray, this->allocated * sizeof(struct Entry));
    check_mem (this->values);

    return this->values;

error:
    return NULL;
}

struct DArray_Entry *DArray_Entry_new(size_t initial) {

    struct DArray_Entry *DArray_Entry = malloc(sizeof(struct DArray_Entry));
    check_mem(DArray_Entry);

    DArray_Entry->allocated = initial;
    DArray_Entry->size = 0;
    DArray_Entry->values = malloc(initial * sizeof(struct Entry));
    check_mem(this->values);

    return DArray_Entry;

error:
    return NULL;

}

int DArray_Entry_clear(struct DArray_Entry *this) {

    this->size = 0;
    this->allocated = 1;
    this->values = realloc(this->ptrArray, this->allocated * sizeof(struct Entry));
    check_mem(this->values);

    return 0;

error:
    return -1;
}

size_t DArray_Entry_size(struct DArray_Entry *this) {
    return this->size;
}

int DArray_Entry_pushBack(struct DArray_Entry *this, struct Entry element) {

    if (this->size == this->allocated) {

        check_mem(_reallocate(this, 2 * this->size));
    }
    this->values[this->size] = element;
    this->size++;
    return 0;
error:
    return -1;
}

struct Entry DArray_Entry_popBack(struct DArray_Entry *this) {
    check(this->size > 0, "there is no element left");

    this->size--;

    if ( (4 * this->size) <= this->allocated && this->size > 0) {

        _reallocate(this, 2 * this->size);
    }

error:
    return NULL;
}

struct Entry DArray_Entry_get(struct DArray_Entry *this, int index) {

    check (0 <= index && index < this->size, "out of bounds: %d, allowed: [0,%d]",
            index, this->size - 1);

    return this->values[index];

error:
    return NULL;
}

int DArray_Entry_forEach(struct DArray_Entry *this, int (*doThis)(struct Entry entry, void *args, void *ret),
                        void *args, void *ret) {

    int i = 0;

    for (i = 0; i < this->size; i++) {
        int ret;
        check (ret = doThis(this->values[index], args, ret) != -1, "error on foreach function");

        if (ret == 0) {
            break;
        }
    }

    return 0;

error:
    return -1;

}

void DArray_Entry_free(struct DArray_Entry *DArray_Entry) {

    free(DArray_Entry->ptrArray);
    free(DArray_Entry);
}
