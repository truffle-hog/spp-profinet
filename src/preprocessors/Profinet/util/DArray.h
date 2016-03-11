#ifndef __D_ARRAY_H__
#define __D_ARRAY_H__

#include <stdlib.h>

struct DArray {

    size_t allocated;
    size_t size;
    void **ptrArray;
};

struct DArray *DArray_new(size_t initial);

int DArray_pushBack(struct DArray *this, void *elementPtr);

void *DArray_popBack(struct DArray *this);

void *DArray_get(struct DArray *this, unsigned long index);

int DArray_forEach(struct DArray *this, void (doThis)(void *elementPtr));

void DArray_free(struct DArray *dArray);

int DArray_clear(struct DArray *this);

#endif
