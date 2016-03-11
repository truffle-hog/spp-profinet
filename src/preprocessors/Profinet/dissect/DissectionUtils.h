#ifndef __DISSECTION_UTILS_H__
#define __DISSECTION_UTILS_H__

typedef struct _value_string {
    uint32_t      value;
    const char *strptr;
} value_string;

struct Value {

    /** The length of this value. **/
    int length;
    /** The different types this value can be. **/
    enum {  is_int8, is_uint8,
            is_int16, is_uint16,
            is_int32, is_uint32,
            is_int64, is_uint64,
            is_float, is_double,
            is_char, is_string
        } type;

    /** Union of all data types that can be inserted for a Value. **/
    union {
        int8_t int8;
        uint8_t uint8;
        int16_t int16;
        uint16_t uint16;
        int32_t int32;
        uint32_t uint32;
        int64_t int64;
        uint64_t uint64;
        float float32;
        double double64;
        char character;
        char *string;
    } val;
};

const char *valueToString(uint32_t value, const value_string *vs, const char *defaultValue);

#endif
