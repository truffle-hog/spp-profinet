#ifndef __DARRAY_ENTRY_H__
#define __DARRAY_ENTRY_H__

#include <stdlib.h>

struct DArray_Entry {

    size_t allocated;
    size_t size;
    struct Entry *values;
};

struct DArray_Entry *DArray_Entry_new(size_t initial);

int DArray_Entry_pushBack(struct DArray_Entry *this, struct Entry element);

int DArray_Entry_popBack(struct DArray_Entry *this, struct Entry *popped);

struct Entry *DArray_Entry_get(struct DArray_Entry *this, unsigned long index);

int DArray_Entry_forEach(struct DArray_Entry *this, int (*doThis)(struct Entry entry, void *args, void *ret), void *args, void *ret);

void DArray_Entry_free(struct DArray_Entry *dArray);

int DArray_Entry_clear(struct DArray_Entry *this);

#endif
