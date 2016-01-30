#ifndef __ENTRY_H__
#define __ENTRY_H__

#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1

struct Entry {

  char stringValue[256];
  uint64_t intValue;
  int endianess;
};
typedef struct Entry Entry_t;

/**
* Creates a new entry.
*
* @return the newly created entry
*/
Entry_t *Entry_new();

/**
* @brief Returns the entry that is mapped to the given key in this truffle.
*
* @param this the calling truffle
* @param key the key to be looking for
*/
Entry_t *Truffle_get(Truffle_t *this, char* key);

/**
* @brief Inserts the given entry into the truffle mapped to the key.
*
* @param this the calling truffle
* @param key the value to map the entry to
* @param entry the entry to be put into the truffle
*
* @return NULL if there was no previous entry mapped to the given key,
*         the existing entry otherwise - which will be overwritten
*/
Entry_t *Truffle_putChars(Truffle_t *this, char* key, char* value);

Entry_t *Truffle_putInt(Truffle_t *this, char* key, uint64_t value);
