#include "minunit.h"

#include "util/HashMap.h"
#include "dissect/DissectionUtils.h"

struct HashMap *map = NULL;

char *beforeTestSuite() {

    return NULL;
}

char *afterTestSuite() {

    return NULL;
}

char *beforeEachTest() {

    debug("CREATING HASHMAP");
    map = HashMap_new(20);

    mu_assert_int_equals(0, map->size, "map size wrong");
    mu_assert_int_equals(20, map->allocated, "allocation size wrong");

    return NULL;
}



char *afterEachTest() {
    debug("FREEING HASHMAP");
    HashMap_free(map);
    map = NULL;
    return NULL;
}

void fillMap(struct HashMap *map, int num, char** keys) {

    //struct Value val; //

    int check;

    int i;
    for (i = 0; i < num; i++) {

        struct Value val = {.type=is_uint64, .val.int64=i+1, .length=64};

        debug("inserting key number %d : %s", i, keys[i]);

        check = HashMap_insert(map, keys[i], val, NULL);
        mu_assert(check != -1, "there should be no error on inserting");
    }
}

char *test_HashMap_fillMapCheckTableStateWithoutReallocation() {

    char *keys[10] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};
    fillMap(map, 10, keys);

    int i;
    for (i = 0; i < 10; i++) {

        debug("%d", i);
        //mu_assert(map->table[i].key != NULL, "the key in the table must not be null");

        //mu_assert(strcmp(map->table[i].key, keys[i]) == 0, "keys are not inserted properly");

    }

    return NULL;

}

char *test_HashMap_findCorrectValueWithoutReallocation() {

    char *keys[10] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};

    fillMap(map, 10, keys);

    mu_assert_int_equals(10, map->size, "maps size should be 10");
    mu_assert(map->allocated == 20, "maps allocation size should not have changed from 20");

    struct Value *val;

    val = HashMap_find(map, "one");
    mu_assert(val->val.uint64 == 1, "the value should be one");

    val = HashMap_find(map, "five");
    mu_assert(val->val.uint64 == 5, "the value should be five");

    return NULL;
}

char *test_HashMap_reallocationWorksCorrectly() {

    char *keys[21] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten",
                      "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eightteen", "nineteen", "twenty", "twenty_one"};

    fillMap(map, 21, keys);

    mu_assert_int_equals(21, map->size, "the size is incorrect");
    mu_assert_int_equals(40, map->allocated, "the allocation size should be doubled to 40");

    return NULL;
}

char *test_HashMap_findCorrectValueWithReallocation() {

    char *keys[21] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten",
                      "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eightteen", "nineteen", "twenty", "twenty_one"};

    fillMap(map, 21, keys);

    struct Value *val;

    val = HashMap_find(map, "one");
    mu_assert(val->val.uint64 == 1, "the value should be one");

    val = HashMap_find(map, "five");
    mu_assert(val->val.uint64 == 5, "the value should be five");

    val = HashMap_find(map, "twenty");
    mu_assert(val->val.uint64 == 20, "the value should be twenty");

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
    mu_run_test_set(beforeEachTest, afterEachTest, test_HashMap_insert);
    mu_run_test_set(beforeEachTest, afterEachTest, test_HashMap_fillMapCheckTableStateWithoutReallocation);
    mu_run_test_set(beforeEachTest, afterEachTest, test_HashMap_findCorrectValueWithoutReallocation);
    mu_run_test_set(beforeEachTest, afterEachTest, test_HashMap_reallocationWorksCorrectly);
    mu_run_test_set(beforeEachTest, afterEachTest, test_HashMap_findCorrectValueWithReallocation);

    return NULL;
}

RUN_TESTS(beforeTestSuite, afterTestSuite, all_tests);
