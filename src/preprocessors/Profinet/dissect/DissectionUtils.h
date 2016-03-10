#ifndef __DISSECTION_UTILS_H__
#define __DISSECTION_UTILS_H__

typedef struct _value_string {
    uint32_t      value;
    const char *strptr;
} value_string;

const char *valueToString(uint32_t value, const value_string *vs, const char *defaultValue);

#endif
