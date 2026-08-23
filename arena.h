#ifndef _ARENA_H
#define _ARENA_H

#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

typedef struct arena_region_t arena_region_t;
struct arena_region_t {
    arena_region_t* next;
    uint8_t* ptr;
    uint8_t* end;
    uint8_t data[];
};

typedef struct {
    arena_region_t* first, *cur;
    size_t def_region_size; 
} arena_t;

arena_t* arena_new(size_t def_region_size);
void* arena_alloc(arena_t* a, size_t size);
void arena_reset(arena_t* a);
void arena_free(arena_t* a);

#endif // _ARENA_H
