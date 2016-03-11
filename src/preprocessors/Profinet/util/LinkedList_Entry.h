#ifndef __LINKED_LIST_ENTRY_H__
#define __LINKED_LIST_ENTRY_H__

struct Item {

    struct Entry element;
    struct Item *next;
    struct Item *prev;

};


struct LinkedList_Entry {
    struct Item *head;
};

struct LinkedList_Entry *LinkedList_Entry_new(size_t initial);

bool LinkedList_Entry_isEmpty(struct LinkedList_Entry *this);

struct Entry LinkedList_Entry_first(struct LinkedList_Entry *this);

struct Entry LinkedList_Entry_last(struct LinkedList_Entry *this);

void LinkedList_splice(struct LinkedList_Entry *this, struct Item *a, struct Item *b, struct Item *t);

void LinkedList_Entry_moveAfter(struct LinkedList_Entry *this, struct Item *b, struct Item *a);

void LinkedList_Entry_moveToFront(struct LinkedList_Entry *this, )

int LinkedList_Entry_pushBack(struct LinkedList_Entry *this, struct Entry element);

void *LinkedList_Entry_popBack(struct LinkedList_Entry *this);

void *LinkedList_Entry_get(struct LinkedList_Entry *this, int index);

int LinkedList_Entry_forEach(struct LinkedList_Entry *this, int (*doThis)(struct Entry entry, void *args, void *ret), void *args, void *ret);

void LinkedList_Entry_free(struct LinkedList_Entry *dArray);

int LinkedList_Entry_clear(struct LinkedList_Entry *this);

#endif
