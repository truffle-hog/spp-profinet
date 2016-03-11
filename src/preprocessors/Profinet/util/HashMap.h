#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#include "dissect/DissectionUtils.h"

struct Entry {
    char *key;
    struct Value value;
    uint8_t valid;
};

struct HashMap {

    size_t size;
    size_t allocated;
    struct Entry *table;
};

struct HashMap *HashMap_new(size_t initial);

int HashMap_insert(struct HashMap *this, char *key, struct Value value, struct Value *existing);

struct Value *HashMap_remove(struct HashMap *this, char *key);

struct Value *HashMap_find(struct HashMap *this, char *key);

void *HashMap_forEach(struct HashMap *this, int (*doThis)(struct Entry entry, void *args, void *ret));

void HashMap_free(struct HashMap *HashMap);

int HashMap_clear(struct HashMap *this);

#endif
