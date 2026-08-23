#ifndef _UTILS_H
#define _UTILS_H

#include <stddef.h>
#include <stdint.h>
#include "arena.h"

typedef struct str_slice_t {
    const char* str;
    uint32_t len;
} str_slice_t;

//char* substring(char* str, size_t idx, size_t len, arena_t* str_alloc);

#endif //_UTILS_H
