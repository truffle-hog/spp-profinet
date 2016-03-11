#include <stdlib.h>
#include <stdint.h>

#include "dbg.h"

#include "util/DArray_Entry.h"
#include "util/HashMap.h"

static int
_forEachRehash(struct Entry entry, void *args, void *ret) {

	(void) ret;

    check(args != NULL, "arguments must not be null");

    struct HashMap *map = (struct HashMap *) args;

    HashMap_insert(map, entry.key, entry.value, NULL);

	return 1;

error:

	return -1;

}

static void
*_reallocate(struct HashMap *this, size_t newSize) {

    this->allocated = newSize;

    free(this->table);
    this->table = malloc(this->allocated * sizeof(struct Entry));
    check_mem(this->table);
	memset(this->table, 0, sizeof(struct Entry) * this->allocated);

    DArray_Entry_forEach(this->entries, _forEachRehash, this, NULL);

    return this->table;

error:
    return NULL;
}


struct HashMap
*HashMap_new(size_t initial) {

    struct HashMap *hashMap = malloc(sizeof(struct HashMap));
    check_mem(hashMap);

    hashMap->size = 0;
    hashMap->allocated = initial;

    hashMap->entries = DArray_Entry_new(1);
    check_mem(hashMap->entries);

    hashMap->table = malloc(sizeof(struct Entry) * hashMap->allocated);
	check_mem(hashMap->table);
	memset(hashMap->table, 0, sizeof(struct Entry) * hashMap->allocated);

	return hashMap;

error:
    return NULL;

}

static unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {

        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

static int
_hashMe(size_t allocated, char *key) {

    unsigned long myHash = hash((unsigned char*) key);

    return myHash % allocated;
}

int
HashMap_insert(struct HashMap *this, char *key, struct Value value, struct Value *existing) {

    if (this->size == this->allocated) {

        check_mem(_reallocate(this, 2 * this->size));
    }

    int i;
    for (i = _hashMe(this->allocated, key); (this->table + i)->valid; i++) {

		debug("index: %d", i);
		debug("valid: %d", this->table[i].valid);

		//debug("entry %d, %s", i, this->table[i]);

        if (!strcmp(this->table[i].key, key)) {

            if (existing != NULL) {

				memcpy(existing, &this->table[i].value, sizeof(struct Value));
            }
            this->table[i] = (struct Entry) {.key = key, .value = value};
            return 1;
        }
    }
    this->table[i] = (struct Entry) {.key = key, .value = value, .valid = 1};
    return 0;
error:
    return -1;
}

struct Value *HashMap_find(struct HashMap *this, char *key) {

    int i;
    for (i = _hashMe(this->allocated, key); !(this->table + i)->valid; i++) {
        if (!strcmp(this->table[i].key, key)) {
            return &(this->table + i)->value;
        }
    }
    return NULL;
}

struct Value *HashMap_remove(struct HashMap *this, char *key) {

	(void) this;
	(void) key;

	sentinel("Not implemented exception");



error:
    return NULL;
}



void *HashMap_forEach(struct HashMap *this, int (*doThis)(struct Entry entry, void *args, void *ret)) {
	(void) this;
	(void) doThis;

	sentinel("Not implemented exception");

error:
    return NULL;

}

void HashMap_free(struct HashMap *hashMap) {

    hashMap->size = 0;
    hashMap->allocated = 10;
    free(hashMap->table);
    free(hashMap->entries);
    free(hashMap);

    hashMap = NULL;
}


int HashMap_clear(struct HashMap *this) {

	(void) this;

    sentinel("Not implemented exception");

	return 0;

error:
    return -1;

}
