
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

void printValue(struct Value val) {


	switch(val.type) {

		case is_string:
			printf("%s", val.val.string);
			break;

		case is_char:
			printf("%c", val.val.character);
			break;

		case is_int8:
			printf("%d", val.val.int8);
			break;
		case is_int16:
			printf("%d", val.val.int16);
			break;
		case is_int32:
			printf("%d", val.val.int32);
			break;
		case is_int64:
			printf("%ld", val.val.int64);
			break;

		case is_uint8:
			printf("0x%02X", val.val.uint8);
			break;
		case is_uint16:
			printf("0x%04X", val.val.uint16);
			break;
		case is_uint32:
			printf("0x%08X", val.val.uint32);
			break;
		case is_uint64:
			printf("0x%016lX", val.val.uint64);
			break;

		case is_float:
			printf("%f", val.val.float32);
			break;
		case is_double:
			printf("%f", val.val.double64);
			break;

	}
	return;
}
