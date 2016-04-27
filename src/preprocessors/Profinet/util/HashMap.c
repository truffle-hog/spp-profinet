#include <stdlib.h>
#include <stdint.h>

#include "dbg.h"

#include "util/HashMap.h"
#include "dissect/DissectionUtils.h"

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

	debug("reallocation frome size=%ld to size=%ld", this->allocated, newSize);

	size_t allocated = this->allocated;

	struct Entry *copiedTable = malloc(sizeof(struct Entry) * this->allocated);
	check_mem(copiedTable);

	memcpy(copiedTable, this->table, this->allocated * sizeof(struct Entry));
    this->allocated = newSize;

    free(this->table);
    this->table = malloc(this->allocated * sizeof(struct Entry));

	this->size = 0;

    check_mem(this->table);
	memset(this->table, 0, sizeof(struct Entry) * this->allocated);

	debug("starting to rehash all the %d values and insert them into the newly created table...", allocated);
	int i;
	for (i = 0; i < allocated; i++) {
		if (copiedTable[i].valid) {
			debug("inserting %d key: %s", i, copiedTable[i].key);
			HashMap_insert(this, copiedTable[i].key, copiedTable[i].value, NULL);
		}
	}
	debug("rehashing done");

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
	hashMap->seed = 5381;
	hashMap->collisions = 0;

    hashMap->table = calloc(hashMap->allocated, sizeof(struct Entry));
	check_mem(hashMap->table);
	return hashMap;

error:
    return NULL;

}

// static unsigned long
// hash_B(unsigned char *str, unsigned long a, unsigned long p, unsigned long seed) {
//
// 	unsigned long h = seed;
// 	unsigned int i;
// 	for (i = 0; i < strlen(str); ++i) {
// 		h = ((h * a) + str[i]) % p;
// 	}
// 	return h;
// }


static unsigned long
hash(unsigned char *str, unsigned long seed, unsigned long p)
{
    unsigned long hash = seed;
    int c;

    while ((c = *str++)) {

        hash = (((hash << 5) + hash) + c) % p; /* hash * 33 + c */
    }
    return hash;
}

int
HashMap_insert(struct HashMap *this, char *key, struct Value value, struct Value *existing) {

	debug("HashMap current size=%d", this->size);

    if (this->size == this->allocated) {

        check_mem(_reallocate(this, 2 * this->size));
    }

	char *localKey = malloc(sizeof(char) * (strlen(key) + 1));
	strcpy(localKey, key);
	debug("collisions= %d", this->collisions);
    unsigned long i;
    for (i = hash(key, this->seed, this->allocated); (this->table + i)->valid; i= (i+1) % this->allocated) {

		this->collisions++;
		debug("index: %d", i);
		debug("valid: %d", this->table[i].valid);

		//debug("entry %d, %s", i, this->table[i]);

        if (!strcmp(this->table[i].key, key)) {

            if (existing != NULL) {
				*existing = this->table[i].value;
				//memcpy(existing, &this->table[i].value, sizeof(struct Value));
            }
            this->table[i] = (struct Entry) {.key = key, .value = value};
			this->size++;
			return 1;
        }
    }
    this->table[i] = (struct Entry) {.key = key, .value = value, .valid = 1};
	debug("table %d : (key=%s, valid=%d)", i, this->table[i].key, this->table[i].valid);
#ifdef NDEBUG
	printValue(this->table[i].value);
	printf("\n");
#endif

	this->size++;

    return 0;
error:
    return -1;
}

struct Value *HashMap_find(struct HashMap *this, char *key) {

    int i;
    for (i = _hashMe(this->allocated, key, this->seed); (this->table + i)->valid; i= (i+1) % this->allocated) {

		debug("hash=%d, haystack=%s, needle=%s", i, this->table[i].key, key);

        if (!strcmp(this->table[i].key, key)) {
            return &(this->table + i)->value;
        }
    }
    return NULL;
}

struct Value *HashMap_remove(struct HashMap *this, char *key) {

	(void) this;
	(void) key;

	sentinel("Not implemented exception: before implementation need to care about the correct wrap around!!!");

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

	int i;
    hashMap->size = 0;
    hashMap->allocated = 0;
    free(hashMap->table);
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
