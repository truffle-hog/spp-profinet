
#include <stdlib.h>
#include <stdint.h>

#include "dissect/DissectionUtils.h"
#include "dbg.h"

const char
*valueToString(uint32_t value, const value_string *vs, const char *defaultValue) {



    check(vs != NULL, "the provided Valuestring is null");

    if (vs[value].value == value && vs[value].strptr) {
        return vs[value].strptr;
    }

    int i = 0;
    while (vs[i].strptr) {
        if (vs[i].value == value) {
            return(vs[i].strptr);
        }
        i++;
    }
error:
    return defaultValue;
}
