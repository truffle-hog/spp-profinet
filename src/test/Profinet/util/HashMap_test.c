#include "minunit.h"

#include "util/HashMap.h"
#include "dissect/DissectionUtils.h"

struct HashMap *map = NULL;

char *beforeEachTest() {

    map = HashMap_new(20);
    return NULL;
}

char *afterEachTest() {
    HashMap_free(map);
    map = NULL;
    return NULL;
}

char *test_HashMap_insert() {

    struct Value val = {.type=is_uint8, .val.int8=1, .length=0};

    int check = HashMap_insert(map, "Hallo", val , NULL);

    mu_assert(check != -1, "there should not have been any error");
    mu_assert(check != 1, "there should not have been any collisions");

    mu_assert(map->size == 1, "the size should have been increased to 1");
    mu_assert(map->allocated == 20, "the allocation size should not have changed");

    return NULL;
}

char *test_HashMap_new() {

    struct HashMap *myMap = HashMap_new(20);
    mu_assert(myMap != NULL, "Failed to create new dynamic array");

    mu_assert(myMap->allocated == 20, "Failed to set allocation count");
    mu_assert(myMap->size == 0, "Failed to set size to zero during initialization");
    mu_assert(myMap->table != NULL, "Failed to create poitner array for initial size");

    unsigned int i;
    for (i = 0; i < myMap->allocated; i++) {

        mu_assert(myMap->table[i].key == 0, "table not correctly initialized");
        mu_assert(myMap->table[i].value.length == 0, "table not correctly initialized");
        mu_assert(myMap->table[i].value.type == 0, "table not correctly initialized");
        mu_assert(myMap->table[i].value.val.uint64 == 0, "table not correctly initialized");
        mu_assert(myMap->table[i].valid == 0, "table not correctly initialized");
    }

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_HashMap_new);
    mu_run_test(test_HashMap_insert);

    return NULL;
}

RUN_TESTS(beforeEachTest, afterEachTest, all_tests);
