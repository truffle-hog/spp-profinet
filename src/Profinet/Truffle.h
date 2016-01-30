#ifndef __TRUFFLE_H__
#define __TRUFFLE_H__


struct Truffle;
typedef struct Truffle Truffle_t;

/**
* Creates a new truffle that can be filled with data from the package.
*/
Truffle_t *Truffle_new();

/**
* Returns the entry that is mapped to the given key in this truffle.
*
* @param this the calling truffle
* @param key the key to be looking for
*/
Entry_t *Truffle_get(Truffle_t *this, char* key);

/**
* Inserts the given entry into the truffle mapped to the key.
*
* @param this the calling truffle
* @param key the value to map the entry to
* @param entry the entry to be put into the truffle
*/
Entry_t *Truffle_put(Truffle_t *this, char* key, Entry_t entry);
